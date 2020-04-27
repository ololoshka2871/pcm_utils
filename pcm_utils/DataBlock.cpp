#include "stdafx.h"

#include <algorithm>
#include <cassert>

#include "DataBlock.h"

DataBlock::DataBlock(std::initializer_list<Sample> samples) {
	assert(samples.size() <= static_cast<size_t>(ElementNames::COUNT));

	auto it = samples.begin();
	for (int i = 0; it != samples.end(); ++it, ++i) {
		m[i] = *it;
	}
}

Sample DataBlock::TR2() const {
	return Sample{ {R2[13], R2[0], R2[1], R2[2], R2[3], R2[4], R2[5], R2[6],
		static_cast<uint8_t>(R2[7] ^ R2[13]),
		R2[8], R2[9], R2[10], R2[11], R2[12]} };
}

Sample DataBlock::T2L2() const {
	return Sample{ {L2[12], L2[13], L2[0], L2[1], L2[2], L2[3], L2[4], L2[5],
		static_cast<uint8_t>(L2[6] ^ L2[12]),
		static_cast<uint8_t>(L2[7] ^ L2[13]),
		L2[8], L2[9], L2[10], L2[11]} };
}

Sample DataBlock::T32R1() const {
	return Sample{ {R1[11], R1[12], R1[13], R1[0], R1[1], R1[2], R1[3], R1[4],
		static_cast<uint8_t>(R1[5] ^ R1[11]),
		static_cast<uint8_t>(R1[6] ^ R1[12]),
		static_cast<uint8_t>(R1[7] ^ R1[13]),
		R1[8], R1[9], R1[10]} };
}

Sample DataBlock::T42L1() const {
	return Sample{ {L1[10], L1[11], L1[12], L1[13], L1[0], L1[1], L1[2], L1[3],
		static_cast<uint8_t>(L1[4] ^ L1[10]),
		static_cast<uint8_t>(L1[5] ^ L1[11]),
		static_cast<uint8_t>(L1[6] ^ L1[12]),
		static_cast<uint8_t>(L1[7] ^ L1[13]),
		L1[8], L1[9] } };
}

Sample DataBlock::T52R0() const {
	return Sample{ {R0[9], R0[10], R0[11], R0[12], R0[13], R0[0], R0[1], R0[2],
		static_cast<uint8_t>(R0[3] ^ R0[9]),
		static_cast<uint8_t>(R0[4] ^ R0[10]),
		static_cast<uint8_t>(R0[5] ^ R0[11]),
		static_cast<uint8_t>(R0[6] ^ R0[12]),
		static_cast<uint8_t>(R0[7] ^ R0[13]),
		R0[8] } };
}

Sample DataBlock::T6L0() const {
	return Sample{ {L0[8], L0[9], L0[10], L0[11], L0[12], L0[13], L0[0], L0[1],
		static_cast<uint8_t>(L0[2] ^ L0[8]),
		static_cast<uint8_t>(L0[3] ^ L0[9]),
		static_cast<uint8_t>(L0[4] ^ L0[10]),
		static_cast<uint8_t>(L0[5] ^ L0[11]),
		static_cast<uint8_t>(L0[6] ^ L0[12]),
		static_cast<uint8_t>(L0[7] ^ L0[13]) } };
}

Sample DataBlock::Correct_P() const {
	return ((((L0 ^ R0) ^ L1) ^ R1) ^ L2) ^ R2;
}

Sample DataBlock::Correct_Q() const {
	return T6L0() ^ T52R0() ^ T42L1() ^ T32R1() ^ T2L2() ^ TR2();
}

Sample DataBlock::S_Pn() const {
	return Correct_P() ^ P;
}

Sample DataBlock::S_Qn() const {
	return T6L0() ^ T52R0() ^ T42L1() ^ T32R1() ^ T2L2() ^ TR2() ^ Q;
}

const Sample& DataBlock::operator[](const ElementNames& element) const {
	return m[static_cast<size_t>(element)];
}

Sample& DataBlock::operator[](const ElementNames& element) {
	return m[static_cast<size_t>(element)];
}

bool DataBlock::operator==(const DataBlock& lr) {
	for (auto sample = 0; sample < static_cast<size_t>(ElementNames::COUNT); ++sample) {
		if (m[sample] != lr.m[sample]) {
			return false;
		}
	}
	return true;
}

DataBlock DataBlock::Reversed() const {
	DataBlock res;

	for (auto sample = 0; sample < static_cast<size_t>(ElementNames::COUNT); ++sample) {
		res[static_cast<ElementNames>(sample)] = m[sample].Reversed();
	}

	return res;
}

void* DataBlock::ptr()  {
	return m[0].ptr();
}

DataBlock& DataBlock::operator=(const DataBlock& lr) {
	std::memcpy(m, lr.m, sizeof(Sample) * static_cast<size_t>(ElementNames::COUNT));
	return *this;
}