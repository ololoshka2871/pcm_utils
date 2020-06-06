#include <cstdint>
#include <iterator>
#include <algorithm>
#include <cassert>

#include "OwnIterator.h"
#include "find_it_if.h"

#include "Comparator.h"

static constexpr auto bits_per_line = 128;

void calc_offsets(uint8_t *data, int32_t size,
	/* out */ int32_t *offset_start,
	/* out */ int32_t *offset_end) {
	OwnForwardIterator<uint8_t> first(data, size);

	auto sync1_end = find_it_if(first, first.End(), [](OwnForwardIterator<uint8_t> p) {
		OwnForwardIterator<uint8_t> next = p.next();
		return (*p == 1) && (*next == 0);
	});

	auto sync2_start = find_it_if(sync1_end, first.End(), [](OwnForwardIterator<uint8_t> p) {
		return (*p == 0) && (*p.next() == 1);
	});

	auto black_line_width = std::distance(sync1_end, sync2_start);

	auto offset_start_it = find_it_if(sync2_start, first.End(), [](OwnForwardIterator<uint8_t> p) {
		return (*p == 1) && (*p.next() == 0);
	});
	std::advance(offset_start_it, black_line_width);
	*offset_start = offset_start_it.start_offset() + 1;

	OwnReversIterator<uint8_t> last(data, size);
	auto offset_end_it = find_it_if(last, last.End(), [](OwnReversIterator<uint8_t> p) {
		OwnReversIterator<uint8_t> next = p.next();
		return (*p == 1) && (*next == 0);
	});
	std::advance(offset_end_it, black_line_width);

	*offset_end = offset_end_it.start_offset();
}

extern "C" __declspec(dllexport) void Comparator(uint8_t *data, int32_t size,
	/* out */ uint8_t bytes[DATA_OUT_BYTES],
	/* out */ int32_t *offset_start,
	/* out */ int32_t *offset_end,
	/* out */ double *pixel_per_bit) {
	calc_offsets(data, size, offset_start, offset_end);

	std::memset(bytes, 0, DATA_OUT_BYTES);

	if ((*offset_start >= bits_per_line / 2)
		|| (*offset_end <= bits_per_line / 2)
		|| (*offset_start > *offset_end)) {
		*pixel_per_bit = -1.0;
		return;
	}

	auto data_pixels_count = *offset_end - *offset_start;

	*pixel_per_bit = data_pixels_count / (double)bits_per_line;

	uint32_t index = 0;
	for (auto bit_index_f = (*pixel_per_bit / 2) + *offset_start;
		index < bits_per_line;
		bit_index_f += *pixel_per_bit, ++index) {
		auto p_index = (uint32_t)round(bit_index_f) - 1;

		auto _of = p_index - *offset_start;

		if (p_index < 0) { p_index = 0; }

		uint8_t pixel_values[3];
		std::memcpy(pixel_values, &data[p_index], sizeof(pixel_values));

		uint8_t r = 0;
		for (auto j = 0; j < sizeof(pixel_values); ++j) {
			r += pixel_values[j];
		}

		if (r > 1) {
			bytes[index >> 3] |= 1 << (7 - index % 8);
		}
	}
}

extern "C" __declspec(dllexport) void ComparatorBoolOut(uint8_t *data, int32_t size,
	/* out */ uint8_t bytes[DATA_OUT_BYTES],
	/* out */ void *bits /*[DATA_OUT_BYTES * 8] */,
	/* out */ int32_t *offset_start,
	/* out */ int32_t *offset_end,
	/* out */ double *pixel_per_bit) {
	uint8_t *_bits = static_cast<uint8_t*>(bits);

	calc_offsets(data, size, offset_start, offset_end);

	std::memset(bytes, 0, DATA_OUT_BYTES);
	std::memset(bits, 0, DATA_OUT_BYTES * 8);

	if ((*offset_start >= bits_per_line / 2)
		|| (*offset_end <= bits_per_line / 2)
		|| (*offset_start > *offset_end)) {
		*pixel_per_bit = -1.0;
		return;
	}

	auto data_pixels_count = *offset_end - *offset_start;

	*pixel_per_bit = data_pixels_count / (double)bits_per_line;

	uint32_t index = 0;
	for (auto bit_index_f = (*pixel_per_bit / 2) + *offset_start;
		index < bits_per_line;
		bit_index_f += *pixel_per_bit, ++index) {
		auto p_index = (uint32_t)round(bit_index_f) - 1;

		auto _of = p_index - *offset_start;

		if (p_index < 0) { p_index = 0; }

		uint8_t pixel_values[3];
		std::memcpy(pixel_values, &data[p_index], sizeof(pixel_values));

		uint8_t r = 0;
		for (auto j = 0; j < sizeof(pixel_values); ++j) {
			r += pixel_values[j];
		}

		if (r > 1) {
			bytes[index >> 3] |= 1 << (7 - index % 8);
			_bits[index] = 1;
		}
	}
}

template<typename T>
static bool raising_detect(const T &p) {
	auto next = p.next();
	return (*p == 0) && (*next == 1);
}

template<typename T>
static bool falling_detect(const T &p) {
	auto next = p.next();
	return (*p == 1) && (*next == 0);
}

