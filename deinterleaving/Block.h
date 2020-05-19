#pragma once

#include <cstdint>

struct Block2 {
	static constexpr auto bits_per_block = 2;
	static constexpr auto BlocksPerLine = 112 / bits_per_block;

	Block2() : bits{} {}

	Block2& operator^=(const Block2 &lr) {
		bits[0] ^= lr.bits[0];
		bits[1] ^= lr.bits[1];
		return *this;
	}

	uint32_t summ() const {
		return bits[0] + bits[1];
	}

	uint8_t bits[bits_per_block];
};

struct Block14 {
	static constexpr auto bits_per_block = 14;
	static constexpr auto BlocksPerLine = 8;

	uint8_t bits[bits_per_block];
};

struct Block16 {
	static constexpr auto bits_per_block = 16;
	static constexpr auto BlocksPerLine = 7;

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

	static void* copy(void* dest, void* src) {
		std::memcpy(dest, src, sizeof(T));

		return static_cast<uint8_t*>(dest) + sizeof(T);
	}

private:
	uint8_t *ptr;
	int32_t width;
	int32_t heigth;
};
