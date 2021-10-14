#pragma once

#include <Windows.h>
#include <cstdint>

class c_config {
public:
    HMODULE hmodule;
    uint16_t cores = 287 * 5;
    uint16_t original_cores;
    void* globalsettings;
    bool(__fastcall* IsServer_original)(void*);
    int(__fastcall* SetBlockData_original)(void*, uint32_t, wchar_t*, uint32_t, uint32_t);
    __int64(__fastcall* SetRefreshRate_original)(void*, int);
};

extern c_config config;