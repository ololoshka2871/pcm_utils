#include "stdafx.h"

#include <algorithm>
#include <cassert>
#include <cstring>

#include <omp.h>

#include "deinterleaving.h"

struct Block {
	static constexpr auto bits_per_block = 14;
	static constexpr auto BlocksPerLine = 8;

	uint8_t bits[bits_per_block];
};

template<typename T>
struct BlockAccessor {
	BlockAccessor(void* ptr, const int32_t heigth, const int32_t width)
		: ptr(static_cast<uint8_t*>(ptr)), width(width), heigth(heigth) {	}

	T *getBlock(uint32_t string, uint32_t block) const {
		assert(string < heigth && block < width);

		return reinterpret_cast<T*>(ptr + (string * width + block * sizeof(T)));
	}

	static void copy(T* dest, T* src) {
		std::memcpy(dest, src, sizeof(T));
	}

private:
	uint8_t *ptr;
	int32_t width;
	int32_t heigth;
};

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

	BlockAccessor<Block> src(input_array, heigth, Block::BlocksPerLine * Block::bits_per_block + sizeof(uint16_t) * CHAR_BIT);
	BlockAccessor<Block> dest(output_array, heigth + line_shift * (Block::BlocksPerLine - 1),
		Block::BlocksPerLine * Block::bits_per_block);


#pragma omp parallel for num_threads(N)
	for (auto column = 0; column < Block::BlocksPerLine; ++column) {
		auto dest_element = line_shift * (Block::BlocksPerLine - column - 1);
		for (auto element = 0; element < heigth; ++element, ++dest_element) {
			auto _src = src.getBlock(element, column);
			auto _dest = dest.getBlock(dest_element, column);

			BlockAccessor<Block>::copy(_dest, _src);
		}
	}
}