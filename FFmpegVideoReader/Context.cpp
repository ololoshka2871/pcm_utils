
#include "stdafx.h"

#include <cmath>

#include "Context.h"

extern "C"
{
#include <libavdevice/avdevice.h>
}

#define snprintf(buf,len, format,...) _snprintf_s(buf, len,len, format, __VA_ARGS__)

FfmpegContext::FfmpegContext(const std::string &filename, const std::string &protocol) 
	: filename(filename), protocol(protocol), iformat{nullptr} {
	int32_t res;

	pFormatContext = avformat_alloc_context();
	if (!pFormatContext) {
		throw FFmpegException{ AVERROR_BUG };
	}

	std::string f(filename);
	if (!protocol.empty()) {
		av_device_register_all();
		iformat = av_find_input_format(protocol.c_str());
		if (iformat == nullptr) {
			throw FFmpegException{ AVERROR_BUG };
		}
		f = "video=" + filename;
	}
	
	res = avformat_open_input(&pFormatContext, f.c_str(), iformat, NULL);
	if (res < 0) {
		throw FFmpegException{ res };
	}
		
	res = avformat_find_stream_info(pFormatContext, NULL);
	if (res < 0) {
		throw FFmpegException{res};
	}

	for (unsigned int i = 0; i < pFormatContext->nb_streams; i++) {
		auto pLocalCodecParameters = pFormatContext->streams[i]->codecpar;
		auto pLocalCodec = avcodec_find_decoder(pLocalCodecParameters->codec_id);
		if (pLocalCodec == NULL) {
			throw FFmpegException{ AVERROR_BUG };
		}

		// when the stream is a video we store its index, codec parameters and codec
		if (pLocalCodecParameters->codec_type == AVMEDIA_TYPE_VIDEO) {
			video_stream_index = i;
			pCodec = pLocalCodec;
			pCodecParameters = pLocalCodecParameters;
			break;
		}
	}

	pCodecContext = avcodec_alloc_context3(pCodec);
	if (!pCodecContext)	{
		throw FFmpegException{ AVERROR_BUG };
	}

	res = avcodec_parameters_to_context(pCodecContext, pCodecParameters);
	if (res < 0) {
		throw FFmpegException{ res };
	}

	res = avcodec_open2(pCodecContext, pCodec, NULL);
	if (res < 0) {
		throw FFmpegException{ res };
	}

	pFrame = av_frame_alloc();
	if (pFrame == nullptr) {
		throw FFmpegException{ AVERROR_BUG };
	}

	pPacket = av_packet_alloc();
	if (pPacket == nullptr) {
		throw FFmpegException{ AVERROR_BUG };
	}

	/**********************************/
	pFrameYUV = av_frame_alloc();
	if (pFrameYUV == nullptr) {
		throw FFmpegException{ AVERROR_BUG };
	}
	std::tie(pFrameYUV->width, pFrameYUV->height) = get_frame_size();
	pFrameYUV->format = AV_PIX_FMT_YUV420P;

	auto numBytes = av_image_get_buffer_size(AV_PIX_FMT_YUV420P, pFrameYUV->width, pFrameYUV->height, 1);
	if (numBytes < 0) {
		throw FFmpegException{ AVERROR_BUG };
	}

	YUVbuffer = (uint8_t *)av_malloc(numBytes);
	if (YUVbuffer == nullptr) {
		throw FFmpegException{ AVERROR_BUG };
	}

	av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize, YUVbuffer, AV_PIX_FMT_YUV420P, 
		pFrameYUV->width, pFrameYUV->height, 1);

	sws_ctx = sws_getContext(pCodecContext->width,
		pCodecContext->height,
		pCodecContext->pix_fmt,
		pCodecContext->width,
		pCodecContext->height,
		AV_PIX_FMT_YUV420P,
		SWS_BILINEAR,
		NULL,
		NULL,
		NULL
	);

	if (sws_ctx == nullptr) {
		throw FFmpegException{ AVERROR_BUG };
	}
}

FfmpegContext::~FfmpegContext() {
	sws_freeContext(sws_ctx);
	av_free(YUVbuffer);
	av_frame_free(&pFrameYUV);
	avformat_close_input(&pFormatContext);
	av_packet_free(&pPacket);
	av_frame_free(&pFrame);
	avcodec_free_context(&pCodecContext);
}

std::tuple<int32_t, int32_t> FfmpegContext::get_frame_size() const {
	return std::make_tuple(pCodecParameters->width, pCodecParameters->height);
}

int32_t FfmpegContext::get_frame_count() const {
	return floor(pFormatContext->duration * av_q2d(pFormatContext->streams[video_stream_index]->r_frame_rate) / 1000000.0);
}

void FfmpegContext::read_next_video_pocket() {
	while (1) {
		auto res = av_read_frame(pFormatContext, pPacket);
		if (res < 0) {
			throw FFmpegException{ res };
		}

		// if it's the video stream
		if (pPacket->stream_index == video_stream_index) {
			break;
		}
		unref_current_pocket();
	}
}

void FfmpegContext::av_device_register_all() {
	avdevice_register_all();
}

int64_t FfmpegContext::read_frame_to(uint8_t* pixel_data) {
	while (1) {
		auto res = avcodec_receive_frame(pCodecContext, pFrame);
		if (res == AVERROR(EAGAIN)) {
			read_next_video_pocket();
			res = avcodec_send_packet(pCodecContext, pPacket);
			if (res != 0) {
				throw FFmpegException{ res };
			}
			unref_current_pocket();
			continue;
		}
		if (res < 0) {
			throw FFmpegException{ res };
		}
		break;
	}

	sws_scale(sws_ctx, (uint8_t const * const *)pFrame->data, pFrame->linesize, 0,
		pCodecContext->height, pFrameYUV->data, pFrameYUV->linesize);

	// теперь в pFrameYUV->data[0] лежать Y для всех пикселей кадра
	std::memcpy(pixel_data, pFrameYUV->data[0], pFrameYUV->width * pFrameYUV->height * sizeof(uint8_t));
	return int64_t(pFrame->pts * av_q2d(pFormatContext->streams[video_stream_index]->time_base) 
		* av_q2d(pFormatContext->streams[video_stream_index]->r_frame_rate));
}

void FfmpegContext::unref_current_pocket() {
	av_packet_unref(pPacket);
}

int FfmpegContext::skip_frames(int64_t frame_count) {
	auto fps = av_q2d(pFormatContext->streams[video_stream_index]->r_frame_rate);
	auto frame_time_tb = AV_TIME_BASE / fps;

	return av_seek_frame(pFormatContext, -1, int64_t(frame_count * frame_time_tb), 0);
}

void FfmpegContext::print_timestamp_to(char* s) const {
	auto time_s = pFrame->pts * av_q2d(pFormatContext->streams[video_stream_index]->time_base);

	if (time_s > 0) {
		int hour = std::floor(time_s / (60 * 60));

		int minute = std::floor((time_s - (hour * 60 * 60)) / 60);

		int seconds = std::floor(time_s - (hour * 60 * 60) - (minute * 60));

		auto t = time_s - (hour * 60 * 60) - (minute * 60) - seconds;

		t *= 1000;

		snprintf(s, 13, "%02d:%02d:%02d.%03d", hour, minute, seconds, int(t));
	} else {
		snprintf(s, 13, "Invalid");
	}
}