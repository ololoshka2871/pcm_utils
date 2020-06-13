#pragma once

extern "C" {
#include "libavformat/avformat.h"
}

#include <cstdint>
#include <exception>
#include <vector>

struct FFmpegException : std::exception {
  FFmpegException() = default;
  FFmpegException(int32_t code) : error(code), text(1024) {}

  int32_t errorCode() const { return error; }

  // http://ffmpeg.org/doxygen/trunk/group__lavu__error.html#ga5792b4a2d18d7d9cb0efbcfc335dce24
  const char *what() const noexcept override {
    av_strerror(error, (char *)text.data(), (size_t)text.size());
    return text.data();
  }

private:
  int32_t error;
  std::vector<char> text;
};
