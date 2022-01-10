#pragma once

#include <string>

namespace riptop {

	constexpr auto to_mega_bytes(int64_t x) { return x / 1048576; }

	std::string format_memory(int64_t memory);
}
