#include "stdafx.h"

#include <vector>
#include <algorithm>

#include <omp.h>

#include "MyTwoDimArray.h"
#include "Comparator.h"
#include "crc16_ccitt.h"
#include "ComparatorPlusCRC.h"
#include "hystogramm.h"
#include "OwnIterator.h"
#include "find_it_if.h"

#include "frame_process.h"

static constexpr int BYTES_PER_PCM_STRING = 14;
static constexpr int CRC_BYTES_PER_PCM_STRING = 2;
static constexpr int DATA_BYTES_PER_PCM_STRING = BYTES_PER_PCM_STRING + CRC_BYTES_PER_PCM_STRING;
static constexpr int DATA_BYTES_PER_PCM_STRING_BITS = DATA_BYTES_PER_PCM_STRING * CHAR_BIT;

TresholdProvider::TresholdProvider()
	: base(0), max_offest(0), offest(0) {}

TresholdProvider::TresholdProvider(const uint8_t threshold, const uint8_t threshold_range)
	: base(threshold), max_offest(threshold_range * 2), offest(0) {}

int32_t TresholdProvider::value() const {
	return offest & 1
		? base + offest / 2 + 1
		: base - offest / 2;
}

bool TresholdProvider::next() {
	++offest;
	return !end();
}

bool TresholdProvider::end() const {
	return offest > max_offest;
}

uint32_t TresholdProvider::step() const {
	return offest;
}

bool TresholdProvider::plusHalf() {
	offest += std::max<uint32_t>(1, max_offest / 4);
	return !end();
}

uint32_t TresholdProvider::maximum() const {
	return max_offest;
}

struct LineInfo {
	enum Resut {
		NoData,
		Error,
		Success,
	};

	LineInfo() = default;

	int32_t offset_start;
	int32_t offset_end;
	double pixel_per_bit;

	int32_t treshold;
	uint32_t line;
	uint32_t trys;
	Resut success;
};

class SingleLinePlocessor {
public:
	SingleLinePlocessor(uint8_t *pixel_data, const int32_t width, uint8_t out_line[BYTES_PER_PCM_STRING * CHAR_BIT],
		uint8_t *binarization_buff, TresholdProvider threshold, LineInfo *lineinfo)
		: pixel_data(pixel_data), width(width), out_line(out_line), binarization_buff(binarization_buff),
		threshold(threshold), lineinfo(lineinfo) {}

	void execute() {
		uint8_t bytes[BYTES_PER_PCM_STRING];

		TresholdProvider initialThreshold(threshold);

		lineinfo->treshold = threshold.value();
		binarize(threshold.value());
		auto res = AdvancedComparatorBoolOut(binarization_buff, width, bytes, out_line, &lineinfo->offset_start,
			&lineinfo->offset_end, &lineinfo->pixel_per_bit);

		if (!res) {
			fail_result(initialThreshold);
			lineinfo->success = LineInfo::NoData;
			return; // FAIL!
		}

		lineinfo->success = LineInfo::Error;

		for (;;) {
			if (res && (0 == crc16_ccitt(bytes, sizeof(bytes)))) {
				lineinfo->success = LineInfo::Success;
				lineinfo->trys = threshold.step() + 1;
				break; // SUCCESS!
			}
			else if (!threshold.next()) {
				fail_result(initialThreshold);
				break; // FAIL!
			}

			binarize(threshold.value());
			res = AdvancedComparatorBoolOut(binarization_buff, width, bytes, out_line, &lineinfo->offset_start,
				&lineinfo->offset_end, &lineinfo->pixel_per_bit);
		}
	}

	void execute2() {
		uint8_t bytes[DATA_BYTES_PER_PCM_STRING];

		lineinfo->treshold = threshold.value();
		binarize(threshold.value());

		PCMPixelReader reader;

		if (!detectct_syncro(reader)) {
			lineinfo->success = LineInfo::NoData;
			lineinfo->trys = 1;
			return;
		}

		const std::vector<uint8_t> initial_binarisation_result(binarization_buff, &binarization_buff[width]);
		lineinfo->success = LineInfo::Error;
		for (;;) {
			read_string(bytes, reader);
			if (0 == crc16_ccitt(bytes, sizeof(bytes))) {
				lineinfo->success = LineInfo::Success;
				lineinfo->trys = threshold.step() + 1;
				break; // SUCCESS!
			}
			else if (!threshold.next()) {
				std::copy(initial_binarisation_result.cbegin(), initial_binarisation_result.cend(), binarization_buff);
				lineinfo->trys = threshold.maximum();
				break; // FAIL!
			}

			binarize(threshold.value());
		}
	}

private:
	uint8_t *pixel_data;
	int32_t width;
	uint8_t *binarization_buff;
	uint8_t *out_line;
	TresholdProvider threshold;
	LineInfo *lineinfo;
	
