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
 * 1. Ищем во входных данных последоватлеьность 1, 0
 * и сохраненем его индекс
 * 2. Ищем во входных данных последоватлеьность 0, 1
 *  ПОСЛЕ п.1 и сохраненем его индекс
 *  3. Вычислить расстояние между последовательностями
 *  п.1 и п.2
 *  4. Ищем во входных данных последоватлеьность 1, 0
 *  ПОСЛЕ п.2 и сохраненем его индекс
 *  5. К смещениею из п.4 прибавить результат п. 3 -> offset_start
 *  6. С конца данных найти последовательность 1, 0
 *  и сохранить индекс (!номер с конца!)
 *  7. К индексу из п. 6 прибавить разность п. 3 -> offset_end
 *  8. Из резмера массива вычесть результат п. 7
 *  9. Из общей длины массива входных данных вычасть сумму
 *  offset_start + offset_end
 *  10. Полученное в п.9 число делим на 128
 *  -> pixel_per_bit
 *  11. Теперь все значения между offset_start и offset_end
 *  в исходном массиве нужно превратить в выходные биты по pixel_per_bit
 *  на бит
 * / !В оригинале используются индексы конца поледовательностей!
 *  Из исходного массива данных извлекаем 3 значения, с индесом центрального
 *  равным pixel_per_bit * Номер_бита, проводим головование
 *  между полученными значениями, результат считаем значением этого
 *  бита
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