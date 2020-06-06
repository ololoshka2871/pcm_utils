#include <string>
#include <cstring>

#include "WriteContext.h"
#include "FFmpegException.h"

#ifdef _MSC_VER
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif

// https://github.com/FFmpeg/FFmpeg/blob/master/doc/examples/transcoding.c
// https://github.com/FFmpeg/FFmpeg/blob/master/doc/examples/muxing.c
// http://ffmpeg.org/doxygen/trunk/filtering_8c-source.html

#define STREAM_PIX_FMT    AV_PIX_FMT_GRAY8 /* default pix_fmt */

/* Add an output stream. */
static void add_stream(OutputStream *ost, AVFormatContext *oc,	AVCodec **codec, const std::string &codec_id,
	int32_t heigth, int32_t width, int64_t bitrate, AVRational& timebase, AVRational& fps) {
	AVCodecContext *c;
	int i;

	/* find the encoder */
	*codec = avcodec_find_encoder_by_name(codec_id.c_str());
	if (!(*codec)) {
		throw FFmpegException{ AVERROR_BUG };
	}

	ost->st = avformat_new_stream(oc, NULL);
	if (!ost->st) {
		throw FFmpegException{ AVERROR_BUG };
	}

	ost->st->id = oc->nb_streams - 1;
	c = avcodec_alloc_context3(*codec);
	if (!c) {
		throw FFmpegException{ AVERROR_BUG };
	}
	ost->enc = c;

	if ((*codec)->type == AVMEDIA_TYPE_VIDEO) {
		c->codec_id = (*codec)->id;

		c->bit_rate = bitrate;
		/* Resolution must be a multiple of two. */
		c->width = width;
		c->height = heigth;
		/* timebase: This is the fundamental unit of time (in seconds) in terms
		 * of which frame timestamps are represented. For fixed-fps content,
		 * timebase should be 1/framerate and timestamp increments should be
		 * identical to 1. */
		c->time_base = ost->st->time_base = timebase;
		c->framerate = fps;

		c->gop_size = 12; /* emit one intra frame every twelve frames at most */
		c->pix_fmt = STREAM_PIX_FMT;
		c->max_b_frames = (c->codec_id == AV_CODEC_ID_MPEG2VIDEO) 
			? 1 : 2; /* just for testing, we also add B-frames */

		if (c->codec_id == AV_CODEC_ID_MPEG1VIDEO) {
			/* Needed to avoid using macroblocks in which some coeffs overflow.
			 * This does not happen with normal video, it just happens here as
			 * the motion of the chroma plane does not match the luma plane. */
			c->mb_decision = 2;
		}
	} else {
		throw FFmpegException{ AVERROR_BUG };
	}

	/* Some formats want stream headers to be separate. */
	if (oc->oformat->flags & AVFMT_GLOBALHEADER) {
		c->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
	}
}

static int write_frame(AVFormatContext *fmt_ctx, AVCodecContext *c,	AVStream *st, AVFrame *frame) {
	int ret;

	// send the frame to the encoder
	ret = avcodec_send_frame(c, frame);
	if (ret < 0) {
		throw FFmpegException{ ret };
	}

	while (ret >= 0) {
		AVPacket pkt{};

		ret = avcodec_receive_packet(c, &pkt);
		if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
			break;
		} else if (ret < 0) {
			throw FFmpegException{ ret };
		}

		/* rescale output packet timestamp values from codec to stream timebase */
		av_packet_rescale_ts(&pkt, c->time_base, st->time_base);
		pkt.stream_index = st->index;

		/* Write the compressed frame to the media file. */
		ret = av_interleaved_write_frame(fmt_ctx, &pkt);
		av_packet_unref(&pkt);
		if (ret < 0) {
			throw FFmpegException{ ret };
		}
	}

	return ret == AVERROR_EOF ? 1 : 0;
}

static AVFrame *alloc_picture(enum AVPixelFormat pix_fmt, int width, int height)
{
	AVFrame *picture;
	int ret;

	picture = av_frame_alloc();
	if (!picture)
		return NULL;

	picture->format = pix_fmt;
	picture->width = width;
	picture->height = height;

	/* allocate the buffers for the frame data */
	ret = av_frame_get_buffer(picture, 0);
	if (ret < 0) {
		throw FFmpegException{ ret };
	}

	return picture;
}

static void open_video(AVFormatContext *oc, AVCodec *codec, OutputStream *ost, AVDictionary *opt_arg)
{
	int ret;
	AVCodecContext *c = ost->enc;
	AVDictionary *opt = NULL;

	av_dict_copy(&opt, opt_arg, 0);

	/* open the codec */
	ret = avcodec_open2(c, codec, &opt);
	av_dict_free(&opt);
	if (ret < 0) {
		throw FFmpegException{ ret };
	}

	/* allocate and init a re-usable frame */
	ost->frame = alloc_picture(c->pix_fmt, c->width, c->height);
	if (!ost->frame) {
		throw FFmpegException{ AVERROR_BUG };
	}

	/* copy the stream parameters to the muxer */
	ret = avcodec_parameters_from_context(ost->st->codecpar, c);
	if (ret < 0) {
		throw FFmpegException{ ret };
	}
}

