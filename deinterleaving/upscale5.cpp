#include "stdafx.h"

#include <omp.h>

#include <cstdint>
#include <algorithm>
#include <cassert>


struct Accessor {
	Accessor(uint8_t* data, int32_t width, int32_t height) : data(data), width(width), height(height) {	}

	uint8_t* get(int32_t X, int32_t Y) {
		assert(X < width && Y < height);

		return data + Y * width + X;
	}

private:
	uint8_t* data;
	int32_t width, height;
};

/*
ABC
DEF
IGK
...
->
AAAAABBBBBCCCCC
DDDDDEEEEEFFFFF
IIIIIGGGGGKKKKK
...............
*/
extern "C" __declspec(dllexport) void upsacle5(
	uint8_t *indata, /* uint8_t[width][heigth] */
	int32_t width,
	int32_t heigth,
	int32_t copy_count, /* default 5 */
	/* out */ uint8_t *outdata, /* uint8_t[width * copy_count][heigth] */
	int32_t max_threads
) {
	const auto N = max_threads <= 0
		? omp_get_max_threads()
		: std::min<decltype(omp_get_max_threads())>(max_threads, omp_get_max_threads());

	Accessor src(indata, width, heigth);
	Accessor dest(outdata, width * copy_count, heigth);

#pragma omp parallel for num_threads(N)
	for(auto string = 0; string < heigth; ++string) {
		for (auto element = 0; element < width; ++element) {
			auto b = *src.get(element, string);

			memset(dest.get(element * copy_count, string), b, copy_count);
		}
	}
}