#include "hooks.h"
#include "config.h"
#include "utils.h"
#include "external/minhook/include/MinHook.h"
#include "external/bitmap/bitmap_image.hpp"

#include <iostream>
#include <chrono>

namespace hooks {
	bool __fastcall IsServer_hook(global_settings* settings) {
		if (!config.globalsettings) {
			printf("g_GlobalSettings: 0x%p\n", settings);
			config.globalsettings = settings;
			config.original_cores = settings->cores;
		}
		settings->cores = config.width * config.height;
		return config.IsServer_original(settings);
	}

	int __fastcall SetBlockData_hook(void* a1, uint32_t core, wchar_t* text, uint32_t fill_color, uint32_t border_color) {
		auto row = core % config.width;
		auto column = std::floor(core / config.width);
		static auto time = std::chrono::steady_clock::now();
		static int frame = 1;

		auto elapsed_time = std::chrono::steady_clock::now() - time;
		auto duration = std::chrono::duration<intmax_t, std::nano>{ std::nano::den / config.fps };
		if (core == 0 && elapsed_time >= duration) {
			using namespace std::literals;
			time = (std::chrono::steady_clock::now() - (elapsed_time - std::chrono::nanoseconds(duration.count())));
			frame++;
		}

		auto color = config.colors[frame][core];
		return config.SetBlockData_original(a1, core, (wchar_t*)L"", color, color);
	}

	__int64 __fastcall GetBlockWidth_hook(void* a1) {
		return 17; // magic number
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

		auto GetBlockWidth = utils::find_pattern("Taskmgr.exe", "48 83 EC ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 44 24 ? 66 0F 6F 05");
		printf("GetBlockWidth: 0x%p\n", (void*)GetBlockWidth);
		MH_CreateHook((void*)GetBlockWidth, GetBlockWidth_hook, (void**)&config.GetBlockWidth_original);
		MH_QueueEnableHook((void*)GetBlockWidth);

		auto SetRefreshRate = utils::find_pattern("Taskmgr.exe", "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC ? 48 8B 3D ? ? ? ? 8B F2");
		config.SetRefreshRate_original = decltype(config.SetRefreshRate_original)(SetRefreshRate);
		printf("SetRefreshRate: 0x%p\n", (void*)SetRefreshRate);
		config.SetRefreshRate_original(nullptr, 0);

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