	struct PCMPixelReader {
		PCMPixelReader(uint8_t *buff = nullptr, double bpp = 0.0) : buff(buff), bpp(bpp) { }

		void configure(uint8_t *buff, double bpp) {
			this->buff = buff;
			this->bpp = bpp;
		}

		uint8_t value(uint8_t npixel) const {
			auto base_offset = uint32_t(round(bpp * (npixel + 0.5))) + 1;

			auto v = 0;
			for (auto i = -1; i < 2; ++i) {
				v += buff[base_offset + i];
			}
			return v > 1;
		}

	private:
		uint8_t *buff;
		double bpp;
	};

	void read_string(uint8_t* bytes, const PCMPixelReader& reader) {
		std::memset(bytes, 0, DATA_BYTES_PER_PCM_STRING);

		for (auto i = 0; i < DATA_BYTES_PER_PCM_STRING * CHAR_BIT; ++i) {
			auto v = out_line[i] = reader.value(i);
			if (v) {
				bytes[i >> 3] |= 1 << (7 - i % 8);
			}
		}
	}

	void binarize(uint8_t threshold) {
		for (auto i = 0; i < width; ++i) {
			binarization_buff[i] = pixel_data[i] >= threshold;
		}
	}

	void fail_result(const TresholdProvider &give_up_threshold) {
		uint16_t _CRC;

		binarize(give_up_threshold.value());
		ComparatorPlusCRCBoolOut(binarization_buff, width, out_line, &_CRC, &lineinfo->offset_start, &lineinfo->offset_end, &lineinfo->pixel_per_bit);

		lineinfo->trys = threshold.maximum();
	}

	bool detectct_syncro(PCMPixelReader& reader) {
		OwnForwardIterator<uint8_t> first_sync_start(binarization_buff, width);
		OwnForwardIterator<uint8_t> first_sync_end(first_sync_start);

		std::advance(first_sync_end, width / 5);

		auto raising_detect = [](const auto &p) {
			auto next = p;
			++next;
			return (*p == 0) && (*next == 1);
		};

		auto falling_detect = [](const auto &p) {
			auto next = p;
			++next;
			return (*p == 1) && (*next == 0);
		};

		auto sync1_start = find_it_if(first_sync_start, first_sync_end, raising_detect);

		if (sync1_start == first_sync_end) {
			return false;
		}

		auto sync1_end = find_it_if(sync1_start, first_sync_end, falling_detect);
		if (sync1_end == first_sync_end) {
			return false;
		}

		auto sync_line_width = std::distance(sync1_start, sync1_end);

		auto sync2_search = sync1_end;
		++sync2_search;
		auto sync2_end = find_it_if(sync2_search, first_sync_end, falling_detect);
		if (sync1_end == first_sync_end) {
			return false;
		}

		//---------------------------------------------------------------------------------------

		OwnReversIterator<uint8_t> white_level_start(binarization_buff, width);
		OwnReversIterator<uint8_t> white_level_end(white_level_start);

		std::advance(white_level_end, width / 5);

		auto white_level_adge = find_it_if(white_level_start, white_level_end, raising_detect);

		if (white_level_adge == white_level_end) {
			return false;
		}

		auto white_level_fall = find_it_if(white_level_adge, white_level_end, falling_detect);
		if (white_level_fall == white_level_end) {
			return false;
		}

		//---------------------------------------------------------------------------------------

		std::advance(white_level_fall, sync_line_width + 1);
		std::advance(sync2_end, sync_line_width + 1);

		auto pixel_pre_pcm_bit = (white_level_fall.start_offset() - sync2_end.start_offset()) /
			(double)(DATA_BYTES_PER_PCM_STRING * CHAR_BIT);

		if (pixel_pre_pcm_bit < 3.0) {
			return false;
		}

		reader.configure(&binarization_buff[sync1_start.start_offset()], pixel_pre_pcm_bit);

		static constexpr std::pair<int32_t, bool> reference[]{
			{0, true}, {1, false}, {2, true}, {3, false},
			// DATA_BYTES_PER_PCM_STRING_BITS
			// { 4 + 0...
			// { 4 + DATA_BYTES_PER_PCM_STRING_BITS - 1...
			{4 + DATA_BYTES_PER_PCM_STRING_BITS, false},
			{4 + DATA_BYTES_PER_PCM_STRING_BITS + 1, true}, {4 + DATA_BYTES_PER_PCM_STRING_BITS + 2, true},
			{4 + DATA_BYTES_PER_PCM_STRING_BITS + 3, true}, {4 + DATA_BYTES_PER_PCM_STRING_BITS + 4, true}
		};

		for (auto it = reference; it != &reference[std::size(reference)]; ++it) {
			if (reader.value(it->first) != it->second)
				return false;
		}

		lineinfo->offset_start = sync2_end.start_offset();
		lineinfo->offset_end = white_level_fall.start_offset();
		lineinfo->pixel_per_bit = pixel_pre_pcm_bit;

		reader.configure(&binarization_buff[sync2_end.start_offset() + sync_line_width], pixel_pre_pcm_bit);

		return true;
	}
};

