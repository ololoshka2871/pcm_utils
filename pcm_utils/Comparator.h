#pragma once

#include <cstdint>

#define DATA_OUT_BYTES 16

/**
 * data - data string
 * size - size of data
 * bytes - recovered data
 * offset_start - start pixel
 * ofset_end - end pixel
 * pixel_pre_bit - pixels pre bit
 * 1. ���� �� ������� ������ ������������������ 1, 0
 * � ���������� ��� ������
 * 2. ���� �� ������� ������ ������������������ 0, 1
 *  ����� �.1 � ���������� ��� ������
 *  3. ��������� ���������� ����� ��������������������
 *  �.1 � �.2
 *  4. ���� �� ������� ������ ������������������ 1, 0
 *  ����� �.2 � ���������� ��� ������
 *  5. � ��������� �� �.4 ��������� ��������� �. 3 -> offset_start
 *  6. � ����� ������ ����� ������������������ 1, 0
 *  � ��������� ������ (!����� � �����!)
 *  7. � ������� �� �. 6 ��������� �������� �. 3 -> offset_end
 *  8. �� ������� ������� ������� ��������� �. 7
 *  9. �� ����� ����� ������� ������� ������ ������� �����
 *  offset_start + offset_end
 *  10. ���������� � �.9 ����� ����� �� 128
 *  -> pixel_per_bit
 *  11. ������ ��� �������� ����� offset_start � offset_end
 *  � �������� ������� ����� ���������� � �������� ���� �� pixel_per_bit
 *  �� ���
 * / !� ��������� ������������ ������� ����� ������������������!
 *  �� ��������� ������� ������ ��������� 3 ��������, � ������� ������������
 *  ������ pixel_per_bit * �����_����, �������� �����������
 *  ����� ����������� ����������, ��������� ������� ��������� �����
 *  ����
 */
extern "C" __declspec(dllexport) void Comparator(uint8_t *data, int32_t size,
	/* out */ uint8_t bytes[DATA_OUT_BYTES],
	/* out */ int32_t *offset_start,
	/* out */ int32_t *ofset_end,
	/* out */ double *pixel_per_bit);

extern "C" __declspec(dllexport) void ComparatorBoolOut(uint8_t *data, int32_t size,
	/* out */ uint8_t bytes[DATA_OUT_BYTES],
	/* out */ void *bits/*[DATA_OUT_BYTES * 8]*/,
	/* out */ int32_t *offset_start,
	/* out */ int32_t *offset_end,
	/* out */ double *pixel_per_bit);

extern "C" __declspec(dllexport) uint8_t AdvancedComparatorBoolOut(uint8_t *data, int32_t size,
	/* out */ uint8_t bytes[DATA_OUT_BYTES],
	/* out */ void *bits/*[DATA_OUT_BYTES * 8]*/,
	/* out */ int32_t *offset_start1,
	/* out */ int32_t *offset_end,
	/* out */ double *pixel_per_bit);

extern "C" __declspec(dllexport) void ComparatorBaseBoolOut(uint8_t *data, int32_t size,
	int32_t offset_start,
	int32_t offset_end,
	double pixel_per_bit,
	/* out */ uint8_t bytes[DATA_OUT_BYTES],
	/* out */ void *bits, /*[DATA_OUT_BYTES * 8]*/
	/* out */ uint64_t *indexes /*[DATA_OUT_BYTES * 8]*/
);