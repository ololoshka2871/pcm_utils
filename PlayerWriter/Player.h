#pragma once

#include <vector>
#include <memory>

#include "LockingQueue.h"

#include "portaudio.h"

struct Player {
	struct audioContainer {
		static constexpr auto samples_pre_frame = 2;

		enum CMD {
			C_EOF = -1,
			C_MUTE = -2
		};

		audioContainer() : data{}, offset{0} {}
		audioContainer(uint16_t samples[], uint32_t samples_count)
			: data{ &samples[0], &samples[samples_count] }, offset{0} {}

		size_t avalable() const { return offset >= 0 
			? data.size() - offset 
			: 0; 
		}

		size_t read_to(void* ptr, size_t count);

		static audioContainer eof() {
			audioContainer res;
			res.offset = C_EOF;
			return res;
		}

		static audioContainer mute() {
			audioContainer res;
			res.offset = C_MUTE;
			return res;
		}

		bool is_end() const { return offset == C_EOF; }
		bool is_mute() const { return offset == C_MUTE; }

	private:
		std::vector<uint16_t> data;
		int32_t offset;
	};

	Player(int32_t outpul_index, int32_t buf_size, int32_t sample_rate);
	~Player();

	void play(uint16_t samples[], uint32_t samples_count);
	void Mute();

private:
	PaStream* stream;
	LockingQueue<audioContainer> playQueue;

	std::unique_ptr<audioContainer> currentelement;

	int callback(const void* input, void* output,
		unsigned long frameCount,
		const PaStreamCallbackTimeInfo* timeInfo,
		PaStreamCallbackFlags statusFlags);
};