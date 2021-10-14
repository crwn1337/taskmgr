#pragma once
#include <Windows.h>
#include <cstdio>

namespace main {
	extern void attach(HMODULE h_module);
	extern void detach();
}