#pragma once

#include <string>

namespace riptop {

const int64_t BYTE = 1;
const int64_t KILOBYTE = 1000ULL;
const int64_t MEGABYTE = KILOBYTE * KILOBYTE;
const int64_t GIGABYTE = MEGABYTE * KILOBYTE;
const int64_t TERABYTE = GIGABYTE * KILOBYTE;
const int64_t PETABYTE = TERABYTE * KILOBYTE;

constexpr auto to_mega_bytes(int64_t x) { return x / 1048576; }

std::string format_memory(int64_t memory);

}  // namespace riptop
