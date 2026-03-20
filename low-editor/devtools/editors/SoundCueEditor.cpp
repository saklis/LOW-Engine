#include "SoundCueEditor.h"

#include <limits>

#include <imgui.h>

#include "devtools/Assets.h"
#include "devtools/controls/SoundPicker.h"
#include "devtools/controls/ImageTextButton.h"

#include "scene/Scene.h"
#include "ecs/components/SoundCueComponent.h"

namespace LowEngine::Editors {
	void AddSoundCueComponent(Scene* scene, std::size_t entityId)
	{
		scene->AddComponent<ECS::SoundCueComponent>(entityId);
	}

	void DrawSoundCueEditor(Scene* scene, std::size_t entityId)
	{
        auto scc = scene->GetComponent<ECS::SoundCueComponent>(entityId);
        if (scc == nullptr) return;

        bool opened = ImGui::CollapsingHeader("Sound Cue", ImGuiTreeNodeFlags_DefaultOpen);

        if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
            ImGui::OpenPopup("SoundCueComponentContextMenu");
        }

        if (ImGui::BeginPopup("SoundCueComponentContextMenu")) {
            if (ImGui::MenuItem("Delete")) {
                if (scene->IsComponentSafeToDestroy<ECS::SoundCueComponent>(entityId)) {
                    scene->DestroyComponent<ECS::SoundCueComponent>(entityId);
                }
            }
            ImGui::EndPopup();
        }

        if (opened) {
            constexpr size_t invalidSoundId = std::numeric_limits<size_t>::max();
            static size_t selectedSoundId = invalidSoundId;

            if (ImGui::Button("Add Sound")) {
                selectedSoundId = invalidSoundId;
                ImGui::OpenPopup("AddSoundCuePopup");
            }

            Controls::SoundPickerPopup("AddSoundCuePopup", selectedSoundId);

            if (selectedSoundId != invalidSoundId) {
                if (scc->GetSoundCues().find(selectedSoundId) == scc->GetSoundCues().end()) {
                    scc->AddSound(selectedSoundId);
                }
                selectedSoundId = invalidSoundId;
            }

            bool removeSound = false;
            size_t removeSoundId = 0;

            if (ImGui::BeginTable("SoundCueTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
                ImGui::TableSetupColumn("##Play");
                ImGui::TableSetupColumn("ID");
                ImGui::TableSetupColumn("Alias");
                ImGui::TableSetupColumn("##Actions");
                ImGui::TableHeadersRow();

                for (const auto& soundId : scc->GetSoundCues() | std::views::keys) {
                    ImGui::TableNextRow();

                    ImGui::TableSetColumnIndex(0);
                    ImGui::PushID(static_cast<int>(soundId));
                    if (Controls::ImageTextButton("##playSoundCue", EditorAssets::PlayTexture()->getNativeHandle(), ImVec2(12.0f, 12.0f), "Play")) {
                        scc->Play(soundId);
                    }
                    ImGui::PopID();

                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("%zu", soundId);

                    ImGui::TableSetColumnIndex(2);
                    std::string alias = Assets::GetSoundAlias(soundId);
                    ImGui::Text("%s", alias.c_str());

                    ImGui::TableSetColumnIndex(3);
                    ImGui::PushID(static_cast<int>(soundId));
                    if (ImGui::SmallButton("Delete")) {
                        removeSound = true;
                        removeSoundId = soundId;
                    }
                    ImGui::PopID();
                }

                ImGui::EndTable();
            }

            if (removeSound) {
                scc->RemoveSound(removeSoundId);
            }
        }
	}
}
