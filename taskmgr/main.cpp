#include "main.h"
#include "config.h"
#include "hooks.h"
#include "utils.h"
#include "external/bitmap/bitmap_image.hpp"

#include <filesystem>

namespace main {
	void attach(HMODULE h_module) {
		DisableThreadLibraryCalls(h_module);

		config.hmodule = h_module;

		AllocConsole();
		freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
		SetConsoleTitleA("taskmgr");
		printf("uninject with DEL\n");

		printf("parsing files...\n");

		char dll[MAX_PATH];
		GetModuleFileNameA(config.hmodule, dll, sizeof dll);
		std::filesystem::path path = (std::filesystem::path(dll).parent_path() += "\\images");

		if (!std::filesystem::exists(path))
			printf("images folder doesn't exist, program might crash");

		for (const auto& dirEntry : std::filesystem::directory_iterator(path)) {
			if (dirEntry.path().extension() != ".bmp")
				continue;

			if (!utils::is_number(dirEntry.path().stem().string()))
				continue;

			bitmap_image image(dirEntry.path().string());
			if (!image) {
				printf("invalid bmp: %s\n", (char*)dirEntry.path().filename().string().data());
				continue;
			}

			if (image.width() != config.width || image.height() != config.height) {
				printf("bmp width or height not correct: %s\n", (char*)dirEntry.path().filename().string().data());
				continue;
			}

			rgb_t color;
			std::vector<uint32_t> colors;

			for (unsigned int j = 0; j < image.height(); j++) {
				for (unsigned int i = 0; i < image.width(); i++) {
					image.get_pixel(i, j, color);
					uint32_t cmyk = utils::rgb_to_cmyk(color.red, color.green, color.blue);
					colors.push_back(cmyk);
				}
			}
			config.colors[atoi(dirEntry.path().filename().string().c_str())] = colors;
		}

		printf("done parsing files\n");

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