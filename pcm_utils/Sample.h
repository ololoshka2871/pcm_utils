#pragma once

#include <cstdint>

struct Sample {
	static constexpr auto bitsInSample = 14;

	Sample operator^(const Sample& lr) const;
	uint8_t& operator[](const size_t element);
	uint8_t operator[](const size_t element) const;

	uint8_t values[bitsInSample];
};

