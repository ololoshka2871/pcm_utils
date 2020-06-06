#include <omp.h>

#include <vector>
#include <algorithm>
#include <cassert>

#include "Block.h"
#include "16BitDetector.h"

struct SingleStair {
	SingleStair() : summ{}, parity{} {}

	bool isPaityZero() const {
		return (parity.bits[0] + parity.bits[1]) == 0;
	}

	uint32_t summ;
	Block2 parity;
};

static void process_single_stair(SingleStair& res, int item, BlockAccessor<Block2> &src) {
	static constexpr auto line_shift = 16;
	static constexpr auto Q_column_offset = 98 / Block2::bits_per_block;

	for (auto step = 0; step < Block14::bits_per_block / Block2::bits_per_block; ++step) {
		auto row = item + step * line_shift;
		auto column = Q_column_offset + step;

		auto &block = *src.getBlock(row, column);

		res.parity ^= block;
		res.summ += block.summ();
	}
}


extern "C" __declspec(dllexport) void detect16Bit(
	void* input_array /* raw frame uint8_t[112 + 16][heigth] */,
	const int32_t heigth,
	const int32_t max_to_process,
	double *white_part,
	double *q_parity_ok_part,
	int32_t max_threads
) {
	assert(white_part);
	assert(q_parity_ok_part);

	static constexpr auto stop_at_end = 100;

	const auto items_to_process = std::min<int32_t>(heigth - stop_at_end, max_to_process);

	const auto N = max_threads <= 0
		? omp_get_max_threads()
		: std::min<decltype(omp_get_max_threads())>(max_threads, omp_get_max_threads());

	BlockAccessor<Block2> src(input_array, heigth, 
		Block14::BlocksPerLine * Block14::bits_per_block + sizeof(uint16_t) * CHAR_BIT);

	std::vector<SingleStair> results(items_to_process);
	
#pragma omp parallel for num_threads(N)
	for (auto i = 0; i < items_to_process; ++i) {
		process_single_stair(results.at(i), i, src);
	}

	std::for_each(results.cbegin(), results.cend(), [white_part, q_parity_ok_part](const SingleStair& element) {
		*white_part += element.summ;
		*q_parity_ok_part += element.isPaityZero();
	});

	*white_part /= items_to_process * Block14::bits_per_block;
	*q_parity_ok_part /= items_to_process;
}