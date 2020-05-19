#include "stdafx.h"

#include <algorithm>
#include <cassert>
#include <cstring>

#include <omp.h>

#include "Block.h"

extern "C" __declspec(dllexport) void deinterleave16(
	void* input_array /* uint8_t[112 + 16][heigth] */,
	const int32_t heigth,
	/* out */ void* output_array /* uint8_t[128][heigth] */,
	int32_t max_threads
) {
	const auto line_shift = 16;

	const auto N = max_threads <= 0
		? omp_get_max_threads()
		: std::min<decltype(omp_get_max_threads())>(max_threads, omp_get_max_threads());

	BlockAccessor<Block16> src(input_array, heigth, Block16::BlocksPerLine * Block16::bits_per_block);
	BlockAccessor<Block16> dest(output_array, heigth + line_shift * (Block16::BlocksPerLine - 1),
		Block16::BlocksPerLine * Block16::bits_per_block);


#pragma omp parallel for num_threads(N)
	for (auto column = 0; column < Block16::BlocksPerLine; ++column) {
		auto dest_element = line_shift * (Block16::BlocksPerLine - column - 1);
		for (auto element = 0; element < heigth; ++element, ++dest_element) {
			auto _src = src.getBlock(element, column);
			auto _dest = dest.getBlock(dest_element, column);

			BlockAccessor<Block16>::copy(_dest, _src);
		}
	}
}

extern "C" __declspec(dllexport) void deinterleave16Plus(
	void* input_array /* uint8_t[112 + 16][heigth] */,
	const int32_t heigth,
	/* out */ void* output_array /* uint8_t[128][heigth] */,
	int32_t max_threads
) {
	const auto line_shift = 16;
	const auto src2bit_offset = 98 / Block2::bits_per_block;

	const auto N = max_threads <= 0
		? omp_get_max_threads()
		: std::min<decltype(omp_get_max_threads())>(max_threads, omp_get_max_threads());

	BlockAccessor<Block14> src(input_array, heigth, Block14::BlocksPerLine * Block14::bits_per_block + sizeof(uint16_t) * CHAR_BIT);
	BlockAccessor<Block2> src2bit(input_array, heigth, Block2::BlocksPerLine * Block2::bits_per_block + sizeof(uint16_t) * CHAR_BIT);
	BlockAccessor<Block16> dest(output_array, heigth + line_shift * (Block16::BlocksPerLine - 1),
		Block16::BlocksPerLine * Block16::bits_per_block);


#pragma omp parallel for num_threads(N)
	for (auto column = 0; column < Block16::BlocksPerLine; ++column) {
		auto dest_element = line_shift * (Block16::BlocksPerLine - column - 1);
		for (auto string = 0; string < heigth; ++string, ++dest_element) {
			auto _src = src.getBlock(string, column);
			auto add2bit_src = src2bit.getBlock(string, src2bit_offset + column);

			auto _dest = dest.getBlock(dest_element, column);
			
			// copy 14 bit data
			auto add_dest = BlockAccessor<Block14>::copy(_dest, _src);

			// copy 2 bit from Q-area
			BlockAccessor<Block2>::copy(add_dest, add2bit_src);
		}
	}
}