#pragma once

#include <string>
#include <exception>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavformat/avio.h>
#include <libavutil/pixdesc.h>
#include <libavutil/hwcontext.h>
#include <libavutil/opt.h>
#include <libavutil/avassert.h>
#include <libavutil/imgutils.h>
#include <libavutil/motion_vector.h>
#include <libavutil/frame.h>
#include <libavfilter/avfilter.h>
}

struct FFmpegException : std::exception {
	FFmpegException() = default;
	FFmpegException(int32_t code) : error(code) {}

	int32_t errorCode() const {	return error; }
private:
	int32_t error;
};

struct FfmpegContext {
	FfmpegContext(const std::string &filename);

private:
	AVFormatContext* pFormatContext;
	AVCodec *pCodec;
	AVCodecParameters *pCodecParameters;
	AVCodecContext *pCodecContext;
};