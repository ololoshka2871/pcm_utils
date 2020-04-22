#pragma once

#include <cstdint>

#include "MyTwoDimArray.h"

extern "C" __declspec(dllexport) void build_line_hystogramm(void* line_data, int32_t size,
	uint32_t* histogramm_data, uint8_t *threshtolds, uint32_t histogramm_fields_count);

extern "C" __declspec(dllexport) void build_hystogramm_paralel(void* frame_data, int32_t heigth, int32_t width,
	uint32_t* histogramm_levels, uint8_t* histogramm_thresholds, uint32_t histogramm_fields_count, uint32_t paralel_threads);

extern "C" __declspec(dllexport) uint32_t hystogramm_find_maximum_index(uint32_t* histogramm_data, 
	uint32_t histogramm_fields_count, uint32_t offset);

extern "C" __declspec(dllexport) uint32_t hystogramm_find_minimum_index(uint32_t* histogramm_data,
	uint32_t histogramm_fields_count, uint32_t offset);

extern "C" __declspec(dllexport) void hystogramm_find_two_maximums(uint32_t* histogramm_data,
	uint32_t histogramm_fields_count, uint32_t* max1_index, uint32_t* max2_index);

void build_hystogramm_paralel_impl(const myTwoDimArray<uint8_t> &indata, uint32_t* histogramm_levels,
	uint8_t* histogramm_thresholds, uint32_t histogramm_fields_count, uint32_t paralel_threads);
