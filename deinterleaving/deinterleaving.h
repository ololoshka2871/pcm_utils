#pragma once

#include <cstdint>

/**
 * input_array - двумерный массив битов данных одного кадра
 * heigth - количество строк
 * output_array - бассив для готового результат
 * max_threads - максимальное количество потоков
 */
extern "C" __declspec(dllexport) void deinterleave(
	void* input_array /* uint8_t[112][heigth] */,
	const int32_t heigth,
	/* out */ void* output_array /* uint8_t[112][heigth] */,
	int32_t max_threads
);