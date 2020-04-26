#pragma once

#include <cstdint>

#ifdef _EXPORTING
#define CLASS_DECLSPEC __declspec(dllexport)
#else
#define CLASS_DECLSPEC __declspec(dllimport)
#endif

class CLASS_DECLSPEC TresholdProvider {
public:
	TresholdProvider();
	TresholdProvider(const uint8_t threshold, const uint8_t threshold_range);
	TresholdProvider(const TresholdProvider&) = default;

	uint32_t value() const;
	bool next();
	bool end() const;
	uint32_t step() const;

	bool plusHalf();
	uint32_t maximum() const;

private:
	int32_t base;
	uint32_t max_offest;
	uint32_t offest;
}; 

/**
frame_data - ��������� ������ uint8_t ��������
heigth, width - ������� ������� ��������

threshold - ����� ����������� ���������
threshold_range - ���������� ������ ������ ����������� threshold +/- threshold_range
max_threads - ������������ ���������� ������� ������������ ����� (<= 0 - �� ���������� ����)

outdata - ��������� ������ bool �������� ������� heigth x width
avg_offset_start - ������� �������� �������� ������ ��� ���� �����
avg_offset_end - ������� �������� ����� ��� ���� �����
avg_pixel_per_bit - ������� �������� �������� �� ��� pcm ��� ���� �����
tracking - TODO

first_pcm_line - ����� ������ ������, ��� ����� ���������� ������ pcm (����-�� ����������)
last_pcm_line - ����� ��������� ������, ��� ����� ���������� ������ pcm (����-�� ����������)
*/
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
);