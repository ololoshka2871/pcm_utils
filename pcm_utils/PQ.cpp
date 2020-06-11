#include <cstdint>
#include <utility>
#include <cassert>

#include "PQ.h"

#include "matrix_constants.h"
#include "matrix_equations.h"

#include "DataBlock.h"

/****************************************************************************************/

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
		{ // L0 � R0 (1)
			[](const Sample &S_Pn, const Sample& S_Qn) -> Sample {
				// TpIm1 * (S_Pn ^ Tm5 * S_Qn)
				return TpIm1 * (S_Pn ^ Tm5 * S_Qn);
			},
			xor_Sample
		},
		{ // L0 � L1 (7)
			[](const Sample &S_Pn, const Sample& S_Qn) -> Sample {
				// T2pm1 * (S_Pn ^ Tm4 * S_Qn)
				return T2pm1 * (S_Pn ^ Tm4 * S_Qn);
			},
			xor_Sample
		},
		{ // L0 � R1 (12)
			[](const Sample &S_Pn, const Sample& S_Qn) -> Sample {
				// T3pm1 * (S_Pn ^ Tm3 * S_Qn)
				return T3pm1 * (S_Pn ^ Tm3 * S_Qn);
			},
			xor_Sample
		},
		{ // L0 � L2 (16)
			[](const Sample &S_Pn, const Sample& S_Qn) -> Sample {
				// T4pm1 * (S_Pn ^ Tm2 * S_Qn)
				return T4pm1 * (S_Pn ^ Tm2 * S_Qn);
			},
			xor_Sample
		},
		{ // L0 � R2 (19)
			[](const Sample &S_Pn, const Sample& S_Qn) -> Sample {
				// T5pm1 * (S_Pn ^ Tm1 * S_Qn) 
				return T5pm1 * (S_Pn ^ Tm1 * S_Qn);
			},
			xor_Sample
		},
		{ // L0 � P (21)
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
		{ // R0 � L1 (2)
			[](const Sample &S_Pn, const Sample& S_Qn) -> Sample {
				// TpIm1 * (S_Pn ^ Tm4 * S_Qn)
				return TpIm1 * (S_Pn ^ Tm4 * S_Qn);
			},
			xor_Sample
		},
		{ // R0 � R1 (8)
			[](const Sample &S_Pn, const Sample& S_Qn) -> Sample {
				// T2pm1 * (S_Pn ^ Tm3 * S_Qn)
				return T2pm1 * (S_Pn ^ Tm3 * S_Qn);
			},
			xor_Sample
		},
		{ // R0 � L2 (13)
			[](const Sample &S_Pn, const Sample& S_Qn) -> Sample {
				// T3pm1 * (S_Pn ^ Tm2 * S_Qn)
				return T3pm1 * (S_Pn ^ Tm2 * S_Qn);
			},
			xor_Sample
		},
		{ // R0 � R2 (17)
			[](const Sample &S_Pn, const Sample& S_Qn) -> Sample {
				// T4pm1 * (S_Pn ^ Tm1 * S_Qn)
				return T4pm1 * (S_Pn ^ Tm1 * S_Qn);
			},
			xor_Sample
		},
		{ // R0 � P (20)
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
		{ // L1 � R1 (3)
			[](const Sample &S_Pn, const Sample& S_Qn) -> Sample {
				// TpIm1 * (S_Pn ^ Tm3 * S_Qn)
				return TpIm1 * (S_Pn ^ Tm3 * S_Qn);
			},
			xor_Sample
		},
		{ // L1 � L2 (9)
			[](const Sample &S_Pn, const Sample& S_Qn) -> Sample {
				// T2pm1 * (S_Pn ^ Tm2 * S_Qn)
				return T2pm1 * (S_Pn ^ Tm2 * S_Qn);
			},
			xor_Sample
		},
		{ // L1 � R2 (14)
			[](const Sample &S_Pn, const Sample& S_Qn) -> Sample {
				// T3pm1 * (S_Pn ^ Tm1 * S_Qn)
				return T3pm1 * (S_Pn ^ Tm1 * S_Qn);
			},
			xor_Sample
		},
		{ // L1 � P (18)
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
		{ // R1 � L2 (4)
			[](const Sample &S_Pn, const Sample& S_Qn) -> Sample {
				// TpIm1 * (S_Pn ^ Tm2 * S_Qn)
				return TpIm1 * (S_Pn ^ Tm2 * S_Qn);
			},
			xor_Sample
		},
		{ // R1 � R2 (10)
			[](const Sample &S_Pn, const Sample& S_Qn) -> Sample {
				// T2pm1 * (S_Pn ^ Tm1 * S_Qn)
				return T2pm1 * (S_Pn ^ Tm1 * S_Qn);
			},
			xor_Sample
		},
		{ // R1 � P (15)
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
		{ // L2 � R2 (5)
			[](const Sample &S_Pn, const Sample& S_Qn) -> Sample {
				// TpIm1 * (S_Pn ^ Tm1 * S_Qn)
				return TpIm1 * (S_Pn ^ Tm1 * S_Qn);
			},
			xor_Sample
		},
		{ // L2 � P (11)
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
		{ // R2 � P (6)
			[](const Sample &S_Pn, const Sample& S_Qn) -> Sample {
				// Tm1 * S_Qn
				return Tm1 * S_Qn;
			},
			__recalc_P_marker
		},
	},
};

/****************************************************************************************/

static Sample restore_by_P(DataBlock &line, DataBlock::ElementNames err_index) {
	Sample result{};
	for (size_t sample = 0; sample < static_cast<size_t>(DataBlock::ElementNames::P); ++sample) {
		const auto _sample = DataBlock::ElementNames(sample);
		if (_sample == err_index) {
			continue;
		}
		result = result ^ line[_sample];
	}
	return result ^ line[DataBlock::ElementNames::P];
}

static Sample restore_single(DataBlock &line, DataBlock::ElementNames err_index) {
	switch (err_index)
	{
		case DataBlock::ElementNames::P:
			return line.Correct_P();
		case DataBlock::ElementNames::Q:
			return line.Correct_Q();
		default:
			return restore_by_P(line, err_index);
	}
}

/****************************************************************************************/


extern "C" DLLEXPORT void RestoreByQ(
	void* blocks /* uint8_t[8][14] */,
	int32_t index1, int index2,
	/* out */ void* restored_block1 /* uint8_t[14] */,
	/* out */ void* restored_block2 /* uint8_t[14] */
) {
	auto _restored_block1 = static_cast<Sample *>(restored_block1);
	auto _restored_block2 = static_cast<Sample *>(restored_block2);

	DataBlock *line = static_cast<DataBlock *>(blocks);

	if (index1 == index2) {			 // ����� �����-��!
		*_restored_block2 = *_restored_block1 = restore_single(*line, DataBlock::ElementNames(index1));
		return;
	}

	if (index2 < index1) {
		std::swap(index1, index2);
		std::swap(restored_block1, restored_block2);
	}

	if ((index1 >= 7) || (index2 >= 8)) {
		return; // INVALID request
	}

	if (index2 == 7) {
		const auto _index1 = static_cast<DataBlock::ElementNames>(index1);

		Sample restored_block1 = (index1 == 6)
			? line->Correct_P() // restore P and Q, data ok	
			: restore_by_P(*line, _index1);// �������������� ����� index1 �� parity

		DataBlock restored_data(*line);
		restored_data[_index1] = restored_block1;

		*_restored_block1 = restored_block1;
		*_restored_block2 = restored_data.Correct_Q();
		return;
	}

	auto &calc = calculator_table[index1][index2 - 1];

	auto S_Pn = line->S_Pn();
	auto S_Qn = line->S_Qn();

	if (S_Pn.isZero() && S_Qn.isZero()) {// all ok!
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

// ������� ������������
extern "C" DLLEXPORT void RestoreByQ_FixedBlockOut(
	void* blocks /* uint8_t[8][14] */,
	int32_t index1, int index2,
	/* out */ void* restored_blocks /*uint8_t[8][14] */
) {
	DataBlock reversed_result = static_cast<DataBlock *>(blocks)->Reversed();
	DataBlock *result = static_cast<DataBlock *>(restored_blocks);

	if (index1 == index2) { // ����� �����-��!
		reversed_result[DataBlock::ElementNames(index1)] =
			restore_single(reversed_result, DataBlock::ElementNames(index1));
		
		*result = reversed_result.Reversed();
		return;
	}

	if (index2 < index1) {
		std::swap(index1, index2);
	}

	auto &_restored_block1 = reversed_result[DataBlock::ElementNames(index1)];
	auto &_restored_block2 = reversed_result[DataBlock::ElementNames(index2)];
	
	if ((index1 >= 7) || (index2 >= 8)) {
		*result = reversed_result.Reversed();
		return; // INVALID request
	}

	if (index2 == 7) {
		const auto _index1 = static_cast<DataBlock::ElementNames>(index1);

		_restored_block1 = (index1 == 6)
			? reversed_result.Correct_P() // restore P and Q, data ok	
			: restore_by_P(reversed_result, _index1);// �������������� ����� index1 �� parity

		_restored_block2 = reversed_result.Correct_Q();
		
		*result = reversed_result.Reversed();
		return;
	}

	auto &calc = calculator_table[index1][index2 - 1];

	auto S_Pn = reversed_result.S_Pn();
	auto S_Qn = reversed_result.S_Qn();

	if (S_Pn.isZero() && S_Qn.isZero()) {// all ok!
		*result = reversed_result.Reversed();
		return;
	}

	auto ER1 = calc.ER1(S_Pn, S_Qn);
	_restored_block1 = reversed_result[DataBlock::ElementNames(index1)] ^ ER1;

	_restored_block2 = calc.ER2 != __recalc_P_marker 
		? reversed_result[DataBlock::ElementNames(index2)] ^ calc.ER2(ER1, S_Pn)
		: reversed_result.Correct_P();

	*result = reversed_result.Reversed();
}


extern "C" DLLEXPORT void GenerateP(
	void* blocks /* uint8_t[8][14] */,
	/* out */ void* P /* uint8_t[14] */
) {
	DataBlock *line = static_cast<DataBlock *>(blocks);

	auto _P = static_cast<Sample *>(P);
	*_P = line->Correct_P();
}


extern "C" DLLEXPORT void GenerateQ(
	void* blocks /* uint8_t[8][14] */,
	/* out */ void* Q /* uint8_t[14] */
) {
	auto line = static_cast<DataBlock *>(blocks)->Reversed();

	auto _Q = static_cast<Sample *>(Q);
	*_Q = line.Correct_Q().Reversed();
}

extern "C" DLLEXPORT void GeneratePQ(
	void* blocks /* uint8_t[8][14] */,
	/* out */ void* P /* uint8_t[14] */,
	/* out */ void* Q /* uint8_t[14] */
) {
	auto orig_line = static_cast<DataBlock *>(blocks);
	auto reversed_line = orig_line->Reversed();

	auto _P = static_cast<Sample *>(P);
	auto _Q = static_cast<Sample *>(Q);

	*_P = orig_line->Correct_P();
	*_Q = reversed_line.Correct_Q().Reversed();
}
