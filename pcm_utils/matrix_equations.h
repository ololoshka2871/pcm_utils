#pragma once

#include "Sample.h"

Sample operator*(const uint8_t Tm6[Sample::bitsInSample][Sample::bitsInSample], const Sample& vector);