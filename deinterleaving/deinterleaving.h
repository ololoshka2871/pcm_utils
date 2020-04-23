#pragma once

#include <cstdint>

/**
 * input_array - ��������� ������ ����� ������ ������ �����
 * heigth - ���������� �����
 * output_array - ������ ��� �������� ���������
 * max_threads - ������������ ���������� �������
 */
extern "C" __declspec(dllexport) void deinterleave(
	void* input_array /* uint8_t[112][heigth] */,
	const int32_t heigth,
	/* out */ void* output_array /* uint8_t[112][heigth] */,
	int32_t max_threads
);