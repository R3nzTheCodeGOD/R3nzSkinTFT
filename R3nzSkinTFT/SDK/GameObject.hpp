#pragma once

#include <cstdint>
#include <string>

#include "../offsets.hpp"

class GameObject {
public:
	CLASS_GETTER(std::string, getName, offsets::gameObject::name)
	CLASS_GETTER_P(std::string, getNamePtr, offsets::gameObject::name)
};