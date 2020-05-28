#pragma once


#include "portaudio.h"

struct Player {
	Player(int32_t sample_rate);
	~Player();

	PaError play(uint16_t samples[], uint32_t samples_count);

private:
	PaStream* stream;
};