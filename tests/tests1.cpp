#include "stdafx.h"

#include "CppUnitTest.h"

#include <cassert>

#include "../pcm_utils/Comparator.h"
#include "../pcm_utils/ComparatorPlusCRC.h"
#include "../pcm_utils/crc16_ccitt.h"
#include "../pcm_utils/frame_process.h"

#include "../bitmap/bitmap_image.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static constexpr char frame_file[] = "C:\\LV\\c\\testlib\\tests\\test24.bmp";

namespace tests
{
	static uint8_t testdata[] = { 0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,1,1,1,1,1,0,0,0,0,0,
				0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,1,
				1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,
				1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,
				0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0
				,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
				0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
				1,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,
				1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,1,1,1,1,0,
				0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
				0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,
				1,1,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,
				0,0,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,
				1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,
				0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0
				,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,0
				,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1
				,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0 };

	TEST_CLASS(UnitTest1)
	{
	public:
		TEST_METHOD(Indata)
		{
			uint8_t _testdata[] = { 0, 0, 0, 1,1,0,0, 1,1,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0 };

			uint8_t result[16];

			int32_t offset_start;
			int32_t offset_end;
			double pixel_per_bit;

			Comparator(_testdata, sizeof(testdata), result, &offset_start, &offset_end, &pixel_per_bit);
		}

		TEST_METHOD(Realdata)
		{
			uint8_t result[16];

			int32_t offset_start;
			int32_t offset_end;
			double pixel_per_bit;

			Comparator(testdata, sizeof(testdata), result, &offset_start, &offset_end, &pixel_per_bit);
		}

		TEST_METHOD(RealdataPlusCRC)
		{
			uint8_t result[16];

			int32_t offset_start;
			int32_t offset_end;
			double pixel_per_bit;

			Comparator(testdata, sizeof(testdata), result, &offset_start, &offset_end, &pixel_per_bit);

			assert(crc16_ccitt(result, sizeof(result)) == (uint16_t)0);
		}


		TEST_METHOD(RealdataPlusCRCOnce)
		{
			uint8_t result[16];

			int32_t offset_start;
			int32_t offset_end;
			double pixel_per_bit;
			uint16_t crc;
			int32_t out_data_size;

			ComparatorPlusCRC(testdata, sizeof(testdata), result, &out_data_size, &crc, &offset_start, &offset_end, &pixel_per_bit);

			assert(crc == (uint16_t)0);
		}

		TEST_METHOD(Process_bmp)
		{
			bitmap_image image(frame_file);

			std::ofstream stream("log.txt");

			assert(image);

			auto string_size = image.width();
			auto string_count = image.height();

			auto stringbuf = new uint8_t[string_size];
			for (auto string = 10; string < string_count; ++string) {			
				for (auto pixel = 0; pixel < string_size; ++pixel) {
					rgb_t colour;
					image.get_pixel(pixel, string, colour);

					auto v = colour.red + colour.green + colour.blue;
					stringbuf[pixel] = v > 110 * 3;
				}


				uint8_t result[16];
				int32_t offset_start;
				int32_t offset_end;
				double pixel_per_bit;
				uint16_t crc;
				int32_t out_data_size;

				ComparatorPlusCRC(stringbuf, string_size, result, &out_data_size, &crc, &offset_start, &offset_end, &pixel_per_bit);

				stream 
					<< "Str: " << string
					<< " offset_start: " << offset_start
					<< " offset_end: " << offset_end
					<< " pixel_per_bit: " << pixel_per_bit
					<< " crc: " << crc
					<< std::endl;
			}

			stream.close();

			delete [] stringbuf;
		}

		TEST_METHOD(TestTrhesholdIt) {
			TresholdProvider pr(0, 2);

			assert(pr.value() == 0);
			assert(pr.next());
			assert(pr.value() == 1);
			assert(pr.next());
			assert(pr.value() == -1);
			assert(pr.next());
			assert(pr.value() == 2);
			assert(pr.next());
			assert(!pr.end());
			assert(pr.value() == -2);
			assert(!pr.next());
		}

		TEST_METHOD(ProcessAllFrame)
		{
			bitmap_image image(frame_file);

			auto frame = new uint8_t[image.pixel_count()];

			for (auto x = 0; x < image.width(); ++x) {
				for (auto y = 0; y < image.height(); ++y) {
					auto pixel = image.get_pixel(x, y);
					frame[y * image.width() + x] = ((pixel.red >> 5) << 5) + ((pixel.green >> 5) << 2) + (pixel.blue >> 6);
				}
			}

			const uint8_t start_threshold = 110;
			const uint8_t threshold_range = 30;
			auto outdata = new uint8_t[16 * 8 * image.height()];
			int32_t avg_offset_start;
			int32_t avg_offset_end;
			double avg_pixel_per_bit;
			int32_t tracking;
			int32_t first_pcm_line;
			int32_t last_pcm_line;

			ProcessFrame(frame, image.height(), image.width(), start_threshold, threshold_range, -1, outdata, 
				&avg_offset_start, &avg_offset_end, &avg_pixel_per_bit, &tracking, &first_pcm_line, &last_pcm_line);

			delete[] frame;
			delete[] outdata;
		}
	};
}