extern "C" __declspec(dllexport) void ProcessFrameAutolevel(
	/* uint8_t[width][heigth] */ void* frame_data,
	/* frame size */ const int32_t heigth, const int32_t width,
	const uint32_t max_threads,

	/* out uint8_t[width][heigth] */ void* out_data,
	/* out avg. */ int32_t *avg_offset_start,
	/* out avg. */ int32_t *avg_offset_end,
	/* out avg. */ double *avg_pixel_per_bit,
	/* out */ int32_t *tracking,

	/* out */ int32_t *first_pcm_line,
	/* out */ int32_t *last_pcm_line
) {
	std::vector<LineInfo> lines_info(heigth);
	std::vector<uint8_t> binarisation_storage(width * heigth);

	myTwoDimArray<uint8_t> indata(frame_data, width, heigth);
	myTwoDimArray<uint8_t> outdata(out_data, BYTES_PER_PCM_STRING * CHAR_BIT, heigth);
	myTwoDimArray<uint8_t> binarisation_buf(binarisation_storage.data(), width, heigth);

	// paralel
	const auto N = max_threads <= 0
		? omp_get_max_threads()
		: std::min<decltype(omp_get_max_threads())>(max_threads, omp_get_max_threads());

	static constexpr uint32_t histogramm_fields_count = 6;

	uint32_t histogramm_data[histogramm_fields_count];
	uint8_t histogramm_thresholds[histogramm_fields_count];
	build_hystogramm_paralel_impl(indata, histogramm_data, histogramm_thresholds, histogramm_fields_count, N);

	uint32_t maximums[2];
	hystogramm_find_two_maximums(histogramm_data, histogramm_fields_count, &maximums[0], &maximums[1]);

	if (maximums[1] == histogramm_fields_count - 1) {
		--maximums[1];
	}

	auto threshold_index = hystogramm_find_minimum_index(histogramm_data, maximums[1], maximums[0]);

	auto range = (histogramm_thresholds[maximums[1]] - histogramm_thresholds[maximums[0]]) / 2;

	TresholdProvider initial_tresholdProvider(histogramm_thresholds[threshold_index], range);

#pragma omp parallel for num_threads(N)
	for (auto i = 0; i < heigth; ++i) {
		auto &li = lines_info.at(i);
		li.line = i;

		SingleLinePlocessor line_processor(indata.getLine(i), width, outdata.getLine(i),
			binarisation_buf.getLine(i), initial_tresholdProvider, &li);
		line_processor.execute2();
	}

	// single thread
	*first_pcm_line = -1;
	*last_pcm_line = -1;

	int success_lines = 0;
	int64_t summ_offset_start = 0;
	int64_t summ_offset_end = 0;
	double summ_pixel_per_bit = 0;
	double sum_trys = 0;
	std::for_each(lines_info.cbegin(), lines_info.cend(), [&sum_trys, &success_lines, &summ_offset_start, &summ_offset_end, &summ_pixel_per_bit,
		first_pcm_line, last_pcm_line](const LineInfo &result) {
		sum_trys += result.trys;
		if (result.success != LineInfo::NoData) {
			++success_lines;
			summ_offset_start += result.offset_start;
			summ_offset_end += result.offset_end;
			summ_pixel_per_bit += result.pixel_per_bit;

			if (*first_pcm_line < 0) {
				*first_pcm_line = result.line;
			}

			*last_pcm_line = -1;
		}
		else if ((*last_pcm_line < 0) && (result.success == LineInfo::NoData)) {
			*last_pcm_line = result.line;
		}
	});
	if (success_lines) {
		*avg_offset_start = static_cast<int32_t>(summ_offset_start / success_lines);
		*avg_offset_end = static_cast<int32_t>(summ_offset_end / success_lines);
		*avg_pixel_per_bit = summ_pixel_per_bit / success_lines;
	}
	else {
		*avg_offset_start = *avg_offset_end = -1;
		*avg_pixel_per_bit = 0;
	}
	*tracking = sum_trys;
}


