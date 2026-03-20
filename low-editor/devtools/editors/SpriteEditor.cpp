#include "SpriteEditor.h"

#include <imgui.h>

#include "devtools/controls/TexturePicker.h"
#include "ecs/components/SpriteComponent.h"
#include "scene/Scene.h"

namespace LowEngine::Editors {
    void AddSpriteComponent(Scene* scene, std::size_t entityId) {
        scene->AddComponent<ECS::SpriteComponent>(entityId);
    }

    void DrawSpriteEditor(Scene* scene, std::size_t entityId) {
        auto sc = scene->GetComponent<ECS::SpriteComponent>(entityId);
        if (sc == nullptr) return;

        bool opened = ImGui::CollapsingHeader("Sprite", ImGuiTreeNodeFlags_DefaultOpen);

        if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
            ImGui::OpenPopup("SpriteComponentContextMenu");
        }

        if (ImGui::BeginPopup("SpriteComponentContextMenu")) {
            scene->DestroyComponent<ECS::SpriteComponent>(entityId);
            ImGui::EndPopup();
        }

        if (opened) {
            ImGui::Text("Texture:");
            auto& texture = Assets::GetTexture(sc->TextureId);

            ImGui::Text("Draw Order:");
            ImGui::SameLine();
            int drawOrder = sc->DrawOrder;
            if (ImGui::DragInt("##DrawOrder", &drawOrder)) {
                sc->DrawOrder = drawOrder;
            }
            
            float width = ImGui::CalcItemWidth();
            if (ImGui::ImageButton("CurrentTexture", texture.getNativeHandle(),
                                   ImVec2(width, width))) {
	            ImGui::OpenPopup("SelectTexture");
            }
            
            Controls::TexturePickerPopup("SelectTexture", sc);
        }
    }
}
