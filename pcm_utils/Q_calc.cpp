
#include "stdafx.h"

#include <cstdint>

#include "Q_calc.h"

#include "../eigen/Eigen/Dense"

/**
 * blocks - ��������� ������ bool 8 �����, 14 �����
 * index1, index2 - ������� ������� ������
 * restored_block1 - ��������������� ���� � ������� index1
 * restored_block1 - ��������������� ���� � ������� index2
 */
extern "C" __declspec(dllexport) void RestoreByQ(
	void* blocks /* uint8_t[8][14] */,
	const int32_t index1, const int index2,
	/* out */ void* restored_block1 /* uint8_t[14] */,
	/* out */ void* restored_block2 /* uint8_t[14] */
) {

}