#pragma once

#include <cstdint>

extern "C" __declspec(dllexport) void ComparatorPlusCRC(uint8_t *data, int32_t size,
	/* out */ uint8_t bytes[],
	/* out */ int32_t *out_size,
	/* out */ uint16_t *crc,
	/* out */ int32_t *offset_start,
	/* out */ int32_t *offset_end,
	/* out */ double *pixel_per_bit);

extern "C" __declspec(dllexport) void ComparatorPlusCRCBoolOut(uint8_t* data, int32_t size,
	/* out */ void* bits,
	/* out */ uint16_t *crc,
	/* out */ int32_t *offset_start,
	/* out */ int32_t *offset_end,
	/* out */ double *pixel_per_bit);
