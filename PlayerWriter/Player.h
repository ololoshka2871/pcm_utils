#pragma once

#include <vector>
#include <memory>
#include <algorithm>

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
		audioContainer(int16_t samples[], uint32_t samples_count, double volume)
			: data{ &samples[0], &samples[samples_count] }, offset{0} {
			if (volume < 0) { volume = 0; }
			if (volume > 1.0) { volume = 1.0; }

			std::for_each(data.begin(), data.end(), [volume](float& element) {
				element = element / (0xFFFF >> 1) * volume; 
			});
		}

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
		std::vector<float> data;
		int32_t offset;
	};

	Player(int32_t outpul_index, int32_t buf_size, int32_t sample_rate);
	~Player();

	void play(int16_t samples[], uint32_t samples_count, double volume);
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

#ifdef MSVC
#define DLLEXPORT DLLEXPORT
#else
#define DLLEXPORT
#endif

extern "C" DLLEXPORT void initPlayerContext(int32_t outpul_index, int32_t sample_rate,
    int32_t buf_size, void** ctx, int* errCode);

extern "C" DLLEXPORT void releasePlayerContext(void** ctx);

extern "C" DLLEXPORT void Play(void** ctx, int16_t samples[], uint32_t samples_count);

extern "C" DLLEXPORT void PlayVol(void** ctx, int16_t samples[], uint32_t samples_count,
    double volume);

extern "C" DLLEXPORT void Mute(void** ctx);
