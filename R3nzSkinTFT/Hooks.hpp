#pragma once

#include <Windows.h>

inline WNDPROC originalWndProc;

class Hooks {
public:
	void install() const noexcept;
	void uninstall() const noexcept;
};
