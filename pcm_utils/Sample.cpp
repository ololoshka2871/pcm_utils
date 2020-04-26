#include "stdafx.h"
#include "Sample.h"

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