extern "C" __declspec(dllexport) void ProcessFrame(
	/* uint8_t[width][heigth] */ void* frame_data,
	/* frame size */ const int32_t heigth, const int32_t width,
	const uint8_t threshold,
	const uint8_t threshold_range,
	const uint32_t max_threads,

	/* out uint8_t[width][heigth] */ void* out_data,
	/* out avg. */ int32_t *avg_offset_start,
	/* out avg. */ int32_t *avg_offset_end,
	/* out avg. */ double *avg_pixel_per_bit,
	/* out */ int32_t *tracking,

	/* out */ int32_t *first_pcm_line,
	/* out */ int32_t *last_pcm_line
) {
	TresholdProvider initial_tresholdProvider(threshold, threshold_range);

	std::vector<LineInfo> lines_info(heigth);
	std::vector<uint8_t> binarisation_storage(width * heigth);

	myTwoDimArray<uint8_t> indata(frame_data, width, heigth);
	myTwoDimArray<uint8_t> outdata(out_data, BYTES_PER_PCM_STRING * CHAR_BIT, heigth);
	myTwoDimArray<uint8_t> binarisation_buf(binarisation_storage.data(), width, heigth);

	// paralel
	const auto N = max_threads <= 0
		? omp_get_max_threads()
		: std::min<decltype(omp_get_max_threads())>(max_threads, omp_get_max_threads());

#pragma omp parallel for num_threads(N)
	for (auto i = 0; i < heigth; ++i) {
		auto &li = lines_info.at(i);
		li.line = i;

		SingleLinePlocessor line_processor(indata.getLine(i), width, outdata.getLine(i),
			binarisation_buf.getLine(i), initial_tresholdProvider, &li);
		line_processor.execute();
	}

	// single thread
	*first_pcm_line = -1;
	*last_pcm_line = -1;

	int success_lines = 0;
	int64_t summ_offset_start = 0;
	int64_t summ_offset_end = 0;
	double summ_pixel_per_bit = 0;
	double sum_trys = 0;
	std::for_each(lines_info.cbegin(), lines_info.cend(), [&sum_trys, &success_lines, &summ_offset_start, &summ_offset_end, &summ_pixel_per_bit,
		first_pcm_line, last_pcm_line](const LineInfo &result) {
		sum_trys += result.trys;
		if (result.success != LineInfo::NoData) {
			++success_lines;
			summ_offset_start += result.offset_start;
			summ_offset_end += result.offset_end;
			summ_pixel_per_bit += result.pixel_per_bit;

			if (*first_pcm_line < 0) {
				*first_pcm_line = result.line;
			}

			*last_pcm_line = -1;
		}
		else if ((*last_pcm_line < 0) && (result.success == LineInfo::NoData)) {
			*last_pcm_line = result.line;
		}
	});
	if (success_lines) {
		*avg_offset_start = static_cast<int32_t>(summ_offset_start / success_lines);
		*avg_offset_end = static_cast<int32_t>(summ_offset_end / success_lines);
		*avg_pixel_per_bit = summ_pixel_per_bit / success_lines;
	}
	else {
		*avg_offset_start = *avg_offset_end = -1;
		*avg_pixel_per_bit = 0;
	}
	*tracking = sum_trys;
}