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

uint32_t TresholdProvider::value() const {
	auto res =  offest & 1
		? base + offest / 2 + 1
		: base - offest / 2;

	return res;
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

	Resut result;

	int32_t offset_start;
	int32_t offset_end;
	double pixel_per_bit;

	uint32_t treshold;
	uint32_t line;
	uint32_t trys;
};

class SingleLinePlocessor {
public:
	SingleLinePlocessor(uint8_t *pixel_data, const int32_t width, uint8_t out_line[BYTES_PER_PCM_STRING * CHAR_BIT],
		uint8_t *binarization_buff, TresholdProvider threshold, LineInfo *lineinfo, double global_offset = 0.0)
		: pixel_data(pixel_data), width(width), out_line(out_line), binarization_buff(binarization_buff),
		threshold(threshold), lineinfo(lineinfo), global_offset(global_offset) {}

	void execute2(uint32_t width_divider, bool hard_verify_sync) {
		uint8_t bytes[DATA_BYTES_PER_PCM_STRING];

		auto initial_threshold = lineinfo->treshold = threshold.value();

		binarize(threshold.value());

		PCMPixelReader reader;

		if (!detectct_syncro(reader, width_divider, hard_verify_sync)) {
			lineinfo->result = LineInfo::NoData;
			lineinfo->trys = 1;
			return;
		}

		lineinfo->result = LineInfo::Error;
		read_string(bytes, reader);

		std::vector<uint8_t> initial_result(out_line, &out_line[width]);
		for (;;) {
			if (global_offset == 0.0) {
				if (try_parce(bytes, initial_result, initial_threshold, reader, width_divider)) {
					return;
				}
				lineinfo->trys++;
			}
			else {
				for (auto i = 0; i < 2; ++i) {
					auto df = (i == 0)
						? 0.0
						: i == 1
						? global_offset
						: -global_offset;
					
					if (try_parce(bytes, initial_result, initial_threshold, reader, width_divider, df)) {
						return;
					}
					lineinfo->trys++;
				}	
			}
		}
	}
	
private:
	uint8_t *pixel_data;
	int32_t width;
	uint8_t *binarization_buff;
	uint8_t *out_line;
	TresholdProvider threshold;
	LineInfo *lineinfo;
	double global_offset;
	
	struct PCMPixelReader {
		PCMPixelReader(uint8_t *buff = nullptr, double bpp = 0.0) : buff(buff), bpp(bpp) { }

		void configure(uint8_t *buff, double bpp, double global_offset = 0.0) {
			this->buff = buff;
			this->bpp = bpp;
			this->global_offset = global_offset;
		}

		uint8_t value(uint8_t npixel) const {
			return value(this->buff, npixel);
		}

		uint8_t value(uint8_t *_buff, uint8_t npixel) const {
			if (bpp < 3.0) {
				auto base_offset = uint32_t(round(bpp * npixel + global_offset));
				return _buff[base_offset] > 0;
			}
			else {
				auto base_offset = uint32_t(round(bpp * npixel + global_offset)) + 1;
				auto v = 0;
				for (auto i = -1; i < 2; ++i) {
					v += _buff[base_offset + i];
				}
				return v > 1;
			}
		}

		uint8_t value(uint8_t npixel, uint64_t* index) const {
			auto base_offset = uint32_t(round(bpp * npixel)) + 1;

			*index = (uint64_t)&buff[base_offset - 1];

			auto v = 0;
			for (auto i = -1; i < 2; ++i) {
				v += buff[base_offset + i];
			}
			return v > 1;
		}

	private:
		uint8_t *buff;
		double bpp;
		double global_offset;
	};


	bool  try_parce(uint8_t  bytes[DATA_BYTES_PER_PCM_STRING], std::vector<uint8_t> &initial_result, const uint32_t &initial_threshold,
		PCMPixelReader &reader, const uint32_t &width_divider, double offset = 0.0)
	{
		if (0 == crc16_ccitt(bytes, DATA_BYTES_PER_PCM_STRING)) {
			lineinfo->result = LineInfo::Success;
			return true; // SUCCESS!
		}
		else if (!threshold.next()) {
			std::copy(initial_result.cbegin(), initial_result.cend(), out_line);

			lineinfo->treshold = initial_threshold;
			lineinfo->trys = threshold.maximum();
			return true; // FAIL!
		}
		while (1) {
			lineinfo->treshold = threshold.value();
			binarize(threshold.value());
			if (detectct_syncro(reader, width_divider, offset)) {
				break;
			}
			else if (!threshold.next()) {
				std::copy(initial_result.cbegin(), initial_result.cend(), out_line);

				lineinfo->treshold = initial_threshold;
				lineinfo->trys = threshold.maximum();
				return true; // FAIL!
			}
		}
		read_string(bytes, reader);
		return false;
	}

