#include "stdafx.h"

#include <cstring>
#include <cassert>

#include "Sample.h"

Sample::Sample(std::initializer_list<uint8_t> init) {
	assert(init.size() == bitsInSample);

	auto it = init.begin();
	for (auto i = 0; it != init.end(); ++it, ++i) {
		values[i] = *it;
	}
}

Sample Sample::operator^(const Sample& lr) const {
	Sample result;

	for (auto i = 0; i < bitsInSample; ++i) {
		result.values[i] = static_cast<uint8_t>(values[i] ^ lr.values[i]);
	}

	return result;
}

uint8_t& Sample::operator[](const size_t element) {
	return values[element];
}

uint8_t Sample::operator[](const size_t element) const {
	return values[element];
}

bool Sample::operator==(const Sample& lr) {
	return std::memcmp(values, lr.values, bitsInSample) == 0;
}

bool Sample::operator!=(const Sample& lr) {
	return !(*this == lr);
}

Sample& Sample::operator=(const Sample& lr) {
	std::memcpy(values, lr.values, bitsInSample);
	return *this;
}

Sample Sample::Reversed() const {
	Sample res;

	for (auto i = 0; i < bitsInSample; ++i) {
		res[i] = values[bitsInSample - 1 - i];
	}

	return res;
}

void* Sample::ptr() {
	return values;
}

bool Sample::isZero() const {
	uint8_t r = 0;
	for (auto i = 0; i < bitsInSample; ++i) {
		r |= values[i];
	}
	return !r;
}