#include "hooks.h"

#include "external/minhook/include/MinHook.h"
#include "config.h"
#include "utils.h"
#include <iostream>
#include <wingdi.h>
#include <winnt.h>

namespace hooks {
	bool __fastcall IsServer_hook(void* a) {
		if (!config.globalsettings) {
			config.globalsettings = a;
			printf("g_GlobalSettings: 0x%p\n", a);
			config.original_cores = *(uint16_t*)((size_t)a + 0x944);
		}
		*(uint16_t*)((size_t)a + 0x944) = config.cores;
		return config.IsServer_original(a);
	}

	int __fastcall SetBlockData_hook(void* a1, uint32_t core, wchar_t* text, uint32_t fill_color, uint32_t border_color) {
		uint32_t color = utils::rgb_to_cmyk(255, 255, 255);
		std::wstring string = L"deez";
		return config.SetBlockData_original(a1, core, string.data(), color, color);
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
		config.cores = config.original_cores;
		config.IsServer_original(config.globalsettings);

		MH_DisableHook(MH_ALL_HOOKS);
		MH_RemoveHook(MH_ALL_HOOKS);
		MH_Uninitialize();
	}
}