	void read_string(uint8_t* bytes, const PCMPixelReader& reader) {
		std::memset(bytes, 0, DATA_BYTES_PER_PCM_STRING);

		for (auto i = 0; i < DATA_BYTES_PER_PCM_STRING * CHAR_BIT; ++i) {
			auto v = out_line[i] = reader.value(i/*, &indexes_b[i]*/);
			if (v) {
				bytes[i >> 3] |= 1 << (7 - i % 8);
			}
		}
	}

	void binarize(uint32_t threshold) {
		for (auto i = 0; i < width; ++i) {
			binarization_buff[i] = pixel_data[i] >= threshold;
		}
	}

	bool detectct_syncro(PCMPixelReader& reader, uint32_t width_divider, double offset, bool hard_verify_sync = false) {
		OwnForwardIterator<uint8_t> first_sync_start(binarization_buff, width);
		OwnForwardIterator<uint8_t> first_sync_end(first_sync_start);

		std::advance(first_sync_end, width / width_divider);

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

		auto sync1_end = find_it_if(first_sync_start, first_sync_end, falling_detect);
		if (sync1_end == first_sync_end) {
			return false;
		}

		OwnReversIterator<uint8_t> sync1_start_search(binarization_buff, width);
		OwnReversIterator<uint8_t> __start = sync1_start_search.End();
		std:advance(sync1_start_search, width - sync1_end.start_offset());

		auto sync1_start = find_it_if(sync1_start_search, __start, falling_detect);

		auto sync1_start_offset = (sync1_end == __start)
			? 0
			: sync1_start.start_offset();

		auto sync_line_width = sync1_end.start_offset() - sync1_start_offset;

		auto sync2_search = sync1_end;
		++sync2_search;

		auto sync2_start = find_it_if(sync2_search, first_sync_end, raising_detect);
		if (sync1_end == first_sync_end) {
			return false;
		}
			
		auto sync2_end = find_it_if(sync2_search, first_sync_end, falling_detect);
		if (sync1_end == first_sync_end) {
			return false;
		}

		//---------------------------------------------------------------------------------------

		OwnReversIterator<uint8_t> white_level_start(binarization_buff, width);
		OwnReversIterator<uint8_t> white_level_end(white_level_start);

		std::advance(white_level_end, width / width_divider);

		auto white_level_fall = find_it_if(white_level_start, white_level_end, falling_detect);
		if (white_level_fall == white_level_end) {
			return false;
		}		

		//---------------------------------------------------------------------------------------

		const auto sync1_end_off = sync1_end.start_offset();
		const auto sync2_start_offset = sync2_start.start_offset();
		auto sync_zero_size = sync2_start_offset - sync1_end_off;

		std::advance(white_level_fall, sync_zero_size);
		std::advance(sync2_end, sync_zero_size + 1);

		const auto data_pixels_count = white_level_fall.start_offset() - sync2_end.start_offset();
		const auto pixel_pre_pcm_bit = data_pixels_count / (double)(DATA_BYTES_PER_PCM_STRING * CHAR_BIT);
		
		if (hard_verify_sync) {
			OwnForwardIterator<uint8_t> __sync1_start(&binarization_buff[sync1_start_offset], 
				binarization_buff, &binarization_buff[width]);

			if (!verify_sync(reader, __sync1_start, pixel_pre_pcm_bit, offset)) {
				return false;
			}
		}

		lineinfo->offset_start = sync2_end.start_offset();
		lineinfo->offset_end = white_level_fall.start_offset();
		lineinfo->pixel_per_bit = pixel_pre_pcm_bit;

		reader.configure(&binarization_buff[sync2_end.start_offset() + 1], pixel_pre_pcm_bit, offset);

		return true;
	}

	bool verify_sync(SingleLinePlocessor::PCMPixelReader & reader, OwnForwardIterator<uint8_t> &sync1_start, 
		const double &pixel_pre_pcm_bit, const double offset)
	{
		reader.configure(&binarization_buff[sync1_start.start_offset()], pixel_pre_pcm_bit, offset);

		static constexpr std::pair<int32_t, bool> reference[]{
			{ 0, true },{ 1, false },{ 2, true },{ 3, false },
			// DATA_BYTES_PER_PCM_STRING_BITS
			// { 4 + 0...
			// { 4 + DATA_BYTES_PER_PCM_STRING_BITS - 1...
		{ 4 + DATA_BYTES_PER_PCM_STRING_BITS + 2, true },
		{ 4 + DATA_BYTES_PER_PCM_STRING_BITS + 3, true },{ 4 + DATA_BYTES_PER_PCM_STRING_BITS + 4, true }
		};

		for (auto it = reference; it != &reference[std::size(reference)]; ++it) {
			if (reader.value(it->first) != it->second)
				return false;
		}

		return true;
	}
};

