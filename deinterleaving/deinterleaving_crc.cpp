#include "stdafx.h"

#include <algorithm>
#include <cassert>
#include <cstring>

#include <omp.h>

static constexpr auto columns = 8;
static constexpr auto vertical_shift = 16;

extern "C" __declspec(dllexport) void deinterleave_crc(
	uint8_t* input_array /* uint8_t[heigth] */,
	const int32_t heigth,
	/* out */ uint8_t* output_array /* uint8_t[heigth + 112][columns] */,
	int32_t max_threads
) {
	const auto N = max_threads <= 0
		? omp_get_max_threads()
		: std::min<decltype(omp_get_max_threads())>(max_threads, omp_get_max_threads());

	//output_array[row * 8 + column];
#pragma omp parallel for num_threads(N)
	for (auto column = 0; column < columns; ++column) {
		auto row_offset = (columns - 1 - column) * vertical_shift;
		for (auto y = 0; y < heigth; ++y) {
			output_array[(row_offset + y) * columns + column] = input_array[y];
		}
	}
}
