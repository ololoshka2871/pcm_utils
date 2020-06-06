#include <cstdint>
#include <fstream>

#include "Comparator.h" 
#include "crc16_ccitt.h"

//static int string_num = 0;

extern "C" __declspec(dllexport) void ComparatorPlusCRC(uint8_t *data, int32_t size,
	/* out */ uint8_t bytes[DATA_OUT_BYTES],
	/* out */ int32_t *out_size,
	/* out */ uint16_t *crc,
	/* out */ int32_t *offset_start,
	/* out */ int32_t *offset_end,
	/* out */ double *pixel_per_bit) {

	//std::ofstream stream("C:\\Users\\Tanya\\Desktop\\pcm_visual\\lol\\__log.txt", std::ios::out | std::ios::app);

	Comparator(data, size, bytes, offset_start, offset_end, pixel_per_bit);

	*crc = *pixel_per_bit > 0.0
		? crc16_ccitt(bytes, DATA_OUT_BYTES)
		: 0xffff;

	/*
	stream
		<< "Counter = " << string_num++
		<< " offset_start: " << *offset_start
		<< " offset_end: " << *offset_end
		<< " pixel_per_bit: " << *pixel_per_bit
		<< " crc: " << *crc
		<< std::endl;
	*/
	*out_size = DATA_OUT_BYTES;

	//stream.close();
}

// Специальная версия, которая считает, что data это массив uint8_t, как оказалось, так лабвью передает его
// аналогично bytes разберем на псевдо-bool[]
extern "C" __declspec(dllexport) void ComparatorPlusCRCBoolOut(uint8_t* data, int32_t size,
	/* out */ void* bits /* [DATA_OUT_BYTES * 8] */,
	/* out */ uint16_t *crc,
	/* out */ int32_t *offset_start,
	/* out */ int32_t *offset_end,
	/* out */ double *pixel_per_bit) {

	uint8_t result_bytes[DATA_OUT_BYTES];

	ComparatorBoolOut(data, size, result_bytes, bits, offset_start, offset_end, pixel_per_bit);

	*crc = *pixel_per_bit > 0.0
		? crc16_ccitt(result_bytes, DATA_OUT_BYTES)
		: 0xffff;
}