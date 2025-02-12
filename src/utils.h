#pragma once

#include <string>
#include <vector>
#include "stdint.h"

/* Formats byte vectors to be printable as hex strings or other arbitrary format */
std::string format_vec(const uint8_t *vec, size_t len, const char *format = nullptr);
std::string format_vec(const char *vec, size_t len);
std::string format_vec(const std::vector<uint8_t> &vec);

/* Formats uint16_t vectors to be printable as hex strings or other arbitrary format */
std::string format_vec(const uint16_t *vec, size_t len, const char *format = nullptr);
