#pragma once

#include <cstdint>

extern "C" __declspec(dllexport) uint16_t  crc16_ccitt(uint8_t *In_data, uint32_t size);
