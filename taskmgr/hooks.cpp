﻿#include "hooks.h"

#include "external/minhook/include/MinHook.h"
#include "external/bitmap/bitmap_image.hpp"
#include "config.h"
#include "utils.h"
#include <iostream>
#include <wingdi.h>
#include <winnt.h>

namespace hooks {
	bool __fastcall IsServer_hook(global_settings* settings) {
		if (!config.globalsettings) {
			printf("g_GlobalSettings: 0x%p\n", settings);
			config.globalsettings = settings;
			config.original_cores = settings->cores;
		}
		settings->cores = config.cores;
		return config.IsServer_original(settings);
	}
	int __fastcall SetBlockData_hook(void* a1, uint32_t core, wchar_t* text, uint32_t fill_color, uint32_t border_color) {
		static bitmap_image image(config.image_path.string());
		if (!image) {
			printf("no image found or target image is invalid (should be 24bit bmp)\n");
			return config.SetBlockData_original(a1, core, text, fill_color, border_color);
		}

		if (image.width() != 41 || image.height() != 35) {
			printf("bmp width or height not correct! (should be 41, 35)\n");
			return config.SetBlockData_original(a1, core, text, fill_color, border_color);
		}

		unsigned int row = core % 41;
		unsigned int column = (unsigned int)std::floor(core / 41);

		rgb_t color;

		image.get_pixel(row, column, color);

		uint32_t cmyk = utils::rgb_to_cmyk(color.red, color.green, color.blue);
		return config.SetBlockData_original(a1, core, (wchar_t*)L"", cmyk, cmyk);
	}

	void init() {
		MH_Initialize();
		auto IsServer = utils::find_pattern("Taskmgr.exe", "40 53 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 ? ? ? ? 83 B9");
		printf("IsServer: 0x%p\n", (void*)IsServer);
		MH_CreateHook((void*)IsServer, IsServer_hook, (void**)&config.IsServer_original);
		MH_QueueEnableHook((void*)IsServer);

		auto SetBlockData = utils::find_pattern("Taskmgr.exe", "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 56 41 57 48 83 EC ? 48 8B 41 ? 48 8B F9");
		printf("SetBlockData: 0x%p\n", (void*)SetBlockData);
		MH_CreateHook((void*)SetBlockData, SetBlockData_hook, (void**)&config.SetBlockData_original);
		MH_QueueEnableHook((void*)SetBlockData);

		auto SetRefreshRate = utils::find_pattern("Taskmgr.exe", "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC ? 48 8B 3D ? ? ? ? 8B F2");
		config.SetRefreshRate_original = decltype(config.SetRefreshRate_original)(SetRefreshRate);
		printf("SetRefreshRate: 0x%p\n", (void*)SetRefreshRate);
		config.SetRefreshRate_original(nullptr, 1000 / 60);

		MH_ApplyQueued();
	}

	void deinit() {
		// most of this is useless since taskmanager doesn't use new values once initialized
		config.SetRefreshRate_original(nullptr, 1000);
		config.globalsettings->cores = config.original_cores;

		MH_DisableHook(MH_ALL_HOOKS);
		MH_RemoveHook(MH_ALL_HOOKS);
		MH_Uninitialize();
	}
}