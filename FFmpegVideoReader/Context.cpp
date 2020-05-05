
#include "stdafx.h"

#include "Context.h"

FfmpegContext::FfmpegContext(const std::string &filename) {
	int32_t res;

	pFormatContext = avformat_alloc_context();
	if (!pFormatContext) {
		throw FFmpegException{ AVERROR_BUG };
	}


	res = avformat_open_input(&pFormatContext, filename.c_str(), NULL, NULL);
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
}
