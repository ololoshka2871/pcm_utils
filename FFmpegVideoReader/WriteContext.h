#pragma once

#include <string>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libavutil/avassert.h>
#include <libavutil/frame.h>
#include <libavutil/hwcontext.h>
#include <libavutil/imgutils.h>
#include <libavutil/motion_vector.h>
#include <libavutil/opt.h>
#include <libavutil/pixdesc.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
}

struct OutputStream {
  AVStream *st;
  AVCodecContext *enc;

  /* pts of the next frame that will be generated */
  int64_t next_pts;
  int samples_count;

  AVFrame *frame;

  float t, tincr, tincr2;

  struct SwsContext *sws_ctx;
  struct SwrContext *swr_ctx;
};

struct FfmpegWriteContext {
  FfmpegWriteContext(const std::string &filename, const std::string &codec_name,
                     int32_t heigth, int32_t width, int64_t bitrate, bool PAL,
                     int32_t src_buf_width);
  ~FfmpegWriteContext();

  void encode_frame(uint8_t *pixel_data);

private:
  AVFormatContext *oc;
  AVOutputFormat *fmt;
  AVCodec *codec;
  AVDictionary *opt;
  AVFrame *tframe;
  SwsContext *sws_ctx;

  OutputStream video_st;
};