static void close_stream(AVFormatContext *oc, OutputStream *ost) {
	avcodec_free_context(&ost->enc);
	av_frame_free(&ost->frame);
	sws_freeContext(ost->sws_ctx);
	swr_free(&ost->swr_ctx);
}

extern "C" DLLEXPORT void FfmpegCreateWriteContext(char* filename, char* codec,
	int32_t heigth, int32_t width, int64_t bitrate, uint8_t PAL, int32_t src_buf_width, void **pCtx, int *error_code) {
	try {
		*pCtx = new FfmpegWriteContext{ std::string(filename), std::string(codec), heigth, width, bitrate, !!PAL, src_buf_width };
		*error_code = 0;
	}
	catch (FFmpegException ex) {
		*error_code = ex.errorCode();
	}
}

extern "C" DLLEXPORT void FfmpegDestroyWriteContext(void** pCtx) {
	auto _ctx = static_cast<FfmpegWriteContext*>(*pCtx);
	delete _ctx;
}

extern "C" DLLEXPORT void WriteFrame(void** pCtx,
	/* in */  uint8_t* frame_data,
	/* out */ int *error_code) {
	try {
		auto _ctx = static_cast<FfmpegWriteContext*>(*pCtx);
		_ctx->encode_frame(frame_data);
	}
	catch (FFmpegException ex) {
		*error_code = ex.errorCode();
	}
}

FfmpegWriteContext::FfmpegWriteContext(const std::string & filename, 
	const std::string & codec_name, int32_t heigth, int32_t width, int64_t bitrate, 
	bool PAL, int32_t src_buf_width) : video_st{} 
{
	int ret;
	AVDictionary *opt = NULL;

	/* allocate the output media context */
	ret = avformat_alloc_output_context2(&oc, NULL, NULL, filename.c_str());
	if (ret < 0) {
		throw FFmpegException{ ret };
	}

	fmt = oc->oformat;

	/* frames per second */
	AVRational framerate{ 30000, 1001 }, timebase{ 1001, 30000 }; // default NTSC 29.97 fps
	if (PAL) {
		// 25 fps
		timebase = AVRational{ 1, 25 };
		framerate = AVRational{ 25, 1 };
	}
	
	if (fmt->video_codec != AV_CODEC_ID_NONE) {
		add_stream(&video_st, oc, &codec, codec_name, heigth, width, bitrate, timebase, framerate);
	}

	open_video(oc, codec, &video_st, opt);

	ret = avio_open(&oc->pb, filename.c_str(), AVIO_FLAG_WRITE);
	if (ret < 0) {
		throw FFmpegException{ ret };
	}

	/* Write the stream header, if any. */
	ret = avformat_write_header(oc, &opt);
	if (ret < 0) {
		throw FFmpegException{ ret };
	}

	tframe = alloc_picture(AV_PIX_FMT_GRAY8, src_buf_width, heigth);
	if (tframe == nullptr) {
		throw FFmpegException{ ENOMEM };
	}

	/* create scaling context */
	sws_ctx = sws_getContext(src_buf_width, heigth, AV_PIX_FMT_GRAY8,
		width, heigth, (AVPixelFormat)video_st.frame->format,
		SWS_BILINEAR, NULL, NULL, NULL);
	if (!sws_ctx) {
		throw FFmpegException{ AVERROR_BUG };
	}
}

FfmpegWriteContext::~FfmpegWriteContext() {
	av_frame_free(&tframe);
	sws_freeContext(sws_ctx);
	av_write_trailer(oc);
	close_stream(oc, &video_st);
	avio_closep(&oc->pb);
}

void FfmpegWriteContext::encode_frame(uint8_t* pixel_data) {
	int ret;

	/* make sure the frame data is writable */
	ret = av_frame_make_writable(tframe);
	if (ret < 0) {
		throw FFmpegException{ ret };
	}

	auto linesize = tframe->linesize[0];
	for (auto y = 0; y < tframe->height; y++) {
		auto offset_dest = y * linesize;
		auto offset_src = y * tframe->width;
		std::memcpy(tframe->data[0] + offset_dest, pixel_data + offset_src, linesize);
	}

	sws_scale(sws_ctx, // filter
		(const uint8_t * const*)tframe->data, // src data
		tframe->linesize, // line sizes of src
		0,
		tframe->height, // heigth of image
		video_st.frame->data, // destination data
		video_st.frame->linesize // line sizes of dst
	);

	write_frame(oc, video_st.enc, video_st.st, video_st.frame);
}
