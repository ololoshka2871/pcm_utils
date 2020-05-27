#pragma once

#include <cstdint>
#include <exception>

struct FFmpegException : std::exception {
	FFmpegException() = default;
	FFmpegException(int32_t code) : error(code) {}

	int32_t errorCode() const { return error; }
private:
	int32_t error;
};
