#include "SpriteEditor.h"

#include <imgui.h>

#include "devtools/controls/TexturePicker.h"
#include "ecs/Components/SpriteComponent.h"
#include "scene/Scene.h"

namespace LowEngine::Editors {
    void AddSpriteComponent(Scene* scene, std::size_t entityId) {
        scene->AddComponent<ECS::SpriteComponent>(entityId);
        scene->Update(0.0f);
    }

    void DrawSpriteEditor(Scene* scene, std::size_t entityId) {
        auto sc = scene->GetComponent<ECS::SpriteComponent>(entityId);
        if (sc == nullptr) return;

        bool opened = ImGui::CollapsingHeader("Sprite", ImGuiTreeNodeFlags_DefaultOpen);

        if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
            ImGui::OpenPopup("CameraComponentContextMenu");
        }

        if (ImGui::BeginPopup("CameraComponentContextMenu")) {
            scene->DestroyComponent<ECS::SpriteComponent>(entityId);
            ImGui::EndPopup();
        }

        if (opened) {
            ImGui::Text("Texture:");
            auto& texture = Assets::GetTexture(sc->TextureId);
            
            float width = ImGui::CalcItemWidth();
            if (ImGui::ImageButton("CurrentTexture", texture.getNativeHandle(),
                                   ImVec2(width, width))) {
	            ImGui::OpenPopup("SelectTexture");
            }
            
            ImGui::Text("Layer:");
            ImGui::SameLine();
            int layer = sc->Layer;
            if (ImGui::DragInt("##Layer", &layer)) {
                sc->Layer = layer;
            }
            
            Controls::TexturePickerPopup("SelectTexture", sc);
        }
    }
}