bool calc_offsets_dvanced(uint8_t *data, int32_t size,
	int32_t *offset_start, int32_t *offset_end) {
	// 1010xx...xxx01111

	const auto tolerance = 4;

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

	OwnForwardIterator<uint8_t> first(data, size);
	OwnForwardIterator<uint8_t> last = first.End();

	auto sync1_start = find_it_if(first, last, raising_detect);

	auto sync1_start_offset = sync1_start.start_offset();

	auto sync1_end = find_it_if(sync1_start, last, falling_detect);

	auto sync1_end_offset = sync1_end.start_offset();

	auto sync1_size = sync1_end_offset - sync1_start_offset;
	if (sync1_size <= 0) {
		return false;
	}

	auto sync2_start = find_it_if(sync1_end, last, raising_detect);

	auto sync2_start_offset = sync2_start.start_offset();

	auto sync_zero_size = sync2_start_offset - sync1_end_offset;
	if ((sync1_size <= 0) || (abs(sync_zero_size - sync1_size) > tolerance)) {
		return false;
	}

	auto sync2_end = find_it_if(sync2_start, last, falling_detect);

	auto sync2_end_offset = sync2_end.start_offset();

	auto sync2_size = sync2_end_offset - sync2_start_offset;
	if ((sync1_size <= 0) || (abs(sync2_size - sync1_size) > tolerance)) {
		return false;
	}

	OwnReversIterator<uint8_t> revfirst(data, size);
	OwnReversIterator<uint8_t> revlast = revfirst.End();

	auto ender_start = find_it_if(revfirst, revlast, raising_detect);

	auto ender_start_offset = ender_start.start_offset();

	auto ender_end = find_it_if(ender_start, revlast, falling_detect);

	auto ender_end_offset = ender_end.start_offset();

	auto ender_size = ender_start_offset - ender_end_offset;

	if (abs(ender_size - (sync1_size + sync_zero_size + sync2_size * 2)) > tolerance) {
		return false;
	}

	*offset_start = sync2_end_offset + sync_zero_size + 1;
	*offset_end = ender_end_offset - sync_zero_size;
}

extern "C" __declspec(dllexport) uint8_t AdvancedComparatorBoolOut(uint8_t *data, int32_t size,
	/* out */ uint8_t bytes[DATA_OUT_BYTES],
	/* out */ void *bits/*[DATA_OUT_BYTES * 8]*/,
	/* out */ int32_t *offset_start,
	/* out */ int32_t *offset_end,
	/* out */ double *pixel_per_bit) {
	uint8_t *_bits = static_cast<uint8_t*>(bits);

	if (!calc_offsets_dvanced(data, size, offset_start, offset_end))
	{
		*pixel_per_bit = -1.0;
		return false;
	}

	std::memset(bytes, 0, DATA_OUT_BYTES);
	std::memset(bits, 0, DATA_OUT_BYTES * 8);

	auto data_pixels_count = *offset_end - *offset_start;

	*pixel_per_bit = data_pixels_count / (double)bits_per_line;

	uint32_t index = 0;
	for (auto bit_index_f = (*pixel_per_bit / 2) + *offset_start;
		index < bits_per_line;
		bit_index_f += *pixel_per_bit, ++index) {
		auto p_index = (uint32_t)round(bit_index_f) - 1;

		auto _of = p_index - *offset_start;

		if (p_index < 0) { p_index = 0; }

		uint8_t pixel_values[3];
		std::memcpy(pixel_values, &data[p_index], sizeof(pixel_values));

		uint8_t r = 0;
		for (auto j = 0; j < sizeof(pixel_values); ++j) {
			r += pixel_values[j];
		}

		if (r > 1) {
			bytes[index >> 3] |= 1 << (7 - index % 8);
			_bits[index] = 1;
		}
	}
	return true;
}

extern "C" __declspec(dllexport) void ComparatorBaseBoolOut(uint8_t *data, int32_t size,
	int32_t offset_start,
	int32_t offset_end,
	double pixel_per_bit,
	/* out */ uint8_t bytes[DATA_OUT_BYTES],
	/* out */ void *bits, /*[DATA_OUT_BYTES * 8]*/
	/* out */ uint64_t *indexes /*[DATA_OUT_BYTES * 8]*/
) {
	uint8_t *_bits = static_cast<uint8_t*>(bits);

	std::memset(bytes, 0, DATA_OUT_BYTES);
	std::memset(bits, 0, DATA_OUT_BYTES * 8);

	uint32_t index = 0;
	for (auto bit_index_f = (pixel_per_bit / 2) + offset_start;
		index < bits_per_line;
		bit_index_f += pixel_per_bit, ++index) {
		auto p_index = (uint32_t)round(bit_index_f) - 1;

		indexes[index] = (uint64_t)&data[p_index];

		if (p_index < 0) { p_index = 0; }

		uint8_t pixel_values[3];
		std::memcpy(pixel_values, &data[p_index], sizeof(pixel_values));

		uint8_t r = 0;
		for (auto j = 0; j < sizeof(pixel_values); ++j) {
			r += pixel_values[j];
		}

		if (r > 1) {
			bytes[index >> 3] |= 1 << (7 - index % 8);
			_bits[index] = 1;
		}
	}
}