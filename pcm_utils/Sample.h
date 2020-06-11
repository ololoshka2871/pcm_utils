#pragma once

#include <cstdint>
#include <initializer_list>
#include <cstdlib>

#ifdef _MSC_VER
#ifdef _EXPORTING
#define CLASS_DECLSPEC __declspec(dllexport)
#else
#define CLASS_DECLSPEC __declspec(dllimport)
#endif
#else
#define CLASS_DECLSPEC
#endif

struct CLASS_DECLSPEC Sample {
	static const auto bitsInSample = 14;

	Sample() = default;
	Sample(std::initializer_list<uint8_t> init);

	Sample operator^(const Sample& lr) const;
	uint8_t& operator[](const size_t element);
	uint8_t operator[](const size_t element) const;

	uint8_t values[bitsInSample];

	bool operator==(const Sample& lr);
	bool operator!=(const Sample& lr);

	Sample& operator=(const Sample& lr);

	Sample Reversed() const;

	void* ptr();

	bool isZero() const;
};

