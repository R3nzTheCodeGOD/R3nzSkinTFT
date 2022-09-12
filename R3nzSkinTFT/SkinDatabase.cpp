#include <cstdint>
#include <string>

#include "CheatManager.hpp"
#include "SkinDatabase.hpp"
#include "offsets.hpp"

std::uint32_t SkinDatabase::getSkinsLenForModel(std::string model) noexcept
{
	static const auto getCharacterPackage{ reinterpret_cast<std::uintptr_t(__cdecl*)(std::string&,std::uint32_t)>(cheatManager.memory->getLeagueModule() + offsets::functions::FnCharacterData__GetCharacterPackage) };
	const auto defaultSkinData{ *reinterpret_cast<std::uintptr_t*>(getCharacterPackage(model, 0u) + 0x34) };
	for (auto skinsLen{ 1u };; ++skinsLen)
		if (*reinterpret_cast<std::uintptr_t*>(getCharacterPackage(model, skinsLen) + 0x34) == defaultSkinData)
			return skinsLen - 1u;
}

void SkinDatabase::update() noexcept
{
	// automatically update number of skins
	for (auto& pet : this->pets)
		pet.skinCount = this->getSkinsLenForModel(pet.modelName);
}
