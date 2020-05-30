
#include "pch.h"

#include <chrono>
#include <algorithm>

#include "MyTwoDimArray.h"

#include "stc007binarizer.h"

extern "C" __declspec(dllexport) uint64_t parce_frame(
	/* uint8_t[width][heigth] */ void* frame_data,
	/* frame size */ const int32_t heigth, const int32_t width,
	const uint8_t threshold,
	const uint8_t threshold_range,
	const uint32_t max_threads,

	/* out uint8_t[width][heigth] */ void* out_data,
	/* out avg. */ int32_t * avg_offset_start,
	/* out avg. */ int32_t * avg_offset_end,
	/* out avg. */ double* avg_pixel_per_bit,
	/* out */ int32_t * tracking,

	/* out */ int32_t * first_pcm_line,
	/* out */ int32_t * last_pcm_line,
	const bool hard_verify_sync,
	const uint32_t width_divider,
	/* out */ int32_t * error_lines,
	double global_offset
) {
	static constexpr uint32_t DATA_BYTES_PER_PCM_STRING = 16;

	auto start = std::chrono::high_resolution_clock::now();

	STC007Binarizer binarizer;
	binarizer.setReferenceLevel(threshold);

	myTwoDimArray<uint8_t> indata(frame_data, width, heigth);
	myTwoDimArray<uint8_t> outdata(out_data, DATA_BYTES_PER_PCM_STRING * CHAR_BIT, heigth);

	*first_pcm_line = -1;
	*last_pcm_line = -1;

	double _avg_offset_start = 0;
	double _avg_offset_end = 0;
	double _avg_threshold = 0;

	uint32_t success_lines = 0;
	int32_t _error_lines = 0;
	int64_t summ_offset_start = 0;
	int64_t summ_offset_end = 0;
	for (uint16_t i = 0; i < heigth; ++i) {
		VideoLine videoline{ i, 0, indata.getLine(i), size_t(width), uint8_t(VideoLine::DEINT_OFF) };
		STC007Line outline;

		auto presult = binarizer.processLine(&videoline, &outline);

		auto line = outdata.getLine(i);
		for (auto bn = 0; bn < DATA_BYTES_PER_PCM_STRING * CHAR_BIT; ++bn) {
			auto cur = outline.bytes[bn / CHAR_BIT];
			line[bn] = !!(cur & (1 << (CHAR_BIT - 1 - bn % CHAR_BIT)));
		}

		if (outline.hasMarkers()) {
			if (outline.isCRCvalid()) {
				binarizer.setDataCoordinates(outline.data_start_coord, outline.data_stop_coord);
				binarizer.setReferenceLevel(outline.ref_level);

				// stat
				_avg_offset_start += outline.data_start_coord;
				_avg_offset_end += outline.data_stop_coord;
				_avg_threshold += outline.ref_level;
				success_lines++;

				if (*first_pcm_line < 0) {
					*first_pcm_line = i;
				}
				*last_pcm_line = -1;
			}
			else {
				++(*error_lines);
			}
		} else {
			if (*first_pcm_line >= 0 && *last_pcm_line < 0) {
				*last_pcm_line = i;
			}
		}
	}

	if (*last_pcm_line < 0) {
		*last_pcm_line = heigth;
	}

	*avg_offset_start = _avg_offset_start / success_lines;
	*avg_offset_end = _avg_offset_end / success_lines;
	*avg_pixel_per_bit = (*avg_offset_end - *avg_offset_start) / (DATA_BYTES_PER_PCM_STRING * double(CHAR_BIT));
	*tracking = _avg_threshold / success_lines;

	auto stop = std::chrono::high_resolution_clock::now();

	return std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count();
}