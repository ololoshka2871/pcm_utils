#pragma once

#include <cstdint>

#include "DLLEXPORT.h"

extern "C" DLLEXPORT uint16_t  crc16_ccitt(uint8_t *In_data, uint32_t size);
