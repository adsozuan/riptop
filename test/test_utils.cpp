#include <catch2/catch.hpp>
#include "../include/utils/formatter.h"

#include <cstdint>
#include <sstream>
#include <string>


TEST_CASE("Formatter ", "format_memory") { 

	REQUIRE(riptop::format_memory(12000) == "12KB");
	REQUIRE(riptop::format_memory(32768000) == "32MB");
	REQUIRE(riptop::format_memory(64000000000) == "64GB");
	REQUIRE(riptop::format_memory(2000000000000) == "2TB");
	REQUIRE(riptop::format_memory(4000000000000000) == "4PB");
}