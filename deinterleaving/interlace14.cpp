#include "stdafx.h"

#include <omp.h>

#include <algorithm>
#include <cassert>

#include "Block.h"

extern "C" __declspec(dllexport) void deinterlace(
	void* input_array, /* uint8_t[112 + 16][heigth] */
	int32_t heigth,
	/* out */ void* output_array, /* uint8_t[112 + 16][heigth] */
	int32_t max_threads,
	uint8_t even_odd_first
) {
	const auto N = max_threads <= 0
		? omp_get_max_threads()
		: std::min<decltype(omp_get_max_threads())>(max_threads, omp_get_max_threads());

	constexpr auto bits_count = Block14::BlocksPerLine * Block14::bits_per_block + sizeof(uint16_t) * CHAR_BIT;

	BlockAccessor<Block14> src(input_array, heigth, bits_count);
	BlockAccessor<Block14> dest(output_array, heigth, bits_count);

#pragma omp parallel for num_threads(N)
	for (auto line = 0; line < heigth; ++line) {
		if ((line & 1) ^ !!even_odd_first) {
			// нечетная строка
			std::memcpy(dest.getBlock((line + heigth) / 2, 0), src.getBlock(line, 0), bits_count);
		}
		else {
			// четная строка
			std::memcpy(dest.getBlock(line / 2, 0), src.getBlock(line, 0), bits_count);
		}
	}
}

extern "C" __declspec(dllexport) void interlace(
	void* input_array, /* uint8_t[112 + 16][heigth] */
	const int32_t heigth,
	/* out */ void* output_array, /* uint8_t[112 + 16][heigth] */
	int32_t max_threads,
	uint8_t even_odd_first
) {
	const auto N = max_threads <= 0
		? omp_get_max_threads()
		: std::min<decltype(omp_get_max_threads())>(max_threads, omp_get_max_threads());

	constexpr auto bits_count = Block14::BlocksPerLine * Block14::bits_per_block + sizeof(uint16_t) * CHAR_BIT;

	BlockAccessor<Block14> src(input_array, heigth, bits_count);
	BlockAccessor<Block14> dest(output_array, heigth, bits_count);

#pragma omp parallel for num_threads(N)
	for (auto line = 0; line < heigth; ++line) {
		if ((line & 1) ^ !!even_odd_first) {
			// нечетная строка
			std::memcpy(dest.getBlock(line, 0), src.getBlock((line + heigth) / 2, 0), bits_count);
		}
		else {
			// четная строка
			std::memcpy(dest.getBlock(line, 0), src.getBlock(line / 2, 0), bits_count);
		}
	}
}
