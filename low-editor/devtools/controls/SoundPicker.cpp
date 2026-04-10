#include "SoundPicker.h"

#include <functional>
#include <imgui.h>

#include "ecs/Components/SoundComponent.h"

#include "ImageTextButton.h"
#include "devtools/Assets.h"

namespace LowEngine::Controls {
    static void SoundPickerPopupImpl(const char *popup_str_id, std::size_t currentSoundId,
                                     const std::function<void(const std::string&, std::size_t)> &onSelect) {
        if (ImGui::BeginPopupModal(popup_str_id, nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Select Sound");
            ImGui::Separator();

            static char filter[128] = "";
            ImGui::InputText("Search", filter, sizeof(filter));

            ImGui::BeginChild("SoundGrid", ImVec2(500, 400), true);

            constexpr int columnsCount = 4;
            constexpr float thumbSize = 64.0f;

            ImGui::Columns(columnsCount, "SoundBrowserColumnsLayout", false);

            auto aliases = Assets::GetSoundAliases();
            for (auto &alias: aliases) {
                if (filter[0] != '\0' && alias.find(filter) == std::string::npos)
                    continue;

                std::size_t soundId = Assets::GetSoundId(alias);

                ImGui::PushID(static_cast<int>(soundId));

                bool selected = (soundId == currentSoundId);
                if (selected)
                    ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));

                if (ImGui::ImageButton("##tex", EditorAssets::SoundIconTexture()->getNativeHandle(),
                                       ImVec2(thumbSize, thumbSize))) {
                    onSelect(alias, soundId);
                    ImGui::CloseCurrentPopup();
                }

                if (selected)
                    ImGui::PopStyleColor();

                if (ImageTextButton("##playSound", EditorAssets::PlayTexture()->getNativeHandle(), ImVec2(16.0f, 16.0f),
                                    alias.c_str())) {
                    auto &sound = Assets::GetSound(alias);
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
                ImGui::CloseCurrentPopup();

            ImGui::EndPopup();
        }
    }

    void SoundPickerPopup(const char *popup_str_id, ECS::SoundComponent *sc) {
        SoundPickerPopupImpl(popup_str_id, sc->SoundId, [sc](const std::string &alias, std::size_t) {
            sc->SetSound(alias);
        });
    }

    void SoundPickerPopup(const char *popup_str_id, size_t &selectedSoundId) {
        SoundPickerPopupImpl(popup_str_id, selectedSoundId, [&selectedSoundId](const std::string&, std::size_t id) {
            selectedSoundId = id;
        });
    }
}