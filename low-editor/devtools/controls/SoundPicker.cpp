#include "SoundPicker.h"

#include <imgui.h>

#include "EngineConfig.h"
#include "ecs/Components/SoundComponent.h"

#include "ImageTextButton.h"
#include "devtools/Assets.h"

namespace LowEngine::Controls
{
	void SoundPickerPopup(const char* popup_str_id, ECS::SoundComponent* sc)
	{
		if (ImGui::BeginPopupModal(popup_str_id, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			sf::Texture soundIconTexture;
			soundIconTexture.loadFromFile("assets/editor/icons/soundicon.png");

			ImGui::Text("Select Sound");
			ImGui::Separator();

			static char filter[128] = "";
			ImGui::InputText("Search", filter, sizeof(filter));

			ImGui::BeginChild("SoundGrid", ImVec2(500, 400), true);

			constexpr int columnsCount = 4;
			constexpr float thumbSize = 64.0f;

			ImGui::Columns(columnsCount, "SoundBrowserColumnsLayout", false);

			auto aliases = Assets::GetSoundAliases();
			for (auto& alias : aliases)
			{
				if (filter[0] != '\0' && alias.find(filter) == std::string::npos)
					continue;

				std::size_t soundId = Assets::GetSoundId(alias);

				ImGui::PushID(soundId);

				bool selected = (soundId == sc->SoundId);
				if (selected)
				{
					ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
				}

				if (ImGui::ImageButton("##tex", EditorAssets::SoundIconTexture()->getNativeHandle(), ImVec2(thumbSize, thumbSize)))
				{
					sc->SetSound(alias);
					ImGui::CloseCurrentPopup();
				}

				if (selected)
				{
					ImGui::PopStyleColor();
				}

				if (ImageTextButton("##playSound", EditorAssets::PlayTexture()->getNativeHandle(), ImVec2(16.0f, 16.0f),
				                    alias.c_str()))
				{
					auto& sound = Assets::GetSound(alias);
					static sf::Sound soundPreview(sound);
					soundPreview.setBuffer(sound);
					soundPreview.play();
				}

				ImGui::PopID();

				ImGui::NextColumn();
			}

			ImGui::Columns(1);

			ImGui::EndChild();

			if (ImGui::Button("Cancel"))
			{
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}

	void SoundPickerPopup(const char* popup_str_id, size_t& selectedSoundId)
	{
		if (ImGui::BeginPopupModal(popup_str_id, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			sf::Texture soundIconTexture;
			soundIconTexture.loadFromFile("assets/editor/icons/soundicon.png");

			ImGui::Text("Select Sound");
			ImGui::Separator();

			static char filter[128] = "";
			ImGui::InputText("Search", filter, sizeof(filter));

			ImGui::BeginChild("SoundGrid", ImVec2(500, 400), true);

			constexpr int columnsCount = 4;
			constexpr float thumbSize = 64.0f;

			ImGui::Columns(columnsCount, "SoundBrowserColumnsLayout", false);

			auto aliases = Assets::GetSoundAliases();
			for (auto& alias : aliases)
			{
				if (filter[0] != '\0' && alias.find(filter) == std::string::npos)
					continue;

				std::size_t soundId = Assets::GetSoundId(alias);

				ImGui::PushID(soundId);

				bool selected = (soundId == selectedSoundId);
				if (selected)
				{
					ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
				}

				if (ImGui::ImageButton("##tex", EditorAssets::SoundIconTexture()->getNativeHandle(), ImVec2(thumbSize, thumbSize)))
				{
					selectedSoundId = soundId;
					ImGui::CloseCurrentPopup();
				}

				if (selected)
				{
					ImGui::PopStyleColor();
				}

				if (ImageTextButton("##playSound", EditorAssets::PlayTexture()->getNativeHandle(), ImVec2(16.0f, 16.0f),
					alias.c_str()))
				{
					auto& sound = Assets::GetSound(alias);
					static sf::Sound soundPreview(sound);
					soundPreview.setBuffer(sound);
					soundPreview.play();
				}

				ImGui::PopID();

				ImGui::NextColumn();
			}

			ImGui::Columns(1);

			ImGui::EndChild();

			if (ImGui::Button("Cancel"))
			{
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}
}
