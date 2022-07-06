#include <cstdint>

#include "AIBaseCommon.hpp"

#include "../CheatManager.hpp"
#include "../Offsets.hpp"

void AIBaseCommon::changeSkin(const char* model, const std::int32_t skin) const noexcept
{
	const auto stack{ this->getCharacterDataStack() };
	stack->baseSkin.skin = skin;
	stack->stack.clear();
	stack->push(model, skin);
}
