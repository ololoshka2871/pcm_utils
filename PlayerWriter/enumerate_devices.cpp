
#include "pch.h"

#include <cstdint>
#include <list>
#include <string>
#include <algorithm>
#include <sstream>

#include "portaudio.h"

extern "C" __declspec(dllexport) void EnumerateAudioDevices(char* result, int32_t * pSize) {
	const char* const delim = "|";

	Pa_Initialize();

	auto numDevices = Pa_GetDeviceCount();

	if (numDevices > 0) {
		std::list<std::string> r;
		for (auto i = 0; i < numDevices; ++i) {
			auto info = Pa_GetDeviceInfo(i); // no need to free()

			if (info->maxOutputChannels > 0) {
				r.emplace_back(info->name);
			}
		}

		std::ostringstream imploded;
		std::copy(r.cbegin(), r.cend(),
			std::ostream_iterator<std::string>(imploded, delim));

		auto string = imploded.str();

		if (result && *pSize >= string.length()) {
			std::copy(string.cbegin(), string.cend(), result);
		}
		*pSize = string.length();
	} else {
		*pSize = 0;
	}

	Pa_Terminate();
}