#include <cstdint>
#include <string>
#include <mutex>
#include <vector>

#include "imgui/imgui.h"
#include "imgui/imgui_stdlib.h"

#include "CheatManager.hpp"
#include "GUI.hpp"
#include "Memory.hpp"
#include "SkinDatabase.hpp"
#include "Utils.hpp"

inline void footer() noexcept
{
	ImGui::Separator();
	ImGui::textUnformattedCentered((std::string("Last Build: ") + __DATE__ + " - " + __TIME__).c_str());
	ImGui::textUnformattedCentered("Copyright (C) 2022 R3nzTheCodeGOD");
}

void GUI::render() noexcept
{
	static const auto player{ cheatManager.memory->localPlayer };

	static const auto vectorGetter = [](void* vec, std::int32_t idx, const char** out_text) noexcept
	{
		const auto& vector{ *static_cast<std::vector<SkinDatabase::PetsInfo>*>(vec) };
		if (idx < 0 || idx > static_cast<std::int32_t>(vector.size())) return false;
		*out_text = idx == 0 ? "Default" : vector.at(idx - 1).skinName;
		return true;
	};

	std::call_once(this->changeSkin, [&]() noexcept -> void {
		if (cheatManager.config->currentComboSkinIndex > 0 && cheatManager.config->curretSkinId > cheatManager.database->pets[cheatManager.config->currentComboSkinIndex - 1].skinCount)
			cheatManager.config->curretSkinId = 1;

		if (cheatManager.config->currentComboSkinIndex > 0)
			player->changeSkin(cheatManager.database->pets[cheatManager.config->currentComboSkinIndex - 1].modelName, cheatManager.config->curretSkinId);
	});

	ImGui::Begin("R3nzSkin TFT", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_AlwaysAutoResize);
	if (ImGui::BeginTabBar("TabBar", ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_FittingPolicyScroll | ImGuiTabBarFlags_NoTooltip)) {
		if (ImGui::BeginTabItem("Skin Changer")) {
			if (ImGui::Combo("Current Pet", &cheatManager.config->currentComboSkinIndex, vectorGetter, static_cast<void*>(&cheatManager.database->pets), cheatManager.database->pets.size() + 1))
				cheatManager.config->curretSkinId = 1;
			
			ImGui::SliderInt("Current Pet SkinId", &cheatManager.config->curretSkinId, 1, cheatManager.database->pets[cheatManager.config->currentComboSkinIndex - 1].skinCount);
			if (ImGui::Button("Change Pet Skin"))
				player->changeSkin(cheatManager.database->pets[cheatManager.config->currentComboSkinIndex - 1].modelName, cheatManager.config->curretSkinId);
			footer();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Logger")) {
			cheatManager.logger->draw();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Extra")) {
			ImGui::InputText("##pushmanualmodel", &this->manualModel);
			ImGui::SameLine();
			if (ImGui::Button("Push Manual Model"))
				player->changeSkin(this->manualModel.c_str(), 1);
			ImGui::Separator();
			ImGui::InputText("Change Nick", player->getNamePtr());
			ImGui::Separator();
			if (ImGui::Button("Force Close"))
				cheatManager.hooks->uninstall();
			footer();
			ImGui::EndTabItem();
		}
	}
	ImGui::End();
}
