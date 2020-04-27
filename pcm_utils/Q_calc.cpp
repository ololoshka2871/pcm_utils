#include "stdafx.h"

#include <cstdint>
#include <utility>
#include <cassert>

#include "Q_calc.h"

#include "matrix_constants.h"
#include "matrix_equations.h"

#include "DataBlock.h"


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

static auto __recalc_P_marker(const Sample &A, const Sample& B) {
	return Sample{};
};

static auto emptySample(const Sample &A, const Sample& B) {
	return Sample{};
};

static const E_Calculator calculator_table[6][6]{
	{ // L0
		{ // L0 Ц R0 (1)
			[](const Sample &S_Pn, const Sample& S_Qn) -> Sample {
				// TpIm1 * (S_Pn ^ Tm5 * S_Qn)
				return TpIm1 * (S_Pn ^ Tm5 * S_Qn);
			},
			xor_Sample
		},
		{ // L0 Ц L1 (7)
			[](const Sample &S_Pn, const Sample& S_Qn) -> Sample {
				// T2pm1 * (S_Pn ^ Tm4 * S_Qn)
				return T2pm1 * (S_Pn ^ Tm4 * S_Qn);
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
				// Tm6 * S_Qn
				return Tm6 * S_Qn;
			},
			__recalc_P_marker
		},
	},
	{ // R0
		{ // no element
			emptySample,
			emptySample
		},
		{ // R0 Ц L1 (2)
			[](const Sample &S_Pn, const Sample& S_Qn) -> Sample {
				// TpIm1 * (S_Pn ^ Tm4 * S_Qn)
				return TpIm1 * (S_Pn ^ Tm4 * S_Qn);
			},
			xor_Sample
		},
		{ // R0 Ц R1 (8)
			[](const Sample &S_Pn, const Sample& S_Qn) -> Sample {
				// T2pm1 * (S_Pn ^ Tm3 * S_Qn)
				return T2pm1 * (S_Pn ^ Tm3 * S_Qn);
			},
			xor_Sample
		},
		{ // R0 Ц L2 (13)
			[](const Sample &S_Pn, const Sample& S_Qn) -> Sample {
				// T3pm1 * (S_Pn ^ Tm2 * S_Qn)
				return T3pm1 * (S_Pn ^ Tm2 * S_Qn);
			},
			xor_Sample
		},
		{ // R0 Ц R2 (17)
			[](const Sample &S_Pn, const Sample& S_Qn) -> Sample {
				// T4pm1 * (S_Pn ^ Tm1 * S_Qn)
				return T4pm1 * (S_Pn ^ Tm1 * S_Qn);
			},
			xor_Sample
		},
		{ // R0 Ц P (20)
			[](const Sample &S_Pn, const Sample& S_Qn) -> Sample {
				// Tm5 * S_Qn
				return Tm5 * S_Qn;
			},
			__recalc_P_marker
		}
	},
	{ // L1
		{
			emptySample,
			emptySample
		},
		{
			emptySample,
			emptySample
		},
		{ // L1 Ц R1 (3)
			[](const Sample &S_Pn, const Sample& S_Qn) -> Sample {
				// TpIm1 * (S_Pn ^ Tm3 * S_Qn)
				return TpIm1 * (S_Pn ^ Tm3 * S_Qn);
			},
			xor_Sample
		},
		{ // L1 Ц L2 (9)
			[](const Sample &S_Pn, const Sample& S_Qn) -> Sample {
				// T2pm1 * (S_Pn ^ Tm2 * S_Qn)
				return T2pm1 * (S_Pn ^ Tm2 * S_Qn);
			},
			xor_Sample
		},
		{ // L1 Ц R2 (14)
			[](const Sample &S_Pn, const Sample& S_Qn) -> Sample {
				// T3pm1 * (S_Pn ^ Tm1 * S_Qn)
				return T3pm1 * (S_Pn ^ Tm1 * S_Qn);
			},
			xor_Sample
		},
		{ // L1 Ц P (18)
			[](const Sample &S_Pn, const Sample& S_Qn) -> Sample {
				// Tm4 * S_Qn
				return Tm4 * S_Qn;
			},
			__recalc_P_marker
		}
	},
	{ // R1
		{
			emptySample,
			emptySample
		},
		{
			emptySample,
			emptySample
		},
		{
			emptySample,
			emptySample
		},
		{ // R1 Ц L2 (4)
			[](const Sample &S_Pn, const Sample& S_Qn) -> Sample {
				// TpIm1 * (S_Pn ^ Tm2 * S_Qn)
				return TpIm1 * (S_Pn ^ Tm2 * S_Qn);
			},
			xor_Sample
		},
		{ // R1 Ц R2 (10)
			[](const Sample &S_Pn, const Sample& S_Qn) -> Sample {
				// T2pm1 * (S_Pn ^ Tm1 * S_Qn)
				return T2pm1 * (S_Pn ^ Tm1 * S_Qn);
			},
			xor_Sample
		},
		{ // R1 Ц P (15)
			[](const Sample &S_Pn, const Sample& S_Qn) -> Sample {
				// Tm3 * S_Qn
				return Tm3 * S_Qn;
			},
			__recalc_P_marker
		}
	},
	{ // L2
		{
			emptySample,
			emptySample
		},
		{
			emptySample,
			emptySample
		},
		{
			emptySample,
			emptySample
		},
		{
			emptySample,
			emptySample
		},
		{ // L2 Ц R2 (5)
			[](const Sample &S_Pn, const Sample& S_Qn) -> Sample {
				// TpIm1 * (S_Pn ^ Tm1 * S_Qn)
				return TpIm1 * (S_Pn ^ Tm1 * S_Qn);
			},
			xor_Sample
		},
		{ // L2 Ц P (11)
			[](const Sample &S_Pn, const Sample& S_Qn) -> Sample {
				// Tm2 * S_Qn
				return Tm2 * S_Qn;
			},
			__recalc_P_marker
		}
	},
	{ // R2
		{
			emptySample,
			emptySample
		},
		{
			emptySample,
			emptySample
		},
		{
			emptySample,
			emptySample
		},
		{
			emptySample,
			emptySample
		},
		{
			emptySample,
			emptySample
		},
		{ // R2 Ц P (6)
			[](const Sample &S_Pn, const Sample& S_Qn) -> Sample {
				// Tm1 * S_Qn
				return Tm1 * S_Qn;
			},
			__recalc_P_marker
		},
	},
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
	int32_t index1, int index2,
	/* out */ void* restored_block1 /* uint8_t[14] */,
	/* out */ void* restored_block2 /* uint8_t[14] */
) {
	assert((index1 < 6) && (index2 < 7));

	if (index2 < index1) {
		std::swap(index1, index2);
		std::swap(restored_block1, restored_block2);
	}

	auto &calc = calculator_table[index1][index2 - 1];

	DataBlock *line = static_cast<DataBlock *>(blocks);

	auto S_Pn = line->S_Pn();
	auto S_Qn = line->S_Qn();

	auto _restored_block1 = static_cast<Sample *>(restored_block1);
	auto _restored_block2 = static_cast<Sample *>(restored_block2);

	if (S_Pn.isZero() && S_Qn.isZero()) {
		// all ok!
		*_restored_block1 = line->operator[](DataBlock::ElementNames(index1));
		*_restored_block2 = line->operator[](DataBlock::ElementNames(index2));
		return;
	}

	auto ER1 = calc.ER1(S_Pn, S_Qn);
	*_restored_block1 = line->operator[](DataBlock::ElementNames(index1)) ^ ER1;

	if (calc.ER2 != __recalc_P_marker) {
		*_restored_block2 = line->operator[](DataBlock::ElementNames(index2)) ^ calc.ER2(ER1, S_Pn);
	}
	else {
		DataBlock restored_data(*line);
		restored_data[static_cast<DataBlock::ElementNames>(index1)] = *_restored_block1;
		*_restored_block2 = restored_data.Correct_P();
	}
}

