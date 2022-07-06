#pragma once

#include <cstdint>

namespace offsets {
	namespace gameObject {
		inline std::uint32_t name{ 0x54 };
	};

	namespace global {
		inline std::uint32_t LocalPlayer{ 0 };
		inline std::uint32_t Riot__g_window{ 0 };
		inline std::uint32_t GameClient{ 0 };
	};

	namespace AIBaseCommon {
		inline std::uint32_t CharacterDataStack{ 0 };
	};

	namespace MaterialRegistry {
		inline std::uint32_t D3DDevice{ 0 };
		inline std::uint32_t SwapChain{ 0 };
	};

	namespace functions {
		inline std::uint32_t FnRiot__Renderer__MaterialRegistry__GetSingletonPtr{ 0 };
		inline std::uint32_t FnCharacterData__GetCharacterPackage{ 0 };
		inline std::uint32_t FnCharacterDataStack__Push{ 0 };
	};
};
