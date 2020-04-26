#include "stdafx.h"

#include <cstdint>

#include "Q_calc.h"

#include "matrix_constants.h"
#include "matrix_equations.h"

#include "Sample.h"

union DataBlock {
	enum class ElementNames {
		L0 = 0,
		R0 = 1,
		L1 = 2,
		R1 = 3,
		L2 = 4,
		R2 = 5,
		P = 6,
		Q = 7,

		COUNT = 8
	};

	Sample L0, R0, L1, R1, L2, R2, P, Q;
	Sample m[static_cast<size_t>(ElementNames::COUNT)];

	Sample TR2() const {
		return Sample{ {R2[13], R2[0], R2[1], R2[2], R2[3], R2[4], R2[5], R2[6],
			static_cast<uint8_t>(R2[7] ^ R2[13]),
			R2[8], R2[9], R2[10], R2[11], R2[12]} };
	}

	Sample T2L2() const {
		return Sample{ {L2[12], L2[13], L2[0], L2[1], L2[2], L2[3], L2[4], L2[5],
			static_cast<uint8_t>(L2[6] ^ L2[12]),
			static_cast<uint8_t>(L2[7] ^ L2[13]),
			L2[8], L2[9], L2[10], L2[11]} };
	}

	Sample T32R1() const {
		return Sample{ {R1[11], R1[12], R1[13], R1[0], R1[1], R1[2], R1[3], R1[4],
			static_cast<uint8_t>(R1[5] ^ R1[11]),
			static_cast<uint8_t>(R1[6] ^ R1[12]),
			static_cast<uint8_t>(R1[7] ^ R1[13]),
			R1[8], R1[9], R1[10]} };
	}

	Sample T42L1() const {
		return Sample{ {L1[10], L1[11], L1[12], L1[13], L1[0], L1[1], L1[2], L1[3],
			static_cast<uint8_t>(L1[4] ^ L1[10]),
			static_cast<uint8_t>(L1[5] ^ L1[11]),
			static_cast<uint8_t>(L1[6] ^ L1[12]),
			static_cast<uint8_t>(L1[7] ^ L1[13]),
			L1[8], L1[9] } };
	}

	Sample T52R0() const {
		return Sample{ {R0[9], R0[10], R0[11], R0[12], R0[13], R0[0], R0[1], R0[2],
			static_cast<uint8_t>(R0[3] ^ R0[9]),
			static_cast<uint8_t>(R0[4] ^ R0[10]),
			static_cast<uint8_t>(R0[5] ^ R0[11]),
			static_cast<uint8_t>(R0[6] ^ R0[12]),
			static_cast<uint8_t>(R0[7] ^ R0[13]),
			R0[8] } };
	}

	Sample T6L0() const {
		return Sample{ {L0[8], L0[9], L0[10], L0[11], L0[12], L0[13], L0[0], L0[1],
			static_cast<uint8_t>(L0[2] ^ L0[8]),
			static_cast<uint8_t>(L0[3] ^ L0[9]),
			static_cast<uint8_t>(L0[4] ^ L0[10]),
			static_cast<uint8_t>(L0[5] ^ L0[11]),
			static_cast<uint8_t>(L0[6] ^ L0[12]),
			static_cast<uint8_t>(L0[7] ^ L0[13]) } };
	}

	Sample S_Pn() const {
		return T6L0() ^ T52R0() ^ T42L1() ^ T32R1() ^ T2L2() ^ TR2() ^ P;
	}

	Sample S_Qn() const {
		return T6L0() ^ T52R0() ^ T42L1() ^ T32R1() ^ T2L2() ^ TR2() ^ Q;
	}

	const Sample& operator[](const ElementNames& element) const {
		return m[static_cast<size_t>(element)];
	}
};

struct E_Calculator {
	using ER1_calc_t = Sample(*)(const Sample &S_Pn, const Sample& S_Qn);
	using ER2_calc_t = Sample(*)(const Sample &ER1, const Sample& S_Pn);

	ER1_calc_t ER1;
	ER2_calc_t ER2;
};

/****************************************************************************************/

static auto xor_Sample(const Sample &A, const Sample& B) {
	return A ^ B;
};

static auto emptySample(const Sample &A, const Sample& B) {
	return Sample{};
};

static const E_Calculator calculator_table[1][5]{
	{ // L0
		{ // L0 Ц R0 (1)
			[](const Sample &S_Pn, const Sample& S_Qn) -> Sample {
				// TpIm1 * (S_Pn ^ Tm5 * S_Qn)
				return TpIm1 * (S_Pn ^ Tm5 * S_Qn);
			},
			xor_Sample
		},
		{ // L0 Ц R1 (12)
			[](const Sample &S_Pn, const Sample& S_Qn) -> Sample {
				// T3pm1 * (S_Pn ^ Tm3 * S_Qn)
				return T3pm1 * (S_Pn ^ Tm3 * S_Qn);
			},
			xor_Sample
		},
		{ // L0 Ц L2 (16)
			[](const Sample &S_Pn, const Sample& S_Qn) -> Sample {
				// T4pm1 * (S_Pn ^ Tm2 * S_Qn)
				return T4pm1 * (S_Pn ^ Tm2 * S_Qn);
			},
			xor_Sample
		},
		{ // L0 Ц R2 (19)
			[](const Sample &S_Pn, const Sample& S_Qn) -> Sample {
				// T5pm1 * (S_Pn ^ Tm1 * S_Qn) 
				return T5pm1 * (S_Pn ^ Tm1 * S_Qn);
			},
			xor_Sample
		},
		{ // L0 Ц P (21)
			[](const Sample &S_Pn, const Sample& S_Qn) -> Sample {
				// Tm6 * S_Pn
				return Tm6 * S_Pn;
			},
			emptySample
		},
	}
};

/****************************************************************************************/

/**
 * blocks - двумерный массив bool 8 строк, 14 битов
 * index1, index2 - индексы побитых блоков
 * restored_block1 - восстановленный блок с номером index1
 * restored_block1 - восстановленный блок с номером index2
 */
extern "C" __declspec(dllexport) void RestoreByQ(
	void* blocks /* uint8_t[8][14] */,
	const int32_t index1, const int index2,
	/* out */ void* restored_block1 /* uint8_t[14] */,
	/* out */ void* restored_block2 /* uint8_t[14] */
) {
	auto &calc = calculator_table[index1][index2];

	DataBlock *line = static_cast<DataBlock *>(blocks);

	auto S_Pn = line->S_Pn();
	auto S_Qn = line->S_Qn();

	auto _restored_block1 = static_cast<Sample *>(restored_block1);
	auto _restored_block2 = static_cast<Sample *>(restored_block2);

	auto ER1 = calc.ER1(S_Pn, S_Qn);

	*_restored_block1 = line->operator[](DataBlock::ElementNames(index1)) ^ ER1;
	*_restored_block2 = line->operator[](DataBlock::ElementNames(index2)) ^ calc.ER1(ER1, S_Pn);
}