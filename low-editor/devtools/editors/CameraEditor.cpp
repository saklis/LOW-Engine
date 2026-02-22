#include "cameraEditor.h"

#include <imgui.h>

#include "ecs/Components/CameraComponent.h"
#include "scene/Scene.h"

namespace LowEngine::Editors {
	void AddCameraComponent(Scene* scene, std::size_t entityId) {
		scene->AddComponent<ECS::CameraComponent>(entityId);
	}

	void DrawCameraEditor(Scene* scene, std::size_t entityId) {
		auto cc = scene->GetComponent<ECS::CameraComponent>(entityId);
		if (cc == nullptr) return;

		bool opened = ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen);

        if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
            ImGui::OpenPopup("CameraComponentContextMenu");
        }

        if (ImGui::BeginPopup("CameraComponentContextMenu")) {
			scene->DestroyComponent<ECS::CameraComponent>(entityId);
            ImGui::EndPopup();
        }

        if (opened) {
            ImGui::Text("Current camera:");
            ImGui::SameLine();
			bool defaultCamera = scene->IsCurrentCamera(entityId);
            if (defaultCamera) {
                ImGui::Text(" (current)");
            } else {
                if (ImGui::Button("Set as current")) {
                    scene->SetCurrentCamera(entityId);
                }
			}

            ImGui::Text("Zoom factor:");
            ImGui::SameLine();
            float zoomFactor = cc->ZoomFactor;
            if (ImGui::DragFloat("##Zoom", &zoomFactor, 0.01f, 0, 0, "%.3f")) {
                cc->ZoomFactor = zoomFactor;
            }
        }
	}
}
