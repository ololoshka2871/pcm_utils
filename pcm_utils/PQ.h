#pragma once

#include <cstdint>

#include "DLLEXPORT.h"

/**
 * blocks - двумерный массив bool 8 строк, 14 битов
 * index1, index2 - индексы побитых блоков
 * restored_block1 - восстановленный блок с номером index1
 * restored_block1 - восстановленный блок с номером index2
 */
extern "C" DLLEXPORT void RestoreByQ(
    void* blocks /* uint8_t[8][14] */,
    const int32_t index1, const int index2,
    /* out */ void* restored_block1 /* uint8_t[14] */,
    /* out */ void* restored_block2 /* uint8_t[14] */
);

/**
 * blocks - двумерный массив bool 8 строк, 14 битов
 * index1, index2 - индексы побитых блоков
 * restored_blocks - двумерный массив bool 8 строк, 14 битов
 */
extern "C" DLLEXPORT void RestoreByQ_FixedBlockOut(
    void* blocks /* uint8_t[8][14] */,
    int32_t index1, int index2,
    /* out */ void* restored_blocks /*uint8_t[8][14] */
);


/**
 * blocks - двумерный массив bool 8 строк, 14 битов
 * P - восстановленный блок с номером index1
 * Q - восстановленный блок с номером index2
 */
extern "C" DLLEXPORT void GenerateP(
    void* blocks /* uint8_t[8][14] */,
    /* out */ void* P /* uint8_t[14] */
);


/**
 * blocks - двумерный массив bool 8 строк, 14 битов
 * P - восстановленный блок с номером index1
 * Q - восстановленный блок с номером index2
 */
extern "C" DLLEXPORT void GenerateQ(
    void* blocks /* uint8_t[8][14] */,
    /* out */ void* Q /* uint8_t[14] */
);

/**
 * blocks - двумерный массив bool 8 строк, 14 битов
 * P - восстановленный блок с номером index1
 * Q - восстановленный блок с номером index2
 */
extern "C" DLLEXPORT void GeneratePQ(
    void* blocks /* uint8_t[8][14] */,
    /* out */ void* P /* uint8_t[14] */,
    /* out */ void* Q /* uint8_t[14] */
);
