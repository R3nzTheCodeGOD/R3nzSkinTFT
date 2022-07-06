#pragma once

#include <cstdint>
#include <filesystem>
#include <map>

#include "json/json.hpp"

#include "Utils.hpp"

using json = nlohmann::json;

class Config {
public:
	void init() noexcept;
	void save() noexcept;
	void load() noexcept;
	void reset() noexcept;

	std::int32_t currentComboSkinIndex{ 0 };
	std::int32_t curretSkinId{ 1 };
private:
	std::filesystem::path path;
	json config_json{ json() };
};
