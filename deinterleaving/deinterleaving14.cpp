#include "stdafx.h"

#include <algorithm>
#include <cassert>
#include <cstring>

#include <omp.h>

#include "Block.h"

extern "C" __declspec(dllexport) void deinterleave(
	void* input_array /* uint8_t[112 + 16][heigth] */,
	const int32_t heigth,
	/* out */ void* output_array /* uint8_t[112][heigth] */,
	int32_t max_threads
) {
	const auto line_shift = 16;

	const auto N = max_threads <= 0
		? omp_get_max_threads()
		: std::min<decltype(omp_get_max_threads())>(max_threads, omp_get_max_threads());

	BlockAccessor<Block14> src(input_array, heigth, Block14::BlocksPerLine * Block14::bits_per_block + sizeof(uint16_t) * CHAR_BIT);
	BlockAccessor<Block14> dest(output_array, heigth + line_shift * (Block14::BlocksPerLine - 1),
		Block14::BlocksPerLine * Block14::bits_per_block);


#pragma omp parallel for num_threads(N)
	for (auto column = 0; column < Block14::BlocksPerLine; ++column) {
		auto dest_element = line_shift * (Block14::BlocksPerLine - column - 1);
		for (auto element = 0; element < heigth; ++element, ++dest_element) {
			auto _src = src.getBlock(element, column);
			auto _dest = dest.getBlock(dest_element, column);

			BlockAccessor<Block14>::copy(_dest, _src);
		}
	}
}

