#include "SoundEditor.h"

#include <imgui.h>

#include "devtools/controls/SoundPicker.h"
#include "scene/Scene.h"
#include "ecs/components/SoundComponent.h"

namespace LowEngine::Editors {


    void AddSoundComponent(Scene* scene, std::size_t entityId)
    {
        scene->AddComponent<ECS::SoundComponent>(entityId);
    }

    void DrawSoundEditor(Scene* scene, std::size_t entityId)
    {
        auto sc = scene->GetComponent<ECS::SoundComponent>(entityId);
        if (sc == nullptr) return;

        bool opened = ImGui::CollapsingHeader("Sound", ImGuiTreeNodeFlags_DefaultOpen);

        if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
            ImGui::OpenPopup("SoundComponentContextMenu");
        }

        if (ImGui::BeginPopup("SoundComponentContextMenu")) {
            if (ImGui::MenuItem("Delete")) {
                if (scene->IsComponentSafeToDestroy<ECS::SoundComponent>(entityId)) {
                    scene->DestroyComponent<ECS::SoundComponent>(entityId);
                }
            }
            ImGui::EndPopup();
        }

        if (opened) {
            std::string soundAlias = Assets::GetSoundAlias(sc->SoundId);
            ImGui::Text("Sound:");
            ImGui::SameLine();
            ImGui::Text(soundAlias.c_str());

            if (ImGui::Button("Play")) {
                sc->Play();
            }
            ImGui::SameLine();
            if (ImGui::Button("Change Sound")) {
                ImGui::OpenPopup("SelectSound");
            }

            Controls::SoundPickerPopup("SelectSound", sc);
        }
    }
}