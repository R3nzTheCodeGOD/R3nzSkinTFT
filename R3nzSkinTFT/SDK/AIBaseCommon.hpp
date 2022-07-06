#pragma once

#include <cstdint>

#include "CharacterDataStack.hpp"
#include "GameObject.hpp"
#include "Pad.hpp"

class AIBaseCommon : public GameObject {
public:
	CLASS_GETTER_P(CharacterDataStack, getCharacterDataStack, offsets::AIBaseCommon::CharacterDataStack)

	void changeSkin(const char* model, const std::int32_t skin) const noexcept;
};
