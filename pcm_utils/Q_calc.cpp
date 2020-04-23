
#include "stdafx.h"

#include <cstdint>

#include "Q_calc.h"

#include "../eigen/Eigen/Dense"

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

}