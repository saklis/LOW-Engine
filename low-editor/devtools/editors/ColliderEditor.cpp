#include "ColliderEditor.h"

#include <imgui.h>

#include "scene/Scene.h"
#include "ecs/components/ColliderComponent.h"

namespace LowEngine::Editors {
	void AddColliderComponent(Scene* scene, std::size_t entityId) {
		scene->AddComponent<ECS::ColliderComponent>(entityId);
	}

	void DrawColliderEditor(Scene* scene, std::size_t entityId) {
        auto cc = scene->GetComponent<ECS::ColliderComponent>(entityId);
        if (cc == nullptr) return;

        bool opened = ImGui::CollapsingHeader("Collider", ImGuiTreeNodeFlags_DefaultOpen);

        if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
            ImGui::OpenPopup("ColliderComponentContextMenu");
        }

        if (ImGui::BeginPopup("ColliderComponentContextMenu")) {
            if (ImGui::MenuItem("Delete")) {
                if (scene->IsComponentSafeToDestroy<ECS::ColliderComponent>(entityId)) {
                    scene->DestroyComponent<ECS::ColliderComponent>(entityId);
                }
            }
            ImGui::EndPopup();
        }

        if (opened) {
            ImGui::Text("Type: ");
            ImGui::SameLine();
			const char* typeStr = "";
            switch (cc->Type) {
            case ECS::ColliderType::Static:
                typeStr = "Static";
                break;
            case ECS::ColliderType::Dynamic:
                typeStr = "Dynamic";
                break;
            case ECS::ColliderType::Kinematic:
                typeStr = "Kinematic";
                break;
            }
            if (ImGui::BeginCombo("##ColliderType", typeStr)) {
                if (ImGui::Selectable("Static", cc->Type == ECS::ColliderType::Static)) {
                    cc->Type = ECS::ColliderType::Static;
                }
                if (ImGui::Selectable("Dynamic", cc->Type == ECS::ColliderType::Dynamic)) {
                    cc->Type = ECS::ColliderType::Dynamic;
                }
                if (ImGui::Selectable("Kinematic", cc->Type == ECS::ColliderType::Kinematic)) {
                    cc->Type = ECS::ColliderType::Kinematic;
                }
                ImGui::EndCombo();
            }

            if (cc->HasBody()) {
                switch (cc->Type) {
                case ECS::ColliderType::Static:
	                break;
                case ECS::ColliderType::Dynamic:
	                break;
                case ECS::ColliderType::Kinematic:
                    b2Vec2 currentPos = b2Body_GetPosition(cc->GetBodyId());
                    b2Rot currentRot = b2Body_GetRotation(cc->GetBodyId());

                    ImGui::Text("Position: (%.2f, %.2f)", currentPos.x, currentPos.y);
                    ImGui::Text("Rotation: %.2f degrees", b2Rot_GetAngle(currentRot) * 180.0f / 3.14159f);

                    b2Polygon poly = b2Shape_GetPolygon(cc->GetShapeId());
                    float halfW = 0.0f;
                    float halfH = 0.0f;
                    for (int i = 0; i < poly.count; ++i) {
                        float ax = poly.vertices[i].x >= 0.0f ? poly.vertices[i].x : -poly.vertices[i].x;
                        float ay = poly.vertices[i].y >= 0.0f ? poly.vertices[i].y : -poly.vertices[i].y;
                        if (ax > halfW) halfW = ax;
                        if (ay > halfH) halfH = ay;
                    }
                    float size[2] = { halfW * 2.0f, halfH * 2.0f };

					static float currentSize = size[0]; // Assuming it's a box collider, width and height should be the same
                    ImGui::Text("Size: ");
					ImGui::SameLine();
                    ImGui::InputFloat("##PhysBodySize", &currentSize);
                    if (ImGui::IsItemDeactivatedAfterEdit()) {
                        constexpr float minSize = 0.001f;
                        if (currentSize >= minSize) {
                            b2Polygon newPoly = b2MakeBox(currentSize * 0.5f, currentSize * 0.5f);
                            b2Shape_SetPolygon(cc->GetShapeId(), &newPoly);
                        }
                    }
	                break;
                }
            } else {
	            if (ImGui::Button("Create physics body")) {
					cc->CreateBoxCollider(0.5f, 0.5f);
	            }
            }

            ImGui::Text("Draw collider overly: ");
            ImGui::SameLine();
			ImGui::Checkbox("##DrawColliderOverlay", &cc->DrawCollisionOverlay);
        }
	}
}
