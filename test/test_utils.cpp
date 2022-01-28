#include <catch2/catch.hpp>
#include "../include/utils/utils.h"

#include <cstdint>
#include <sstream>
#include <string>


TEST_CASE("Formatter ", "FormatMemory") { 

	REQUIRE(riptop::FormatMemory(12000) == "12KB");
	REQUIRE(riptop::FormatMemory(32000000) == "32MB");
	REQUIRE(riptop::FormatMemory(64000000000) == "64GB");
	REQUIRE(riptop::FormatMemory(2000000000000) == "2TB");
	REQUIRE(riptop::FormatMemory(4000000000000000) == "4PB");
}