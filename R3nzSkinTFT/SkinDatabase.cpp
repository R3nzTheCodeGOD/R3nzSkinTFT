#include <cstdint>
#include <string>

#include "CheatManager.hpp"
#include "SkinDatabase.hpp"

void SkinDatabase::update() noexcept
{
	const auto getSkinsLenForModel = [](std::string model) noexcept -> std::uint32_t
	{
		const auto getCharacterPackage{ cheatManager.memory->getCharacterPackage };
		const auto defaultSkinData{ *reinterpret_cast<std::uintptr_t*>(getCharacterPackage(model, 0u) + 0x34) };
		for (auto skinsLen{ 1u };; skinsLen++) {
			if (*reinterpret_cast<std::uintptr_t*>(getCharacterPackage(model, skinsLen) + 0x34) == defaultSkinData)
				return skinsLen;
		}
	};

	// automatically update number of skins
	for (auto& pet : this->pets)
		pet.skinCount = getSkinsLenForModel(pet.modelName) - 1u;
}
