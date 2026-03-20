#include "AnimatedSpriteEditor.h"

#include <imgui.h>

#include "devtools/DevTools.h"
#include "devtools/controls/TexturePicker.h"
#include "ecs/components/AnimatedSpriteComponent.h"
#include "scene/Scene.h"

namespace LowEngine::Editors {
	void AddAnimatedSpriteComponent(Scene* scene, std::size_t entityId) {
		scene->AddComponent<ECS::AnimatedSpriteComponent>(entityId);
	}

	void DrawAnimatedSpriteEditor(Scene* scene, std::size_t entityId) {
		auto asc = scene->GetComponent<ECS::AnimatedSpriteComponent>(entityId);
		if (asc == nullptr) return;

		bool opened = ImGui::CollapsingHeader("Animated Sprite", ImGuiTreeNodeFlags_DefaultOpen);

		if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
			ImGui::OpenPopup("AnimatedSpriteComponentContextMenu");
		}

		if (ImGui::BeginPopup("AnimatedSpriteComponentContextMenu")) {
			scene->DestroyComponent<ECS::AnimatedSpriteComponent>(entityId);
			ImGui::EndPopup();
		}

		if (opened) {
			ImGui::Text("Draw Order:");
			ImGui::SameLine();
			int drawOrder = asc->DrawOrder;
			if (ImGui::DragInt("##DrawOrder", &drawOrder)) {
				asc->DrawOrder = drawOrder;
			}

			ImGui::Text("Texture:");
			auto& texture = Assets::GetTexture(asc->TextureId);

			float imgSize = ImGui::CalcItemWidth();
			if (ImGui::ImageButton("CurrentTexture", texture.getNativeHandle(),
			                       ImVec2(imgSize, imgSize))) {
				ImGui::OpenPopup("SelectTexture");
			}

			Controls::TexturePickerPopup("SelectTexture", asc);

			if (Assets::HasSpriteSheet(asc->TextureId)) {
				auto& spriteSheet = Assets::GetSpriteSheet(asc->TextureId);
				auto clipNames = spriteSheet.GetAnimationClipNames();
				if (clipNames.size() > 0) {
					int currentClip = -1;
					std::vector<const char*> clipNamesCStr;
					for (size_t i = 0; i < clipNames.size(); i++) {
						clipNamesCStr.push_back(clipNames[i].c_str());
						if (clipNames[i] == asc->CurrentClipName) {
							currentClip = i;
						}
					}

					ImGui::Text("Clip name: ");
					ImGui::SameLine();
					if (ImGui::Combo("##Clip name:", &currentClip, clipNamesCStr.data(), clipNamesCStr.size())) {
						asc->Play(clipNames[currentClip], asc->Loop);
					}

					ImGui::Text("Animation Preview:");
					if (spriteSheet.HasAnimationClip(asc->CurrentClipName)) {
						auto& animClip = spriteSheet.GetAnimationClip(asc->CurrentClipName);
						static float animationPreviewTimer = 0.0f;
						static float currentAnimationFrame = 0;
						animationPreviewTimer += DevTools::DeltaTime.asSeconds();
						if (animationPreviewTimer >= animClip.FrameDuration) {
							currentAnimationFrame++;
							animationPreviewTimer = 0.0f;

							if (currentAnimationFrame >= animClip.FrameCount) {
								currentAnimationFrame = 0;
							}
						}
						auto& currentFrame = animClip.Frames[currentAnimationFrame];
						static sf::Sprite animationPreviewSprite(Assets::GetDefaultTexture());
						animationPreviewSprite.setTexture(texture);
						animationPreviewSprite.setTextureRect(currentFrame);
						animationPreviewSprite.setOrigin(sf::Vector2f(static_cast<float>(spriteSheet.FrameSize.x) / 2.0f, static_cast<float>(spriteSheet.FrameSize.y) / 2.0f));

						ImGui::Image(animationPreviewSprite, sf::Vector2f(imgSize, imgSize));
					}
				} else {
					ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
					ImGui::Text("No animations defined!");
					ImGui::PopStyleColor();
				}
			} else {
				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
				ImGui::Text("Texture don't have sprite sheet!");
				ImGui::PopStyleColor();
			}
		}
	}
}