static void generate_staticstics(int32_t * first_pcm_line, int32_t * last_pcm_line, std::vector<LineInfo> &lines_info,
	int32_t * avg_offset_start, int32_t * avg_offset_end, double * avg_pixel_per_bit, int32_t * error_lines, int32_t * tracking)
{
	*first_pcm_line = -1;
	*last_pcm_line = lines_info.size();

	uint32_t success_lines = 0;
	int32_t _error_lines = 0;
	int64_t summ_offset_start = 0;
	int64_t summ_offset_end = 0;
	double summ_pixel_per_bit = 0;
	double sum_trys = 0;
	std::for_each(lines_info.cbegin(), lines_info.cend(), [&sum_trys, &success_lines, &_error_lines, 
		&summ_offset_start, &summ_offset_end, &summ_pixel_per_bit, first_pcm_line](const LineInfo &result) {
		sum_trys += result.trys;

		if (result.result == LineInfo::Error) {
			++_error_lines;
		}

		if (result.result != LineInfo::NoData) {
			++success_lines;
			summ_offset_start += result.offset_start;
			summ_offset_end += result.offset_end;
			summ_pixel_per_bit += result.pixel_per_bit;

			if (*first_pcm_line < 0) {
				*first_pcm_line = result.line;
			}
		}
	});

	for (auto rit = lines_info.rbegin(); rit != lines_info.rend(); ++rit) {
		if (rit->result != LineInfo::NoData) {
			*last_pcm_line = rit->line;
			break;
		}
	}

	if (success_lines) {
		*avg_offset_start = static_cast<int32_t>(summ_offset_start / success_lines);
		*avg_offset_end = static_cast<int32_t>(summ_offset_end / success_lines);
		*avg_pixel_per_bit = summ_pixel_per_bit / success_lines;
		*error_lines = _error_lines;
	}
	else {
		*avg_offset_start = *avg_offset_end = -1;
		*avg_pixel_per_bit = 0;
	}
	*tracking = sum_trys;
}


extern "C" __declspec(dllexport) void ProcessFrameErrRateGoffset(
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
	/* out */ int32_t *last_pcm_line,
	const bool hard_verify_sync,
	const uint32_t width_divider,
	/* out */ int32_t *error_lines,
	double global_offset
) {
	TresholdProvider initial_tresholdProvider(threshold, threshold_range);

	std::vector<LineInfo> lines_info(heigth);
	std::vector<uint8_t> binarisation_storage(width * heigth);

	myTwoDimArray<uint8_t> indata(frame_data, width, heigth);
	myTwoDimArray<uint8_t> outdata(out_data, DATA_BYTES_PER_PCM_STRING * CHAR_BIT, heigth);
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
			binarisation_buf.getLine(i), initial_tresholdProvider, &li, global_offset);
		line_processor.execute2(width_divider, hard_verify_sync);
	}

	// single thread
	generate_staticstics(first_pcm_line, last_pcm_line, lines_info, avg_offset_start, avg_offset_end,
		avg_pixel_per_bit, error_lines, tracking);
}

extern "C" __declspec(dllexport) void ProcessFrameErrRate(
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
	/* out */ int32_t *last_pcm_line,
	const bool hard_verify_sync,
	const uint32_t width_divider,
	/* out */ int32_t *error_lines
) {
	TresholdProvider initial_tresholdProvider(threshold, threshold_range);

	std::vector<LineInfo> lines_info(heigth);
	std::vector<uint8_t> binarisation_storage(width * heigth);

	myTwoDimArray<uint8_t> indata(frame_data, width, heigth);
	myTwoDimArray<uint8_t> outdata(out_data, DATA_BYTES_PER_PCM_STRING * CHAR_BIT, heigth);
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
		line_processor.execute2(width_divider, hard_verify_sync);
	}

	// single thread
	generate_staticstics(first_pcm_line, last_pcm_line, lines_info, avg_offset_start, avg_offset_end, 
		avg_pixel_per_bit, error_lines, tracking);
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
	int32_t err_lines;

	const uint32_t width_divider = 5;

	ProcessFrameErrRate(frame_data, heigth, width, threshold, threshold_range, max_threads, out_data,
		avg_offset_start, avg_offset_end, avg_pixel_per_bit, tracking, first_pcm_line, last_pcm_line, false, width_divider, &err_lines);
}

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
	/* out */ int32_t *last_pcm_line,
	bool hard_verify_sync,
	const uint32_t width_divider,
	/* out */ int32_t *error_lines
) {
	std::vector<LineInfo> lines_info(heigth);
	std::vector<uint8_t> binarisation_storage(width * heigth);

	myTwoDimArray<uint8_t> indata(frame_data, width, heigth);
	myTwoDimArray<uint8_t> outdata(out_data, DATA_BYTES_PER_PCM_STRING * CHAR_BIT, heigth);
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

	auto range = (histogramm_thresholds[maximums[1]] - histogramm_thresholds[maximums[0]]) / 4;

	ProcessFrameErrRate(frame_data, heigth, width, histogramm_thresholds[threshold_index], range, max_threads, out_data,
		avg_offset_start, avg_offset_end, avg_pixel_per_bit, tracking, first_pcm_line, last_pcm_line, hard_verify_sync, width_divider, error_lines);
}
