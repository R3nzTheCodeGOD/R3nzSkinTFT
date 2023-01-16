#include <cstdint>
#include <string>

#include "CheatManager.hpp"
#include "SkinDatabase.hpp"
#include "offsets.hpp"

std::int32_t SkinDatabase::getSkinsLenForModel(std::string model, const std::int32_t startIdx) const noexcept
{
	const auto defaultSkinData{ cheatManager.memory->getCharacterPackage(model, 0) };
	for (std::int32_t skinsLen{ startIdx };; ++skinsLen)
		if (cheatManager.memory->getCharacterPackage(model, skinsLen) == defaultSkinData)
			return skinsLen - 1;
}

void SkinDatabase::update() noexcept
{
	for (auto& pet : this->pets) {
		if (std::holds_alternative<std::int32_t>(pet.skinIds)) {
			pet.skinIds = this->getSkinsLenForModel(pet.modelName, std::get<std::int32_t>(pet.skinIds));
		} else if (std::holds_alternative<std::pair<std::int32_t, std::int32_t>>(pet.skinIds)) {
			const auto pair{ std::get<std::pair<std::int32_t, std::int32_t>>(pet.skinIds) };
			pet.skinIds = std::make_pair(pair.first, this->getSkinsLenForModel(pet.modelName, pair.first));
		}

		pet.skinName = cheatManager.memory->translateString(pet.skinName);
	}
}
