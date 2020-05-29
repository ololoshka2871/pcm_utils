#include "pch.h"

#include <cstdint>
#include <algorithm>
#include <thread>

#include "Player.h"

// http://portaudio.com/docs/v19-doxydocs/writing_a_callback.html

extern "C" __declspec(dllexport) void initPlayerContext(int32_t sample_rate, void** ctx, int* errCode) {
	try {
		auto err = Pa_Initialize();
		if (err != paNoError) {
			throw err;
		}

		*ctx = new Player(sample_rate);
		*errCode = 0;
	}
	catch (PaError err) {
		*ctx = nullptr;
		*errCode = err;
	}
}

extern "C" __declspec(dllexport) void releasePlayerContext(void** ctx) {
	if (*ctx) {
		delete static_cast<Player*>(*ctx);
		*ctx = nullptr;
	}
	Pa_Terminate();
}

extern "C" __declspec(dllexport) void Play(void** ctx, uint16_t samples[], uint32_t samples_count) {
	static_cast<Player*>(*ctx)->play(samples, samples_count);
}

Player::Player(int32_t sample_rate) : playQueue{ 10 }, currentelement{new audioContainer } {
	auto default_device = Pa_GetDefaultOutputDevice();
	PaStreamParameters outputParameters{
		default_device,
		2,
		paInt16,
		Pa_GetDeviceInfo(default_device)->defaultHighOutputLatency,
		nullptr
	};

	auto err = Pa_OpenStream(
		&stream,
		nullptr,
		&outputParameters,
		sample_rate,
		0,
		paClipOff,      /* we won't output out of range samples so don't bother clipping them */
		[](const void* input, void* output,
			unsigned long frameCount,
			const PaStreamCallbackTimeInfo* timeInfo,
			PaStreamCallbackFlags statusFlags,
			void* userData) {
		return static_cast<Player*>(userData)->callback(input, output, frameCount, timeInfo, statusFlags);
	}, /* no callback, use blocking API */
		this); /* no callback, so no callback userData */
	if (err != paNoError) {
		throw err;
	}

	err = Pa_StartStream(stream);
	if (err != paNoError) {
		throw err;
	}
}

Player::~Player() {
	playQueue.push(audioContainer::eof());
	std::this_thread::sleep_for(std::chrono::milliseconds(300));

	Pa_StopStream(stream);
	Pa_CloseStream(stream);
}

void Player::play(uint16_t samples[], uint32_t samples_count) {
	playQueue.push(audioContainer{ samples, samples_count });
}

int Player::callback(const void* input, void* output, unsigned long frameCount,
	const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags) {

	auto need_to_read = frameCount;

	while (need_to_read > 0) {
		if (currentelement->avalable() > 0) {
			auto read = currentelement->read_to(output, need_to_read);
			if (read == need_to_read) {
				return paContinue; // ok!
			}
			output = static_cast<void*>(static_cast<uint16_t*>(output) + read * audioContainer::samples_pre_frame);
			need_to_read -= read;
		} else {
			playQueue.pop(*currentelement);
			if (currentelement->is_end()) {
				return paAbort;
			}
		}
	}
}

size_t Player::audioContainer::read_to(void* ptr, size_t frame_count) {
	auto begin = data.cbegin() + offset;
	auto to_read = std::min<size_t>(frame_count * samples_pre_frame, avalable());
	auto end = begin + to_read;
	std::copy(begin, end, static_cast<uint16_t*>(ptr));
	offset += to_read;
	return to_read / samples_pre_frame;
}
