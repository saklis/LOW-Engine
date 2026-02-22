#include "TransformEditor.h"

#include <imgui.h>

#include "scene/Scene.h"
#include "ecs/Components/TransformComponent.h"

namespace LowEngine::Editors {
	void AddTransformComponent(Scene* scene, std::size_t entityId) {
        scene->AddComponent<ECS::TransformComponent>(entityId);
	}

	void DrawTransformEditor(Scene* scene, std::size_t entityId) {
        auto tc = scene->GetComponent<ECS::TransformComponent>(entityId);
        if (tc == nullptr) return;

        bool opened = ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen);

        if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
            ImGui::OpenPopup("TransformComponentContextMenu");
        }

        if (ImGui::BeginPopup("TransformComponentContextMenu")) {
            if (ImGui::MenuItem("Delete")) {
                if (scene->IsComponentSafeToDestroy<ECS::TransformComponent>(entityId)) {
                    scene->DestroyComponent<ECS::TransformComponent>(entityId);
                }
            }
            ImGui::EndPopup();
        }

        if (opened) {
            ImGui::Text("Position:");
            ImGui::SameLine();
            float position[2] = { tc->Position.x, tc->Position.y };
            if (ImGui::DragFloat2("##Position", position, 1.0f, 0, 0, "%.3f")) {
                tc->Position = { position[0], position[1] };
            }

            ImGui::Text("Rotation:");
            ImGui::SameLine();
            float rotation = tc->Rotation.asDegrees();
            if (ImGui::DragFloat("##Rotation", &rotation, 1.0f, 0, 0, "%.3f")) {
                tc->Rotation = sf::degrees(rotation);
            }

            ImGui::Text("Scale:");
            ImGui::SameLine();
            float scale[2] = { tc->Scale.x, tc->Scale.y };
            if (ImGui::DragFloat2("##Scale", scale, 0.1f, 0, 0, "%.3f")) {
                tc->Scale = { scale[0], scale[1] };
            }
        }
	}
}
