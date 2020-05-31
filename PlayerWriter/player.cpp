#include "pch.h"

#include <cstdint>
#include <algorithm>

#include "Player.h"

// http://portaudio.com/docs/v19-doxydocs/writing_a_callback.html

static PaStreamParameters getOutputByIndex(int index) {
	if (index >= Pa_GetDeviceCount()) {
		throw ENOMEM;
	}

	if (index == 0) {
		index = Pa_GetDefaultOutputDevice();
		auto di = Pa_GetDeviceInfo(index);

		return PaStreamParameters{
			index,
			2,
			paFloat32,
			di->defaultHighOutputLatency,
			nullptr
		};
	} else {
		auto numDevices = Pa_GetDeviceCount();

		int out_dev_index = -1;
		for (auto i = 0; i < numDevices; ++i) {
			auto info = Pa_GetDeviceInfo(i);

			if (info->maxOutputChannels > 0) {
				out_dev_index++;
				if (out_dev_index == index) {
					return PaStreamParameters{
						i,
						2,
						paFloat32,
						info->defaultHighOutputLatency,
						nullptr
					};
				}
			}
		}

		throw ENODEV;
	}
}

extern "C" __declspec(dllexport) void initPlayerContext(int32_t outpul_index, int32_t sample_rate, 
	int32_t buf_size, void** ctx, int* errCode) {
	try {
		if (buf_size < 1) {
			throw ENOMEM;
		}

		auto err = Pa_Initialize();
		if (err != paNoError) {
			throw err;
		}

		*ctx = new Player(outpul_index, buf_size, sample_rate);
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

extern "C" __declspec(dllexport) void Play(void** ctx, int16_t samples[], uint32_t samples_count) {
	auto _this = static_cast<Player*>(*ctx);
	_this->play(samples, samples_count, 1.0);
}

extern "C" __declspec(dllexport) void PlayVol(void** ctx, int16_t samples[], uint32_t samples_count,
	double volume) {
	auto _this = static_cast<Player*>(*ctx);
	_this->play(samples, samples_count, volume);
}

extern "C" __declspec(dllexport) void Mute(void** ctx) {
	auto _this = static_cast<Player*>(*ctx);
	_this->Mute();
}

Player::Player(int32_t output_index, int32_t buf_size, int32_t sample_rate) 
	: playQueue{ buf_size }, currentelement{new audioContainer } {
	auto  outputParameters = getOutputByIndex(output_index);

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
	},
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

void Player::play(int16_t samples[], uint32_t samples_count, double volume) {
	playQueue.push(audioContainer{ samples, samples_count, volume });
}

void Player::Mute() {
	if (!currentelement->is_mute()) {
		playQueue.push(audioContainer::mute());
	}
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
			output = static_cast<void*>(static_cast<float*>(output) + 
				read * audioContainer::samples_pre_frame);
			need_to_read -= read;
		} else {
			if (currentelement->is_mute() && playQueue.empty()) {
				std::memset(output, 0,
					frameCount * size_t(audioContainer::samples_pre_frame * sizeof(uint16_t)));
				return paContinue;
			} else {
				playQueue.pop(*currentelement);
				if (currentelement->is_end()) {
					return paAbort;
				}
			}
		}
	}
}

size_t Player::audioContainer::read_to(void* ptr, size_t frame_count) {
	auto begin = data.cbegin() + offset;
	auto to_read = std::min<size_t>(frame_count * samples_pre_frame, avalable());
	auto end = begin + to_read;
	std::copy(begin, end, static_cast<float*>(ptr));
	offset += to_read;
	return to_read / samples_pre_frame;
}
