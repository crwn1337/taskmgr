#pragma once

#include <Windows.h>
#include <cstdint>
#include <filesystem>

struct global_settings {
    char pad[0x944];
    uint16_t cores;
};

class c_config {
public:
    HMODULE hmodule;
    std::filesystem::path image_path;
    uint16_t cores = 41 * 35; // width * height
    uint16_t original_cores;
    global_settings* globalsettings{};
    bool(__fastcall* IsServer_original)(void*);
    int(__fastcall* SetBlockData_original)(void*, uint32_t, wchar_t*, uint32_t, uint32_t);
    __int64(__fastcall* SetRefreshRate_original)(void*, int);
};

extern c_config config;