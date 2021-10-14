#include "utils.h"

namespace utils {
	// TODO: fix this, uses a scuffed way to get around some problems (K defined as int)
	uint32_t rgb_to_cmyk(int R, int G, int B)
	{
		double C = 1 - ((255 - R) / (double)255);
		double M = 1 - ((255 - G) / (double)255);
		double Y = 1 - ((255 - B) / (double)255);
		int K = 1;
		if (C < K) K = (int)C;
		if (M < K) K = (int)M;
		if (Y < K) K = (int)Y;
		if (K == 1)
			C = M = Y = 1;
		else
		{
			C = (C - K) / (1 - K);
			M = (M - K) / (1 - K);
			Y = (Y - K) / (1 - K);
		}
		return (uint8_t)(C * 255) << 0 | (uint8_t)(M * 255) << 8 | (uint8_t)(Y * 255) << 16 | (uint8_t)(0xff) << 24;
	}

	std::vector<int> string_to_hex_array(const char* string) {
		std::vector<int> ret;

		for (auto current = string; current < string + strlen(string); ++current) {
			if (*current == '?') {
				++current;
				if (*current == '?')
					++current;
				ret.push_back(-1);
				continue;
			}
			auto a = strtoul(current, (char**)&current, 16);
			ret.push_back(a);
		}

		return ret;
	}

	size_t find_pattern(const std::string& mod, const std::string& pattern) {
		MODULEINFO module_info;
		auto* module_handle = GetModuleHandleA(mod.c_str());

		if (!module_handle) return 0;

		GetModuleInformation(GetCurrentProcess(), module_handle, &module_info, sizeof(module_info));

		auto start = (size_t)module_info.lpBaseOfDll;
		auto size = module_info.SizeOfImage;
		auto pat = string_to_hex_array(pattern.c_str());

		for (auto i = start; i < (start + size); i++) {
			for (size_t j = 0; j < pat.size(); j++) {
				if (pat[j] == -1)
					continue;

				auto p = *(uint8_t*)(i + j);
				if (p != pat[j])
					break;

				if (j == (pat.size() - 1))
					return i;
			}
		}
		return 0;
	}
}