/**
 * blocks - двумерный массив bool 8 строк, 14 битов
 * P - восстановленный блок с номером index1
 * Q - восстановленный блок с номером index2
 */
extern "C" __declspec(dllexport) void GenerateP(
	void* blocks /* uint8_t[8][14] */,
	/* out */ void* P /* uint8_t[14] */
) {
	DataBlock *line = static_cast<DataBlock *>(blocks);

	auto _P = static_cast<Sample *>(P);
	*_P = line->Correct_P();
}


/**
 * blocks - двумерный массив bool 8 строк, 14 битов
 * P - восстановленный блок с номером index1
 * Q - восстановленный блок с номером index2
 */
extern "C" __declspec(dllexport) void GenerateQ(
	void* blocks /* uint8_t[8][14] */,
	/* out */ void* Q /* uint8_t[14] */
) {
	DataBlock *line = static_cast<DataBlock *>(blocks);

	auto _Q = static_cast<Sample *>(Q);
	*_Q = line->Correct_Q();
}


/**
 * blocks - двумерный массив bool 8 строк, 14 битов
 * P - восстановленный блок с номером index1
 * Q - восстановленный блок с номером index2
 */
extern "C" __declspec(dllexport) void GeneratePQ(
	void* blocks /* uint8_t[8][14] */,
	/* out */ void* P /* uint8_t[14] */,
	/* out */ void* Q /* uint8_t[14] */
) {
	DataBlock *line = static_cast<DataBlock *>(blocks);

	auto _P = static_cast<Sample *>(P);
	auto _Q = static_cast<Sample *>(Q);

	*_P = line->Correct_P();
	*_Q = line->Correct_Q();
}