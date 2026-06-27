#include "SpriteSheetsBrowser.h"

#include <imgui.h>
#include <imgui-SFML.h>

#include "Game.h"
#include "EngineConfig.h"
#include "assets/Assets.h"
#include "log/Log.h"

namespace LowEngine::Panels {

    void DisplaySpriteSheetBrowser(Game& game, ImVec2 size) {
        static bool showSpriteSheetEditor = false;
        static std::string selectedSpriteSheetAlias = "";
        static int spriteSheetEditorFrameCount[2] = {0, 0};

        ImGui::BeginChild("SpriteSheetBrowser", size);

        if (ImGui::Button("Create Sprite Sheet")) {
            selectedSpriteSheetAlias = "";
            showSpriteSheetEditor = true;
        }

        if (ImGui::BeginTable("SpriteSheetTable", 4,
                              ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY)) {
            ImGui::TableSetupColumn("Texture Alias", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Texture Id", ImGuiTableColumnFlags_WidthFixed, 75.0f);
            ImGui::TableSetupColumn("Frame Count (XxY px)", ImGuiTableColumnFlags_WidthFixed, 125.0f);
            ImGui::TableSetupColumn("Frame Size (XxY px)", ImGuiTableColumnFlags_WidthFixed, 125.0f);
            ImGui::TableHeadersRow();

            std::vector<std::string> textureAliases = Assets::GetTextureAliases();
            std::ranges::sort(textureAliases);

            for (const std::string& textureAlias : textureAliases) {
                if (textureAlias == Config::DEFAULT_TEXTURE_ALIAS) continue;
                size_t textureId = Assets::GetTextureId(textureAlias);

                if (!Assets::HasSpriteSheet(textureId)) continue;
                auto& spriteSheet = Assets::GetSpriteSheet(textureId);

                ImGui::TableNextRow();
                bool isSelected = selectedSpriteSheetAlias == textureAlias;

                ImGui::TableNextColumn();
                if (ImGui::Selectable(textureAlias.c_str(), isSelected, ImGuiSelectableFlags_SpanAllColumns)) {
                    selectedSpriteSheetAlias = textureAlias;
                    spriteSheetEditorFrameCount[0] = static_cast<int>(spriteSheet.FrameCount.x);
                    spriteSheetEditorFrameCount[1] = static_cast<int>(spriteSheet.FrameCount.y);
                    showSpriteSheetEditor = true;
                }

                ImGui::TableNextColumn();
                ImGui::Text("%zu", textureId);

                ImGui::TableNextColumn();
                ImGui::Text("%dx%d", spriteSheet.FrameCount.x, spriteSheet.FrameCount.y);

                ImGui::TableNextColumn();
                ImGui::Text("%dx%d", spriteSheet.FrameSize.x, spriteSheet.FrameSize.y);
            }

            ImGui::EndTable();
        }

        ImGui::EndChild();

        if (showSpriteSheetEditor) {
            ImVec2 editorSize{1200.0f, 800.0f};
            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImVec2 pos{center.x - editorSize.x / 2, center.y - editorSize.y / 2};
            ImVec2 maxPreviewSize{760.0f, 760.0f};

            ImGui::SetNextWindowPos(pos);
            ImGui::SetNextWindowSize(editorSize, ImGuiCond_Appearing);
            if (ImGui::Begin("Sprite Sheet Editor", &showSpriteSheetEditor,
                             ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize)) {
                if (selectedSpriteSheetAlias.empty()) {
                    ImGui::Dummy(maxPreviewSize);
                    ImGui::SameLine();
                    ImGui::BeginChild("SpriteSheetProperties", ImVec2(400.0f, 0.0f));
                    ImGui::Text("Sprite Sheet:");

                    auto textureAliases = Assets::GetTextureAliases();
                    std::vector<std::string> aliasesWithoutSpriteSheets;
                    std::ranges::copy_if(textureAliases, std::back_inserter(aliasesWithoutSpriteSheets),
                                         [&](const std::string& alias) {
                                             return !Assets::HasSpriteSheet(alias) && alias != Config::DEFAULT_TEXTURE_ALIAS;
                                         });
                    if (ImGui::BeginCombo("##Select New Sprite Sheet", "Select new sprite sheet...")) {
                        for (const auto& alias : aliasesWithoutSpriteSheets) {
                            if (ImGui::Selectable(alias.c_str())) {
                                Assets::AddSpriteSheet(alias, 1, 1);
                                selectedSpriteSheetAlias = alias;
                            }
                        }
                        ImGui::EndCombo();
                    }
                    ImGui::EndChild();
                } else {
                    auto& texture = Assets::GetTexture(selectedSpriteSheetAlias);
                    auto textureSize = texture.getSize();
                    auto& spriteSheet = Assets::GetSpriteSheet(selectedSpriteSheetAlias);

                    float aspectRatio = static_cast<float>(textureSize.x) / static_cast<float>(textureSize.y);
                    ImVec2 previewSize;
                    if (aspectRatio > 1.0f) {
                        previewSize = ImVec2(maxPreviewSize.x, maxPreviewSize.y / aspectRatio);
                    } else {
                        previewSize = ImVec2(maxPreviewSize.x * aspectRatio, maxPreviewSize.y);
                    }

                    ImGui::Image(texture.getNativeHandle(), previewSize);

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
                    }

                    ImGui::SameLine();

                    ImGui::BeginChild("SpriteSheetProperties", ImVec2(400.0f, 0.0f));
                    ImGui::Text("Sprite Sheet: %s", selectedSpriteSheetAlias.c_str());
                    ImGui::Separator();
                    ImGui::Text("Frame Count: ");
                    ImGui::SameLine();
                    if (ImGui::DragInt2("##FrameCount", spriteSheetEditorFrameCount, 0.1f, 1, 100)) {
                        if (spriteSheetEditorFrameCount[0] > 0 && spriteSheetEditorFrameCount[1] > 0) {
                            spriteSheet.FrameCount = {
                                static_cast<size_t>(spriteSheetEditorFrameCount[0]),
                                static_cast<size_t>(spriteSheetEditorFrameCount[1])
                            };
                            spriteSheet.FrameSize = {
                                textureSize.x / spriteSheet.FrameCount.x, textureSize.y / spriteSheet.FrameCount.y
                            };
                            _log->debug("Sprite sheet '{}' frame count updated to {}x{}", selectedSpriteSheetAlias,
                                        spriteSheet.FrameCount.x, spriteSheet.FrameCount.y);
                        }
                    }
                    ImGui::Text("Frame Size: %dx%d", spriteSheet.FrameSize.x, spriteSheet.FrameSize.y);
                    ImGui::Separator();
                    ImGui::Dummy(ImVec2(0.0f, 50.0f));

                    if (ImGui::Button("Delete", ImVec2(400.0f, 0.0f))) {
                        Assets::DeleteSpriteSheet(selectedSpriteSheetAlias);
                        _log->info("Sprite sheet '{}' deleted successfully", selectedSpriteSheetAlias);
                        showSpriteSheetEditor = false;
                    }
                    ImGui::EndChild();
                }

                ImGui::End();
            }
        }
    }

}