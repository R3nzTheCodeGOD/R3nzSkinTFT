#pragma warning(disable : 6387 4715)

#include <Windows.h>
#include <clocale>
#include <chrono>
#include <cstdint>
#include <thread>

#include "CheatManager.hpp"

#include "Config.hpp"
#include "GUI.hpp"
#include "Hooks.hpp"
#include "Memory.hpp"

#include "SDK/GameState.hpp"

bool NTAPI HideThread(HANDLE hThread) noexcept
{
	__try {
		const auto NtSetInformationThread{ reinterpret_cast<NTSTATUS(NTAPI*)(HANDLE, UINT, PVOID, ULONG)>(::GetProcAddress(::GetModuleHandle(TEXT("ntdll.dll")), "NtSetInformationThread")) };

		if (NtSetInformationThread == NULL)
			return false;

		if (const auto status{ NtSetInformationThread(hThread, 0x11u, NULL, 0ul) }; status == 0x00000000)
			return true;
	} __except (TRUE) {
		return false;
	}
}

static void WINAPI DllAttach([[maybe_unused]] LPVOID lp) noexcept
{
	using namespace std::chrono_literals;
	cheatManager.init();
	if (HideThread(::GetCurrentThread()))
		cheatManager.logger->addLog("[+] Thread Hided!\n");
	cheatManager.memory->Search();
	while (true) {
		std::this_thread::sleep_for(1s);
		
		if (!cheatManager.memory->client)
			cheatManager.memory->Search();
		else
			if (cheatManager.memory->client->gameState == GGameState_s::Running)
				break;
	}

	std::this_thread::sleep_for(500ms);
	cheatManager.memory->Search(false);
	std::this_thread::sleep_for(500ms);
	cheatManager.config->init();
	cheatManager.config->load();
	cheatManager.logger->addLog("[+] Config Loaded!\n");
	cheatManager.hooks->install();
	cheatManager.logger->addLog("[+] D3D Hooked!\n");
		
	while (cheatManager.cheatState)
		std::this_thread::sleep_for(250ms);

	::ExitProcess(0u);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID reserved)
{
	if (reason == DLL_PROCESS_ATTACH) {
		HideThread(hModule);
		std::setlocale(LC_ALL, ".utf8");
		::_beginthreadex(nullptr, 0u, reinterpret_cast<_beginthreadex_proc_type>(DllAttach), nullptr, 0u, nullptr);
		::CloseHandle(hModule);
		return TRUE;
	}
	return FALSE;
}
