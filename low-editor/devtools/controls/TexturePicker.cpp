#include "TexturePicker.h"

#include <functional>
#include <imgui.h>

#include "ecs/Components/AnimatedSpriteComponent.h"
#include "ecs/Components/SpriteComponent.h"

namespace LowEngine::Controls {
    static void TexturePickerPopupImpl(const char *popup_str_id, std::size_t currentTextureId, bool requireSpriteSheet,
                                       const std::function<void(std::size_t)> &onSelect) {
        if (ImGui::BeginPopupModal(popup_str_id, nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Select Texture");
            ImGui::Separator();

            static char filter[128] = "";
            ImGui::InputText("Search", filter, sizeof(filter));

            ImGui::BeginChild("TextureGrid", ImVec2(500, 400), true);

            constexpr int columnsCount = 4;
            constexpr float thumbSize = 64.0f;

            ImGui::Columns(columnsCount, "TextureBrowserColumnsLayout", false);

            bool showSpriteSheetError = false;

            auto aliases = Assets::GetTextureAliases();
            for (auto &alias: aliases) {
                if (filter[0] != '\0' && alias.find(filter) == std::string::npos)
                    continue;

                std::size_t textureId = Assets::GetTextureId(alias);

                ImGui::PushID(static_cast<int>(textureId));

                bool selected = (textureId == currentTextureId);
                if (selected)
                    ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));

                auto &texture = Assets::GetTexture(textureId);
                if (ImGui::ImageButton("##tex", texture.getNativeHandle(), ImVec2(thumbSize, thumbSize))) {
                    if (requireSpriteSheet && !Assets::HasSpriteSheet(textureId)) {
                        showSpriteSheetError = true;
                    } else {
                        onSelect(textureId);
                        ImGui::CloseCurrentPopup();
                    }
                }

                if (selected)
                    ImGui::PopStyleColor();

                ImGui::Text("%s", alias.c_str());

                ImGui::PopID();

                ImGui::NextColumn();
            }

            ImGui::Columns(1);

            ImGui::EndChild();

            if (showSpriteSheetError)
                ImGui::OpenPopup("Error##NoSpriteSheet");

            if (ImGui::BeginPopupModal("Error##NoSpriteSheet", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
                ImGui::TextUnformatted("Selected texture has no sprite sheet defined!");
                if (ImGui::Button("OK"))
                    ImGui::CloseCurrentPopup();
                ImGui::EndPopup();
            }

            if (ImGui::Button("Cancel"))
                ImGui::CloseCurrentPopup();

            ImGui::EndPopup();
        }
    }

    void TexturePickerPopup(const char *popup_str_id, ECS::SpriteComponent *sc) {
        TexturePickerPopupImpl(popup_str_id, sc->TextureId, false, [sc](std::size_t id) {
            sc->SetTexture(id);
        });
    }

    void TexturePickerPopup(const char *popup_str_id, ECS::AnimatedSpriteComponent *asc) {
        TexturePickerPopupImpl(popup_str_id, asc->TextureId, true, [asc](std::size_t id) {
            asc->SetTexture(id);
        });
    }

    void TexturePickerPopup(const char *popup_str_id, std::size_t &out_textureId) {
        TexturePickerPopupImpl(popup_str_id, out_textureId, true, [&out_textureId](std::size_t id) {
            out_textureId = id;
        });
    }
}
