#include "TexturePicker.h"

#include <imgui.h>

#include "ecs/Components/SpriteComponent.h"

namespace LowEngine::Controls {
	void TexturePickerPopup(const char* popup_str_id, LowEngine::ECS::SpriteComponent* sc) {
        if (ImGui::BeginPopupModal("SelectTexture", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Select Texture");
            ImGui::Separator();

            static char filter[128] = "";
            ImGui::InputText("Search", filter, sizeof(filter));

            ImGui::BeginChild("TextureGrid", ImVec2(500, 400), true);

            constexpr int columnsCount = 4;
            constexpr float thumbSize = 64.0f;

            ImGui::Columns(columnsCount, "TextureBrowserColumnsLayout", false);

            auto aliases = Assets::GetTextureAliases();
            for (auto& alias : aliases) {
                if (filter[0] != '\0' && alias.find(filter) == std::string::npos)
                    continue;

                std::size_t textureId = Assets::GetTextureId(alias);

                ImGui::PushID(textureId);

                bool selected = (textureId == sc->TextureId);
                if (selected)
                    ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));

                auto& texture = Assets::GetTexture(textureId);
                if (ImGui::ImageButton("##tex", texture.getNativeHandle(), ImVec2(thumbSize, thumbSize))) {
                    sc->SetTexture(textureId);
                    ImGui::CloseCurrentPopup();
                }

                if (selected)
                    ImGui::PopStyleColor();

                ImGui::Text("%s", alias.c_str());

                ImGui::PopID();

                ImGui::NextColumn();
            }

            ImGui::Columns(1);

            ImGui::EndChild();

            if (ImGui::Button("Cancel")) {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
	}
}
