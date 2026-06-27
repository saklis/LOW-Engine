#include "AnimationClipsBrowser.h"

#include <imgui.h>
#include <imgui-SFML.h>

#include "Game.h"
#include "EngineConfig.h"
#include "assets/Assets.h"
#include "log/Log.h"

namespace LowEngine::Panels {

    void DisplayAnimationClipsBrowser(Game& game, ImVec2 size) {
        static bool showAnimationClipEditor = false;
        static std::string selectedAnimationClipAlias = "";
        static std::string selectedSpriteSheetAlias = "";
        static float currentAnimationFrame = 0;

        ImGui::BeginChild("AnimationClipBrowser", size);

        if (ImGui::Button("Create Animation Clip")) {
            selectedSpriteSheetAlias.clear();
            selectedAnimationClipAlias.clear();
            currentAnimationFrame = 0;
            showAnimationClipEditor = true;
        }

        if (ImGui::BeginTable("AnimationClipsTable", 6,
                              ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY)) {
            ImGui::TableSetupColumn("Clip Name", ImGuiTableColumnFlags_WidthStretch, 100.0f);
            ImGui::TableSetupColumn("Texture Alias", ImGuiTableColumnFlags_WidthStretch, 100.0f);
            ImGui::TableSetupColumn("Texture Id", ImGuiTableColumnFlags_WidthFixed, 75.0f);
            ImGui::TableSetupColumn("Frame Count", ImGuiTableColumnFlags_WidthFixed, 125.0f);
            ImGui::TableSetupColumn("Frame Duration (s)", ImGuiTableColumnFlags_WidthFixed, 125.0f);
            ImGui::TableSetupColumn("Animation Length (s)", ImGuiTableColumnFlags_WidthFixed, 125.0f);
            ImGui::TableHeadersRow();

            std::vector<std::string> textureAliases = Assets::GetTextureAliases();
            std::ranges::sort(textureAliases);

            for (const std::string& textureAlias : textureAliases) {
                if (textureAlias == Config::DEFAULT_TEXTURE_ALIAS) continue;
                size_t textureId = Assets::GetTextureId(textureAlias);

                if (!Assets::HasSpriteSheet(textureId)) continue;
                auto& spriteSheet = Assets::GetSpriteSheet(textureId);

                auto clipNames = spriteSheet.GetAnimationClipNames();
                std::ranges::sort(clipNames);

                for (const std::string& clipName : clipNames) {
                    ImGui::TableNextRow();

                    auto& animClip = spriteSheet.GetAnimationClip(clipName);

                    ImGui::TableNextColumn();
                    bool isSelected = clipName == selectedAnimationClipAlias;
                    ImGui::PushID((textureAlias + "_" + clipName).c_str());
                    if (ImGui::Selectable(clipName.c_str(), isSelected, ImGuiSelectableFlags_SpanAllColumns)) {
                        selectedSpriteSheetAlias = textureAlias;
                        selectedAnimationClipAlias = clipName;
                        currentAnimationFrame = 0;
                        showAnimationClipEditor = true;
                    }
                    ImGui::PopID();

                    ImGui::TableNextColumn();
                    ImGui::Text("%s", textureAlias.c_str());

                    ImGui::TableNextColumn();
                    ImGui::Text("%zu", textureId);

                    ImGui::TableNextColumn();
                    ImGui::Text("%d", animClip.FrameCount);

                    ImGui::TableNextColumn();
                    ImGui::Text("%.2f", animClip.FrameDuration);

                    ImGui::TableNextColumn();
                    ImGui::Text("%.2f", animClip.FrameCount * animClip.FrameDuration);
                }
            }

            ImGui::EndTable();
        }

        ImGui::EndChild();

        if (showAnimationClipEditor) {
            ImVec2 editorSize{1200.0f, 800.0f};
            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImVec2 pos{center.x - editorSize.x / 2, center.y - editorSize.y / 2};
            ImVec2 maxSheetPreviewSize{760.0f, 760.0f};
            ImVec2 maxAnimPreviewSize{400.0f, 400.0f};
            static sf::Sprite animationPreviewSprite(Assets::GetDefaultTexture());
            static float animationPreviewTimer = 0.0f;

            ImGui::SetNextWindowPos(pos);
            ImGui::SetNextWindowSize(editorSize, ImGuiCond_Appearing);
            if (ImGui::Begin("Animation Clip Editor", &showAnimationClipEditor,
                             ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize)) {
                if (selectedAnimationClipAlias.empty()) {
                    static std::string newAnimationClipError = "";

                    std::vector<std::string> textureAliasesWithSpriteSheets;
                    auto textureAliases = Assets::GetTextureAliases();
                    for (auto& textureAlias : textureAliases) {
                        if (Assets::HasSpriteSheet(textureAlias)) {
                            textureAliasesWithSpriteSheets.push_back(textureAlias);
                        }
                    }
                    std::ranges::sort(textureAliasesWithSpriteSheets);

                    if (selectedSpriteSheetAlias.empty()) {
                        ImGui::Dummy(maxSheetPreviewSize);
                    } else {
                        auto& texture = Assets::GetTexture(selectedSpriteSheetAlias);
                        float aspectRatio = static_cast<float>(texture.getSize().x) / static_cast<float>(texture.getSize().y);
                        ImVec2 sheetPreviewSize;
                        if (aspectRatio > 1.0f) {
                            sheetPreviewSize = ImVec2(maxSheetPreviewSize.x, maxSheetPreviewSize.y / aspectRatio);
                        } else {
                            sheetPreviewSize = ImVec2(maxSheetPreviewSize.x * aspectRatio, maxSheetPreviewSize.y);
                        }
                        ImGui::Image(texture.getNativeHandle(), sheetPreviewSize);
                    }

                    ImGui::SameLine();
                    ImGui::BeginChild("AnimationClipProperties", ImVec2(400.0f, 0.0f));

                    ImGui::Text("Sprite Sheet:");
                    ImGui::SameLine();
                    const char* comboPreview = selectedSpriteSheetAlias.empty()
                                                   ? "Select sprite sheet..."
                                                   : selectedSpriteSheetAlias.c_str();
                    if (ImGui::BeginCombo("##Select Sprite Sheet", comboPreview)) {
                        for (const auto& textureAlias : textureAliasesWithSpriteSheets) {
                            if (ImGui::Selectable(textureAlias.c_str())) {
                                selectedSpriteSheetAlias = textureAlias;
                                selectedAnimationClipAlias = "";
                                showAnimationClipEditor = true;
                            }
                        }
                        ImGui::EndCombo();
                    }

                    ImGui::Text("Animation Clip Name:");
                    ImGui::SameLine();
                    static char newAnimationClipNameBuffer[255] = "";
                    ImGui::InputText("##AnimationClipName", newAnimationClipNameBuffer, sizeof(newAnimationClipNameBuffer));

                    if (ImGui::Button("Create Animation Clip")) {
                        newAnimationClipError.clear();
                        if (!selectedSpriteSheetAlias.empty()) {
                            if (std::strlen(newAnimationClipNameBuffer) > 0) {
                                auto& spriteSheet = Assets::GetSpriteSheet(selectedSpriteSheetAlias);
                                if (!spriteSheet.HasAnimationClip(std::string(newAnimationClipNameBuffer))) {
                                    selectedAnimationClipAlias = newAnimationClipNameBuffer;
                                    spriteSheet.AddAnimationClip(selectedAnimationClipAlias, 0, 1, 1.0f,
                                                                 sf::Vector2<size_t>(0, 0));
                                    _log->info("Animation clip '{}' created successfully", newAnimationClipNameBuffer);
                                } else {
                                    newAnimationClipError = "Animation clip with this name already exists";
                                }
                            } else {
                                newAnimationClipError = "Animation clip name cannot be empty";
                            }
                        } else {
                            newAnimationClipError = "Select sprite sheet";
                        }
                    }

                    if (!newAnimationClipError.empty()) {
                        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), newAnimationClipError.c_str());
                    }

                    ImGui::EndChild();
                } else {
                    auto& spriteSheet = Assets::GetSpriteSheet(selectedSpriteSheetAlias);
                    auto& texture = Assets::GetTexture(spriteSheet.TextureId);
                    auto& animClip = spriteSheet.GetAnimationClip(selectedAnimationClipAlias);

                    animationPreviewTimer += game.DeltaTime.asSeconds();
                    if (animationPreviewTimer >= animClip.FrameDuration) {
                        currentAnimationFrame++;
                        animationPreviewTimer = 0.0f;
                        if (currentAnimationFrame >= animClip.FrameCount) {
                            currentAnimationFrame = 0;
                        }
                    }

                    if (currentAnimationFrame >= animClip.Frames.size()) {
                        currentAnimationFrame = 0;
                    }

                    auto& currentFrame = animClip.Frames[currentAnimationFrame];

                    animationPreviewSprite.setTexture(texture);
                    animationPreviewSprite.setTextureRect(currentFrame);
                    animationPreviewSprite.setOrigin(sf::Vector2f(static_cast<float>(spriteSheet.FrameSize.x) / 2.0f,
                                                                  static_cast<float>(spriteSheet.FrameSize.y) / 2.0f));

                    float aspectRatio = static_cast<float>(texture.getSize().x) / static_cast<float>(texture.getSize().y);
                    ImVec2 sheetPreviewSize;
                    if (aspectRatio > 1.0f) {
                        sheetPreviewSize = ImVec2(maxSheetPreviewSize.x, maxSheetPreviewSize.y / aspectRatio);
                    } else {
                        sheetPreviewSize = ImVec2(maxSheetPreviewSize.x * aspectRatio, maxSheetPreviewSize.y);
                    }

                    ImGui::Image(texture.getNativeHandle(), sheetPreviewSize);

                    ImVec2 imagePos = ImGui::GetItemRectMin();
                    ImVec2 imageSize = ImGui::GetItemRectSize();

                    if (spriteSheet.FrameCount.x > 0 && spriteSheet.FrameCount.y > 0) {
                        float cellWidth = imageSize.x / spriteSheet.FrameCount.x;
                        float cellHeight = imageSize.y / spriteSheet.FrameCount.y;
                        ImDrawList* drawList = ImGui::GetWindowDrawList();

                        for (size_t i = 1; i < spriteSheet.FrameCount.x; ++i) {
                            float x = imagePos.x + i * cellWidth;
                            drawList->AddLine(ImVec2(x, imagePos.y), ImVec2(x, imagePos.y + imageSize.y),
                                              IM_COL32(0, 255, 255, 255), 1.0f);
                        }
                        for (size_t j = 1; j < spriteSheet.FrameCount.y; ++j) {
                            float y = imagePos.y + j * cellHeight;
                            drawList->AddLine(ImVec2(imagePos.x, y), ImVec2(imagePos.x + imageSize.x, y),
                                              IM_COL32(0, 255, 255, 255), 1.0f);
                        }

                        for (size_t i = 0; i < animClip.FrameCount; ++i) {
                            auto frame = animClip.Frames[i];
                            ImVec2 p1 = {imagePos.x + (frame.position.x / frame.size.x) * cellWidth,
                                         imagePos.y + (frame.position.y / frame.size.y) * cellHeight};
                            ImVec2 p2 = {imagePos.x + (frame.position.x / frame.size.x) * cellWidth,
                                         imagePos.y + ((frame.position.y / frame.size.y) + 1) * cellHeight};
                            ImVec2 p3 = {imagePos.x + ((frame.position.x / frame.size.x) + 1) * cellWidth,
                                         imagePos.y + ((frame.position.y / frame.size.y) + 1) * cellHeight};
                            ImVec2 p4 = {imagePos.x + ((frame.position.x / frame.size.x) + 1) * cellWidth,
                                         imagePos.y + (frame.position.y / frame.size.y) * cellHeight};
                            drawList->AddLine(p1, p2, IM_COL32(255, 0, 0, 255), 1.0f);
                            drawList->AddLine(p2, p3, IM_COL32(255, 0, 0, 255), 1.0f);
                            drawList->AddLine(p3, p4, IM_COL32(255, 0, 0, 255), 1.0f);
                            drawList->AddLine(p4, p1, IM_COL32(255, 0, 0, 255), 1.0f);
                        }
                    }

                    ImGui::SameLine();

                    ImGui::BeginChild("AnimationClipProperties", ImVec2(400.0f, 0.0f));
                    ImGui::Text("Sprite Sheet: %s", selectedSpriteSheetAlias.c_str());
                    ImGui::Text("Clip Name: %s", selectedAnimationClipAlias.c_str());
                    ImGui::Separator();
                    ImGui::Text("First Frame Coordinates:");
                    ImGui::SameLine();
                    int animClipEditorFirstFrameX = animClip.Frames[0].position.x / spriteSheet.FrameSize.x;
                    int animClipEditorFirstFrameY = animClip.Frames[0].position.y / spriteSheet.FrameSize.y;
                    ImGui::SetNextItemWidth(100.0f);
                    if (ImGui::DragInt("##FirstFrameX", &animClipEditorFirstFrameX, 0.1f, 0,
                                       spriteSheet.FrameCount.x - 1)) {
                        animClip.Frames[0].position.x = animClipEditorFirstFrameX * spriteSheet.FrameSize.x;
                        animClip.RecalculateFrames(spriteSheet);
                    }
                    ImGui::SameLine();
                    ImGui::Text("x");
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(100.0f);
                    if (ImGui::DragInt("##FirstFrameY", &animClipEditorFirstFrameY, 0.1f, 0,
                                       spriteSheet.FrameCount.y - 1)) {
                        animClip.Frames[0].position.y = animClipEditorFirstFrameY * spriteSheet.FrameSize.y;
                        animClip.RecalculateFrames(spriteSheet);
                    }
                    int clipFrameCount = static_cast<int>(animClip.FrameCount);
                    int maxAllowedFrameCount = spriteSheet.FrameCount.x - (animClip.StartFrame % spriteSheet.FrameCount.x);
                    ImGui::Text("Frame Count:");
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(100.0f);
                    if (ImGui::DragInt("##FrameCount", &clipFrameCount, 0.1f, 1, maxAllowedFrameCount)) {
                        animClip.FrameCount = static_cast<size_t>(clipFrameCount);
                        animClip.RecalculateFrames(spriteSheet);
                    }
                    float clipFrameDuration = animClip.FrameDuration;
                    ImGui::Text("Frame Duration:");
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(100.0f);
                    if (ImGui::DragFloat("##FrameDuration", &clipFrameDuration, 0.001f, 0.001f, 100.0f)) {
                        animClip.FrameDuration = clipFrameDuration;
                    }
                    ImGui::SameLine();
                    ImGui::Text("seconds");
                    ImGui::Text("Animation Total Length: %.3f seconds", animClip.FrameCount * animClip.FrameDuration);
                    ImGui::Separator();
                    ImGui::Dummy(ImVec2(0.0f, 50.0f));
                    ImGui::Text("Animation preview:");
                    ImGui::Separator();
                    ImGui::Image(animationPreviewSprite, sf::Vector2f(maxAnimPreviewSize.x, maxAnimPreviewSize.y));
                    ImGui::Separator();
                    ImGui::Dummy(ImVec2(0.0f, 50.0f));
                    if (ImGui::Button("Delete", ImVec2(400.0f, 0.0f))) {
                        spriteSheet.RemoveAnimationClip(selectedAnimationClipAlias);
                        _log->info("Animation clip '{}' deleted successfully", selectedAnimationClipAlias);
                        selectedSpriteSheetAlias.clear();
                        selectedAnimationClipAlias.clear();
                        showAnimationClipEditor = false;
                    }
                    ImGui::EndChild();
                }

                ImGui::End();
            }
        }
    }

}