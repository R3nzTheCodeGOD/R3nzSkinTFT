#include <cstdint>
#include <string>
#include <mutex>
#include <vector>
#include <variant>
#include <tuple>

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
	ImGui::textUnformattedCentered("Copyright (C) 2022-2023 R3nzTheCodeGOD");
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
		const auto& pet{ cheatManager.database->pets[cheatManager.config->currentComboSkinIndex - 1] };
		std::int32_t count{ 0 };
		if (std::holds_alternative<std::int32_t>(pet.skinIds))
			count = std::get<std::int32_t>(pet.skinIds);
		else if (std::holds_alternative<std::pair<std::int32_t, std::int32_t>>(pet.skinIds))
			count = std::get<std::pair<std::int32_t, std::int32_t>>(pet.skinIds).second;

		if (cheatManager.config->currentComboSkinIndex > 0 && cheatManager.config->curretSkinId > count)
			cheatManager.config->curretSkinId = 1;

		if (cheatManager.config->currentComboSkinIndex > 0)
			player->changeSkin(cheatManager.database->pets[cheatManager.config->currentComboSkinIndex - 1].modelName, cheatManager.config->curretSkinId);
	});

	ImGui::Begin("R3nzSkin TFT", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_AlwaysAutoResize);
	if (ImGui::BeginTabBar("TabBar", ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_FittingPolicyScroll | ImGuiTabBarFlags_NoTooltip)) {
		if (ImGui::BeginTabItem("Skin Changer")) {
			if (ImGui::Combo("Current Pet", &cheatManager.config->currentComboSkinIndex, vectorGetter, static_cast<void*>(&cheatManager.database->pets), cheatManager.database->pets.size() + 1)) {
				const auto& pet{ cheatManager.database->pets[cheatManager.config->currentComboSkinIndex - 1] };
				if (std::holds_alternative<std::int32_t>(pet.skinIds))
					cheatManager.config->curretSkinId = 1;
				else if (std::holds_alternative<std::pair<std::int32_t, std::int32_t>>(pet.skinIds))
					cheatManager.config->curretSkinId = std::get<std::pair<std::int32_t, std::int32_t>>(pet.skinIds).first;
			}

			const auto& pet{ cheatManager.database->pets[cheatManager.config->currentComboSkinIndex - 1] };
			if (std::holds_alternative<std::int32_t>(pet.skinIds)) {
				ImGui::SliderInt("Current Pet SkinId", &cheatManager.config->curretSkinId, 1, std::get<std::int32_t>(pet.skinIds));
			} else if (std::holds_alternative<std::pair<std::int32_t, std::int32_t>>(pet.skinIds)) {
				const auto pair{ std::get<std::pair<std::int32_t, std::int32_t>>(pet.skinIds) };
				ImGui::SliderInt("Current Pet SkinId", &cheatManager.config->curretSkinId, pair.first, pair.second);
			}

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
			ImGui::InputText("##push_model", &this->model);
			ImGui::SameLine();
			if (ImGui::Button("Push Model"))
				player->changeSkin(this->model.c_str(), 1);
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
