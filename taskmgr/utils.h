#pragma once
#include <string>
#include <vector>
#include <Windows.h>
#include <sstream>
#include <Psapi.h>

namespace utils {
	extern uint32_t rgb_to_cmyk(int R, int G, int B);
	extern std::vector<int> string_to_hex_array(char* string);
	extern size_t find_pattern(const std::string& mod, const std::string& pattern);
}