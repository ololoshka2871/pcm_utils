#include "pch.h"

#include <cstdint>

#include "Player.h"

// http://portaudio.com/docs/v19-doxydocs/writing_a_callback.html

extern "C" __declspec(dllexport) void initPlayerContext(int32_t sample_rate, void **ctx, int *errCode) {
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

extern "C" __declspec(dllexport) int Play(void** ctx, uint16_t samples[], uint32_t samples_count) {
	return static_cast<Player*>(*ctx)->play(samples, samples_count);
}

Player::Player(int32_t sample_rate) {
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
		NULL, /* no callback, use blocking API */
		NULL); /* no callback, so no callback userData */
	if (err != paNoError) {
		throw err;
	}

	err = Pa_StartStream(stream);
	if (err != paNoError) {
		throw err;
	}
}

Player::~Player() {
	Pa_StopStream(stream);
	Pa_CloseStream(stream);
}

PaError Player::play(uint16_t samples[], uint32_t samples_count) {
	return Pa_WriteStream(stream, samples, samples_count);
}