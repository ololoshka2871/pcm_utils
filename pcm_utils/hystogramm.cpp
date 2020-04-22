
#include "stdafx.h"

#include <vector>
#include <list>
#include <algorithm>

#include "OwnIterator.h"

#include "hystogramm.h"

extern "C" __declspec(dllexport) void build_line_hystogramm(void* line_data, int32_t size,
	uint32_t* histogramm_data, uint8_t *threshtolds, uint32_t histogramm_fields_count) {
	auto end = &threshtolds[histogramm_fields_count];
	for (auto i = 0; i < size; ++i) {
		auto pixel_value = static_cast<uint8_t*>(line_data)[i];

		auto it = std::find_if(threshtolds, end, [pixel_value](uint8_t test_v) {
			return pixel_value <= test_v;
		});

		++histogramm_data[std::distance(threshtolds, it)];
	}
}

extern "C" __declspec(dllexport) void build_hystogramm_paralel(void* frame_data, int32_t heigth, int32_t width,
	uint32_t* histogramm_levels, uint8_t* histogramm_thresholds, uint32_t histogramm_fields_count, uint32_t paralel_threads) {

	myTwoDimArray<uint8_t> indata(frame_data, heigth, width);

	build_hystogramm_paralel_impl(indata, histogramm_levels, histogramm_thresholds, histogramm_fields_count, paralel_threads);
}

extern "C" __declspec(dllexport) uint32_t hystogramm_find_maximum_index(uint32_t* histogramm_data, 
	uint32_t histogramm_fields_count, uint32_t offset) {
	auto start = &histogramm_data[offset];
	const auto end = &histogramm_data[histogramm_fields_count];
	
	return std::distance(histogramm_data, std::max_element(start, end));
}

extern "C" __declspec(dllexport) uint32_t hystogramm_find_minimum_index(uint32_t* histogramm_data,
	uint32_t histogramm_fields_count, uint32_t offset) {
	auto start = &histogramm_data[offset];
	const auto end = &histogramm_data[histogramm_fields_count];

	return std::distance(histogramm_data, std::min_element(start, end));
}

extern "C" __declspec(dllexport) void hystogramm_find_two_maximums(uint32_t* histogramm_data,
	uint32_t histogramm_fields_count, uint32_t* max1_index, uint32_t* max2_index) {

	auto diraive_at_point = [histogramm_data](uint32_t point) -> int64_t {
		return 
			static_cast<int64_t>(histogramm_data[point + 1]) - 
			static_cast<int64_t>(histogramm_data[point]);
	};

	std::list<std::pair<uint32_t, uint32_t>> max_indexes;

	for (auto i = 1; i < histogramm_fields_count - 1; ++i) {
		if (diraive_at_point(i) < 0 && diraive_at_point(i - 1) > 0) {
			// maximum 
			max_indexes.emplace_back(i, histogramm_data[i]);
		}
	}

	{
		std::pair<uint32_t, uint32_t> start_v{ 0, histogramm_data[0] };
		if (std::find(max_indexes.cbegin(), max_indexes.cend(), start_v) == max_indexes.cend()) {
			max_indexes.push_back(start_v);
		}

		std::pair<uint32_t, uint32_t> end_v{ histogramm_fields_count - 1, histogramm_data[histogramm_fields_count - 1] };
		if (std::find(max_indexes.cbegin(), max_indexes.cend(), end_v) == max_indexes.cend()) {
			max_indexes.push_back(end_v);
		}
	}

	max_indexes.sort([](const std::pair<uint32_t, uint32_t>& a, std::pair<uint32_t, uint32_t>& b) {
		return a.second > b.second;
	});


	auto it = max_indexes.cbegin();
	*max1_index = it->first;
	++it;
	*max2_index = it->first;

	if (*max1_index > *max2_index) {
		std::swap(*max1_index, *max2_index);
	}
}

void build_hystogramm_paralel_impl(const myTwoDimArray<uint8_t> &indata, uint32_t* histogramm_levels,
	uint8_t* histogramm_thresholds, uint32_t histogramm_fields_count, uint32_t paralel_threads) {

	std::vector<uint32_t> hysto_line_data(histogramm_fields_count * indata.getheigth());
	myTwoDimArray<uint32_t> hysto_array(hysto_line_data.data(), histogramm_fields_count, indata.getheigth());

	const auto step = 256 / histogramm_fields_count;

	for (auto i = 0; i < histogramm_fields_count; ++i) {
		histogramm_thresholds[i] = step * (i + 1);
	}
	
#pragma omp parallel for num_threads(paralel_threads)
	for (auto line = 0; line < indata.getheigth(); ++line) {
		build_line_hystogramm(indata.getLine(line), indata.getwidth(), hysto_array.getLine(line), histogramm_thresholds,
			histogramm_fields_count);
	}

#pragma omp parallel for num_threads(paralel_threads)
	for (auto column = 0; column < histogramm_fields_count; ++column) {
		auto * const hyst_column = &histogramm_levels[column];
		*hyst_column = 0;

		for (auto line = 0; line < indata.getheigth(); ++line) {
			*hyst_column += hysto_array.element(column, line);
		}
	}
}

