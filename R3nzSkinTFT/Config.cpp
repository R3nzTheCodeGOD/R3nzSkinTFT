#include <fstream>
#include <string>
#include <system_error>

#include <Windows.h>
#include <ShlObj.h>

#include "Json/json.hpp"

#include "CheatManager.hpp"
#include "Memory.hpp"
#include "Utils.hpp"

void Config::init() noexcept
{
	if (PWSTR pathToDocuments; SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Documents, 0, nullptr, &pathToDocuments))) {
		this->path = pathToDocuments;
		CoTaskMemFree(pathToDocuments);
	}

	this->path /= "R3nzSkin";
}

void Config::save() noexcept
{
	static const auto player{ cheatManager.memory->localPlayer };
	std::error_code ec;
	std::filesystem::create_directory(this->path, ec);
	auto out{ std::ofstream(this->path / u8"R3nzSkinTFT")};

	if (!out.good())
		return;

	config_json["currentComboSkinIndex"] = this->currentComboSkinIndex;
	config_json["curretSkinId"] = this->curretSkinId;

	out << config_json.dump();
	out.close();
}

void Config::load() noexcept
{
	const auto player{ cheatManager.memory->localPlayer };
	auto in{ std::ifstream(this->path / u8"R3nzSkinTFT") };

	if (!in.good())
		return;

	if (json j{ json::parse(in, nullptr, false, true) }; j.is_discarded())
		return;
	else
		config_json = j;

	this->currentComboSkinIndex = config_json.value("currentComboSkinIndex", 0);
	this->curretSkinId = config_json.value("curretSkinId", 1);

	if (this->curretSkinId < 1)
		this->curretSkinId = 1;

	in.close();
}

void Config::reset() noexcept
{
	this->currentComboSkinIndex = 0;
	this->curretSkinId = 1;
}
