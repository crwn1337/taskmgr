#include "main.h"

#include "config.h"
#include "hooks.h"

namespace main {
	void attach(HMODULE h_module) {
		DisableThreadLibraryCalls(h_module);

		config.hmodule = h_module;

		AllocConsole();
		freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
		SetConsoleTitleA("taskmgr");
		printf("Uninject with DEL\n");

		hooks::init();

		while (!GetAsyncKeyState(VK_DELETE))
			Sleep(25);

		main::detach();
	}

	void detach() {
		FreeConsole();
		fclose(stdout);

		hooks::deinit();

		FreeLibraryAndExitThread(config.hmodule, 0);
	}
}