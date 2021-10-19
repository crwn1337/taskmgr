#include <Windows.h>
#include "main.h"

BOOL APIENTRY DllMain(HMODULE h_module, DWORD reason) {
	switch (reason) {
	case DLL_PROCESS_ATTACH:
		if (auto* thread = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)main::attach, h_module, 0, nullptr))
			CloseHandle(thread);
		break;

	case DLL_PROCESS_DETACH:
	default: break;
	}

	return TRUE;
}