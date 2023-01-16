#pragma once

#include <Windows.h>
#include <cstdint>
#include <d3d9.h>
#include <d3d11.h>

#include "Offsets.hpp"
#include "RetSpoofInvoker.hpp"

#include "SDK/AIBaseCommon.hpp"
#include "SDK/GameClient.hpp"
#include "SDK/Pad.hpp"

class offset_signature {
public:
	std::vector<std::string> pattern;
	bool sub_base;
	bool read;
	std::int32_t additional;
	std::uint32_t* offset;
};

class Memory {
public:
	void Search(bool gameClient = true) noexcept;
	
	const char* translateString(const char* string) const noexcept
	{
		return invoker.invokeCdecl<const char*>(this->moduleBase + offsets::functions::FnTranlateString, string);
	}

	std::uintptr_t getCharacterPackage(std::string& model, const std::int32_t idx) const noexcept
	{
		return *reinterpret_cast<std::uintptr_t*>(invoker.invokeCdecl<std::uintptr_t, std::string&>(this->moduleBase + offsets::functions::FnCharacterData__GetCharacterPackage, model, idx) + 0x34);
	}

	std::uintptr_t moduleBase;
	HWND riotWindow;
	GameClient* client;
	AIBaseCommon* localPlayer;
	IDirect3DDevice9* d3dDevice;
	IDXGISwapChain* swapChain;
private:
	void update(bool gameClient = true) noexcept;

	std::uintptr_t materialRegistry;

	std::vector<offset_signature> gameClientSig
	{
		{
			{
				"A1 ? ? ? ? 68 ? ? ? ? 8B 70 08 E8 ? ? ? ?",
				"A1 ? ? ? ? 56 83 78 08 00 75 ?"
			}, true, true, 0, &offsets::global::GameClient
		},
		{
			{
				"FF 23 80 7A ? ? 75 ? 66 8B 9D ? ? ? ?"
			}, true, false, 0, &offsets::global::retSpoofGadget
		}
	};

	std::vector<offset_signature> sigs
	{
		{
			{
				"8B 3D ? ? ? ? 3B F7 75 09 5F 5E",
				"A1 ? ? ? ? 8B 54 24 28 85 C0 0F 84 ? ? ? ?"
			}, true, true, 0, &offsets::global::LocalPlayer
		},
		{
			{
				"A3 ? ? ? ? 6A 64 6A 00",
				"8B 35 ? ? ? ? FF 15 ? ? ? ? 3B C6 75 ? 8B 0D ? ? ? ? 85 C9 74 ? 8B 01"
			}, true, true, 0, &offsets::global::Riot__g_window
		},
		{
			{
				"8D 8E ? ? ? ? FF 74 24 4C",
				"8D 8E ? ? ? ? FF 74 24 ? FF 74 24 ? E8 ? ? ? ? 8B 74 ? ? 85 F6 74 ? 8B C7 F0 0F C1 46 04"
			}, false, true, 0, &offsets::AIBaseCommon::CharacterDataStack
		},
		{
			{
				"8B B7 ? ? ? ? FF 70 28 8B 0E 56",
				"8B 8F ? ? ? ? 88 A7 ? ? ? ? 0F B6 C4 8B 11"
			}, false, true, 0, &offsets::MaterialRegistry::D3DDevice
		},
		{
			{
				"8D 9F ? ? ? ? 0F 84 ? ? ? ? 8B 0B 6A 00 6A 00 51  8B 01 FF 50 ? 85 C0",
				"8B 95 ? ? ? ? 0F 10 44 24 ? 56 8B 02 66 0F 73 D8 ?"
			}, false, true, 0, &offsets::MaterialRegistry::SwapChain
		},
		{
			{
				"E8 ? ? ? ? FF 73 58 8B C8 8B 10 FF 52 ?",
				"E8 ? ? ? ? 6A 00 6A 02 6A 00 8B 08 57 8B 51 ? 8B C8 FF D2 89 45 ?",
			}, true, false, 0, &offsets::functions::FnRiot__Renderer__MaterialRegistry__GetSingletonPtr
		},
		{
			{
				"E8 ? ? ? ? 8B 74 24 ? 85 F6 74 ? 8B C7 F0 0F C1 46 ? 75 ? 8B 06 8B CE FF 10 F0 0F C1 7E ? 4F 75 ? 8B 06 8B CE FF 50 ? 5F 5E 5B 83 C4 14 C2 44 00"
			}, true, false, 0, &offsets::functions::FnCharacterDataStack__Push
		},
		{
			{
				"E8 ? ? ? ? 83 C4 08 50 FF 74 24 44",
				"E8 ? ? ? ? F3 0F 10 44 24 ? 83 C4 08 C1 ED 1F"
			}, true, false, 0, &offsets::functions::FnCharacterData__GetCharacterPackage
		},
		{
			{
				"E8 ? ? ? ? 0F B6 0B 83 C4 04",
				"E8 ? ? ? ? 83 C4 04 8D 4E 0C 8B D0 80 38 00 5E 74 0C"
			}, true, false, 0, &offsets::functions::FnTranlateString
		}
	};
};
