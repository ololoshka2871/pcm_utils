#pragma once

#include <cstdint>

extern "C"  __declspec(dllexport) void deemphasis(float In_Old, float X1, float *In,
	float *Out, uint32_t len, float *In_Old_OUT, float *X1_OUT, float sampleRate);