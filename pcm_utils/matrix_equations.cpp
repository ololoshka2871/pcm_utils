#include "matrix_equations.h"

Sample operator*(const uint8_t M[Sample::bitsInSample][Sample::bitsInSample], const Sample& vector) {
	Sample result{};

	for (auto column = 0; column < Sample::bitsInSample; ++column) {
		auto multiplyer = vector[column];

		for (auto row = 0; row < Sample::bitsInSample; ++row) {
			result[row] ^= M[row][column] & multiplyer;
		}
	}

	return result;
}