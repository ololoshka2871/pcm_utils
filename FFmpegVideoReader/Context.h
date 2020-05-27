#pragma once

#include <string>
#include <exception>
#include <tuple>

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

struct LStrHandle {
	uint32_t len;
	char symbols[];
};

struct FfmpegContext {
	FfmpegContext(const std::string &filename, const std::string &protocol = "");
	~FfmpegContext();

	std::tuple<int32_t, int32_t> get_frame_size() const;
	int32_t get_frame_count() const;
	int skip_frames(int64_t frame_count);
	void print_timestamp_to(char *s) const;

	static void av_device_register_all();

	int64_t read_frame_to(uint8_t* pixel_data);

	std::string getFilename() const { return filename; }
	std::string getProtocol() const { return protocol; }

private:
	void read_next_video_pocket();
	void unref_current_pocket();

	std::string filename;
	std::string protocol;

	AVFormatContext* pFormatContext;
	AVInputFormat *iformat;
	AVCodec *pCodec;
	AVCodecParameters *pCodecParameters;
	AVCodecContext *pCodecContext;
	AVFrame *pFrame;
	AVPacket *pPacket;

	uint32_t video_stream_index;

	AVFrame* pFrameYUV;
	uint8_t* YUVbuffer;
	SwsContext* sws_ctx;
};