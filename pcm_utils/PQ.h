#pragma once

#include <cstdint>

/**
 * blocks - ��������� ������ bool 8 �����, 14 �����
 * index1, index2 - ������� ������� ������
 * restored_block1 - ��������������� ���� � ������� index1
 * restored_block1 - ��������������� ���� � ������� index2
 */
extern "C" __declspec(dllexport) void RestoreByQ(
	void* blocks /* uint8_t[8][14] */,
	const int32_t index1, const int index2,
	/* out */ void* restored_block1 /* uint8_t[14] */,
	/* out */ void* restored_block2 /* uint8_t[14] */
);

/**
 * blocks - ��������� ������ bool 8 �����, 14 �����
 * index1, index2 - ������� ������� ������
 * restored_blocks - ��������� ������ bool 8 �����, 14 �����
 */
extern "C" __declspec(dllexport) void RestoreByQ_FixedBlockOut(
	void* blocks /* uint8_t[8][14] */,
	int32_t index1, int index2,
	/* out */ void* restored_blocks /*uint8_t[8][14] */
);


/**
 * blocks - ��������� ������ bool 8 �����, 14 �����
 * P - ��������������� ���� � ������� index1
 * Q - ��������������� ���� � ������� index2
 */
extern "C" __declspec(dllexport) void GenerateP(
	void* blocks /* uint8_t[8][14] */,
	/* out */ void* P /* uint8_t[14] */
);


/**
 * blocks - ��������� ������ bool 8 �����, 14 �����
 * P - ��������������� ���� � ������� index1
 * Q - ��������������� ���� � ������� index2
 */
extern "C" __declspec(dllexport) void GenerateQ(
	void* blocks /* uint8_t[8][14] */,
	/* out */ void* Q /* uint8_t[14] */
);

/**
 * blocks - ��������� ������ bool 8 �����, 14 �����
 * P - ��������������� ���� � ������� index1
 * Q - ��������������� ���� � ������� index2
 */
extern "C" __declspec(dllexport) void GeneratePQ(
	void* blocks /* uint8_t[8][14] */,
	/* out */ void* P /* uint8_t[14] */,
	/* out */ void* Q /* uint8_t[14] */
);