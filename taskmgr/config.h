#pragma once

#include <Windows.h>
#include <map>
#include <vector>

struct global_settings {
	char pad[0x944];
	uint16_t cores;
};

class c_config {
public:
	HMODULE hmodule;
	std::map<uint64_t, std::vector<uint32_t>> colors; // frame, colors
	uint16_t width = 98;
	uint16_t height = 39;
	uint8_t fps = 3;
	uint16_t original_cores;
	global_settings* globalsettings;

	bool(__fastcall* IsServer_original)(void*);
	int(__fastcall* SetBlockData_original)(void*, uint32_t, wchar_t*, uint32_t, uint32_t);
	__int64(__fastcall* SetRefreshRate_original)(void*, int);
	__int64(__fastcall* GetBlockWidth_original)(void*);
};

extern c_config config;