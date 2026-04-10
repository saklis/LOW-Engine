#include "AssetBrowserPanel.h"

#include <filesystem>

#include <imgui.h>
#include <imgui-SFML.h>
#include <ImGuiFileDialog.h>

#include "Game.h"
#include "EngineConfig.h"
#include "assets/Assets.h"
#include "assets/terrain/Layer.h"
#include "devtools/Assets.h"
#include "devtools/FormatHelpers.h"
#include "log/Log.h"

namespace LowEngine::Panels {

    // -------------------------------------------------------------------------
    // Textures
    // -------------------------------------------------------------------------

    static void DisplayTextureBrowser(Game& game, ImVec2 size) {
        int columnsCount = 8;
        int thumbnailSize = 124;

        static std::string selectedTextureAlias;
        static sf::Vector2u selectedTextureSize;
        static bool showPreviewWindow = false;

        static std::filesystem::path newTextureFile;
        static bool showAddNewTextureDialog = false;

        ImGui::BeginChild("TextureBrowser", size);

        if (ImGui::Button("Add Texture")) {
            IGFD::FileDialogConfig config;
            config.path = ".";
            config.fileName = "";
            ImGuiFileDialog::Instance()->OpenDialog("SelectTextureToAdd", "Select texture to add...",
                                                    ".png", config);
        }
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        auto addTextureSize = ImVec2(800, 600);
        ImGui::SetNextWindowPos(ImVec2(center.x - addTextureSize.x / 2, center.y - addTextureSize.y / 2),
                                ImGuiCond_Appearing);
        ImGui::SetNextWindowSize(addTextureSize, ImGuiCond_Appearing);
        if (ImGuiFileDialog::Instance()->Display("SelectTextureToAdd")) {
            if (ImGuiFileDialog::Instance()->IsOk()) {
                newTextureFile = std::filesystem::path(ImGuiFileDialog::Instance()->GetCurrentPath()) /
                                 std::filesystem::path(ImGuiFileDialog::Instance()->GetCurrentFileName());
                showAddNewTextureDialog = true;
            }
            ImGuiFileDialog::Instance()->Close();
        }

        auto aliases = Assets::GetTextureAliases();
        std::ranges::sort(aliases);

        ImGui::Columns(columnsCount, "TextureBrowserColumnsLayout", false);

        for (auto alias : aliases) {
            auto& texture = Assets::GetTexture(alias);

            if (ImGui::ImageButton(alias.c_str(), texture.getNativeHandle(), ImVec2(thumbnailSize, thumbnailSize),
                                   ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f))) {
                selectedTextureAlias = alias;
                selectedTextureSize = texture.getSize();
                showPreviewWindow = true;
            }

            ImGui::TextWrapped("%s", alias.c_str());
            ImGui::NextColumn();
        }

        ImGui::Columns(1);
        ImGui::EndChild();

        if (showPreviewWindow) {
            ImGui::SetNextWindowSize(ImVec2(530, 595), ImGuiCond_Appearing);
            if (ImGui::Begin("Texture Preview", &showPreviewWindow,
                             ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize)) {
                auto& texture = Assets::GetTexture(selectedTextureAlias);

                float maxPreviewSize = 512.0f;
                float aspectRatio = static_cast<float>(selectedTextureSize.x) / static_cast<float>(selectedTextureSize.y);
                ImVec2 previewSize;

                if (aspectRatio > 1.0f) {
                    previewSize = ImVec2(maxPreviewSize, maxPreviewSize / aspectRatio);
                } else {
                    previewSize = ImVec2(maxPreviewSize * aspectRatio, maxPreviewSize);
                }

                ImGui::Text("Preview: %s", selectedTextureAlias.c_str());
                ImGui::Separator();
                ImGui::Image(texture.getNativeHandle(), previewSize);

                if (selectedTextureAlias != Config::DEFAULT_TEXTURE_ALIAS) {
                    if (ImGui::Button("Delete", ImVec2(512.0f, 0.0f))) {
                        std::filesystem::remove(texture.Path);
                        Assets::UnloadTexture(selectedTextureAlias);
                        showPreviewWindow = false;
                    }
                }

                ImGui::End();
            }
        }

        if (showAddNewTextureDialog) {
            ImVec2 addNewTextureDialogCenter = ImGui::GetMainViewport()->GetCenter();
            auto addTextureDialogSize = ImVec2(750, 250);
            ImGui::SetNextWindowPos(ImVec2(addNewTextureDialogCenter.x - addTextureSize.x / 2,
                                           addNewTextureDialogCenter.y - addTextureSize.y / 2),
                                    ImGuiCond_Appearing);
            ImGui::SetNextWindowSize(ImVec2(addTextureDialogSize.x, addTextureDialogSize.y), ImGuiCond_Appearing);
            if (ImGui::Begin("Add New Texture", &showAddNewTextureDialog,
                             ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize)) {
                ImGui::Text("Selected file: %s", newTextureFile.string().c_str());

                static char aliasBuffer[255] = "";
                ImGui::Text("Alias:");
                ImGui::SameLine();
                ImGui::InputText("##Alias", aliasBuffer, sizeof(aliasBuffer));

                if (std::strlen(aliasBuffer) == 0) {
                    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Alias cannot be empty!");
                } else if (Assets::TextureExists(aliasBuffer)) {
                    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Texture with this alias already exists!");
                } else {
                    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Alias is valid.");
                }

                if (ImGui::Button("Add Texture")) {
                    std::filesystem::path targetPath = std::filesystem::path(game.ProjectDirectory) /
                                                       std::filesystem::path(Config::ASSETS_FOLDER_NAME) /
                                                       std::filesystem::path(Config::TEXTURES_FOLDER_NAME) /
                                                       newTextureFile.filename();
                    std::filesystem::create_directories(targetPath.parent_path());
                    if (std::filesystem::exists(targetPath)) {
                        std::filesystem::remove(targetPath);
                        _log->warn("File '{}' already exists. It will be overwritten.", targetPath.string());
                    }
                    std::filesystem::copy_file(newTextureFile, targetPath);
                    _log->info("Texture '{}' copied to: {}", newTextureFile.filename().string(), targetPath.string());

                    Assets::LoadTexture(aliasBuffer, targetPath.string());
                    _log->info("Texture '{}' added successfully from file: {}", aliasBuffer, newTextureFile.string());

                    showAddNewTextureDialog = false;
                    ImGui::CloseCurrentPopup();
                }

                if (ImGui::Button("Cancel")) {
                    showAddNewTextureDialog = false;
                    ImGui::CloseCurrentPopup();
                }

                ImGui::End();
            }
        }
    }

    // -------------------------------------------------------------------------
    // Sprite Sheets
    // -------------------------------------------------------------------------

    static void DisplaySpriteSheetBrowser(Game& game, ImVec2 size) {
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

    // -------------------------------------------------------------------------
    // Animation Clips
    // -------------------------------------------------------------------------

    static void DisplayAnimationClipsBrowser(Game& game, ImVec2 size) {
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

    // -------------------------------------------------------------------------
    // Tile Maps
    // -------------------------------------------------------------------------

    static void ShowTileMapLayerEditor(const Game& game, Terrain::Layer& layer) {
        static size_t selectedCellDefinitionIndex = 0;
        static std::string selectedCellDefinitionNewAnimationName = "";

        ImGui::Text("Texture:");
        ImGui::SameLine();

        auto currentTerrainTextureAlias = Assets::GetTextureAlias(layer.TextureId);

        std::vector<std::string> aliasesWithSheets;
        auto textureAliases = Assets::GetTextureAliases();
        std::ranges::sort(textureAliases);
        for (auto& alias : textureAliases) {
            if (Assets::HasSpriteSheet(alias)) {
                aliasesWithSheets.emplace_back(alias);
            }
        }

        if (ImGui::BeginCombo("##TerrainTexture", currentTerrainTextureAlias.c_str())) {
            for (const auto& alias : aliasesWithSheets) {
                bool isSelected = alias == currentTerrainTextureAlias;
                if (ImGui::Selectable(alias.c_str(), isSelected)) {
                    size_t newTerrainTextureId = Assets::GetTextureId(alias);
                    layer.LoadTexture(newTerrainTextureId);
                    layer.GenerateCellDefinitionsFromTexture();
                }
            }
            ImGui::EndCombo();
        }

        auto& texture = Assets::GetTexture(layer.TextureId);
        ImVec2 texturePreviewMaxSize{350.0f, 100.0f};
        float scale = std::min(texturePreviewMaxSize.x / static_cast<float>(texture.getSize().x),
                               texturePreviewMaxSize.y / static_cast<float>(texture.getSize().y));
        ImVec2 texturePreviewSize = ImVec2(static_cast<float>(texture.getSize().x) * scale,
                                           static_cast<float>(texture.getSize().y) * scale);

        size_t definitionsCount = layer.Definition.CellDefinitions.size();
        float definitionHeight = definitionsCount > 0
                                     ? texturePreviewSize.y / static_cast<float>(definitionsCount)
                                     : 0.0f;

        int maxIndexForWidth = definitionsCount > 0 ? static_cast<int>(definitionsCount - 1) : 0;
        char maxIndexBuf[32];
        std::snprintf(maxIndexBuf, sizeof(maxIndexBuf), "%d", maxIndexForWidth);
        float labelGap = 6.0f;

        ImGui::Indent();
        ImGui::Image(texture.getNativeHandle(), texturePreviewSize);

        if (definitionsCount > 0) {
            ImVec2 imgPos = ImGui::GetItemRectMin();
            ImVec2 imgSize = ImGui::GetItemRectSize();
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            ImU32 textCol = ImGui::GetColorU32(ImGuiCol_Text);

            for (size_t i = 0; i < definitionsCount; i++) {
                std::string label = std::to_string(i);
                ImVec2 textSize = ImGui::CalcTextSize(label.c_str());
                float y = imgPos.y + static_cast<float>(i) * definitionHeight + (definitionHeight - textSize.y) * 0.5f;
                float x = imgPos.x - labelGap - textSize.x;

                y = std::max(y, imgPos.y);
                y = std::min(y, imgPos.y + imgSize.y - textSize.y);

                drawList->AddText(ImVec2(x, y), textCol, label.c_str());
            }
        }

        ImGui::Unindent();
        ImGui::Separator();

        ImGui::Text("Cell Definitions:");
        ImGui::BeginChild("TerrainCellDefinitionsTableContainer", ImVec2(0.0f, 200.0f), true);
        if (ImGui::BeginTable("TerrainCellDefinitionsTable", 6,
                              ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY)) {
            ImGui::TableSetupColumn("Index", ImGuiTableColumnFlags_WidthFixed, 65.0f);
            ImGui::TableSetupColumn("Move cost", ImGuiTableColumnFlags_WidthFixed, 75.0f);
            ImGui::TableSetupColumn("Walk", ImGuiTableColumnFlags_WidthFixed, 35.0f);
            ImGui::TableSetupColumn("Swim", ImGuiTableColumnFlags_WidthFixed, 35.0f);
            ImGui::TableSetupColumn("Fly", ImGuiTableColumnFlags_WidthFixed, 35.0f);
            ImGui::TableSetupColumn("Animations", ImGuiTableColumnFlags_WidthFixed, 75.0f);
            ImGui::TableHeadersRow();

            for (size_t index = 0; index < layer.Definition.CellDefinitions.size(); index++) {
                auto& definition = layer.Definition.CellDefinitions[index];
                ImGui::TableNextRow();

                ImGui::TableNextColumn();
                bool isSelected = index == selectedCellDefinitionIndex;
                if (ImGui::Selectable(std::to_string(index).c_str(), &isSelected)) {
                    selectedCellDefinitionIndex = index;
                }

                ImGui::TableNextColumn();
                ImGui::InputFloat(std::format("##MoveCost_{}", index).c_str(), &definition.MoveCost);

                ImGui::TableNextColumn();
                ImGui::Checkbox(std::format("##Walkable_{}", index).c_str(), &definition.IsWalkable);

                ImGui::TableNextColumn();
                ImGui::Checkbox(std::format("##Swimmable_{}", index).c_str(), &definition.IsSwimmable);

                ImGui::TableNextColumn();
                ImGui::Checkbox(std::format("##Flyable_{}", index).c_str(), &definition.IsFlyable);

                ImGui::TableNextColumn();
                ImGui::Text("%d", definition.AnimationClipNames.size());
            }

            ImGui::EndTable();
        }
        ImGui::EndChild();

        ImGui::Text("Animations:");
        ImGui::Separator();
        if (selectedCellDefinitionIndex < layer.Definition.CellDefinitions.size()) {
            auto& clipNames = layer.Definition.CellDefinitions[selectedCellDefinitionIndex].AnimationClipNames;
            if (clipNames.empty()) {
                ImGui::Text("[none]");
            } else {
                for (const auto& clipName : clipNames) {
                    ImGui::PushID(("-_" + clipName).c_str());
                    if (ImGui::Button("-", ImVec2(20.0f, 20.0f))) {
                        clipNames.erase(std::ranges::find(clipNames, clipName));
                        std::ranges::sort(clipNames);
                    }
                    ImGui::PopID();
                    ImGui::SameLine();
                    ImGui::Text("%s", clipName.c_str());
                }
            }
            ImGui::Separator();

            auto animationClipNames = Assets::GetSpriteSheet(layer.TextureId).GetAnimationClipNames();
            std::vector<std::string> availableClipNames;
            for (const auto& clipName : animationClipNames) {
                if (std::ranges::none_of(clipNames, [&](const auto& name) { return name == clipName; })) {
                    availableClipNames.emplace_back(clipName);
                }
            }

            ImGui::Text("Add animation:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(230.0f);
            const char* previewText = selectedCellDefinitionNewAnimationName.empty()
                                          ? "Select animation to add..."
                                          : selectedCellDefinitionNewAnimationName.c_str();
            if (ImGui::BeginCombo("##AddAnimationToCellDefinition", previewText)) {
                for (const auto& clipName : availableClipNames) {
                    bool isSelected = std::ranges::any_of(clipNames, [&](const auto& name) {
                        return name == clipName;
                    });
                    if (ImGui::Selectable(clipName.c_str(), isSelected)) {
                        selectedCellDefinitionNewAnimationName = clipName;
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::SameLine();
            if (ImGui::Button("Add")) {
                if (!selectedCellDefinitionNewAnimationName.empty()) {
                    layer.Definition.CellDefinitions[selectedCellDefinitionIndex].AnimationClipNames.emplace_back(
                        selectedCellDefinitionNewAnimationName);
                    std::ranges::sort(layer.Definition.CellDefinitions[selectedCellDefinitionIndex].AnimationClipNames);
                    selectedCellDefinitionNewAnimationName.clear();
                }
            }
            ImGui::Separator();
        }
    }

    static void ShowTileMapEditor(const Game& game, std::string& tileMapAlias,
                                  const std::filesystem::path& newTileMapFile, bool& showEditor) {
        static sf::RenderTexture tileMapPreview;

        ImVec2 editorSize{1200.0f, 800.0f};
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImVec2 pos{center.x - editorSize.x / 2, center.y - editorSize.y / 2};
        ImVec2 tileMapPreviewMaxSize{760.0f, 760.0f};

        ImGui::SetNextWindowPos(pos);
        ImGui::SetNextWindowSize(editorSize, ImGuiCond_Appearing);
        if (ImGui::Begin("Tile Map Layers Editor", &showEditor,
                         ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize)) {
            if (tileMapAlias.empty()) {
                ImGui::Dummy(tileMapPreviewMaxSize);
                ImGui::SameLine();
                ImGui::BeginChild("New Tile Map Properties", ImVec2(400.0f, 0.0f));

                ImGui::Text("Selected file: %s", newTileMapFile.string().c_str());
                ImGui::Separator();
                ImGui::Text("Alias:");
                ImGui::SameLine();
                static char newTileMapAliasBuffer[255] = "";
                ImGui::InputText("##TileMapAlias", newTileMapAliasBuffer, sizeof(newTileMapAliasBuffer));

                if (ImGui::Button("Load Tile Map")) {
                    Assets::LoadTileMap(newTileMapFile.generic_string(), std::vector<Terrain::LayerDefinition>());
                    tileMapAlias = newTileMapAliasBuffer;
                }

                ImGui::EndChild();
            } else {
                auto& tileMap = Assets::GetTileMap(tileMapAlias);

                if (!tileMapPreview.resize({static_cast<unsigned>(tileMap.Size.x), static_cast<unsigned>(tileMap.Size.y)})) {
                    _log->error("Failed to resize map render texture to {}x{}.", tileMap.Size.x, tileMap.Size.y);
                }
                tileMapPreview.clear(sf::Color::Magenta);

                static bool showTerrainPreview = true;
                if (showTerrainPreview) {
                    auto terrain = tileMap.TerrainLayer.GetDrawable();
                    if (terrain) { tileMapPreview.draw(*terrain); }
                }

                static bool showFeaturesPreview = true;
                if (showFeaturesPreview) {
                    auto features = tileMap.FeaturesLayer.GetDrawable();
                    if (features) { tileMapPreview.draw(*features); }
                }

                tileMapPreview.display();

                float mapAspectRatio = tileMap.Size.x / tileMap.Size.y;
                ImVec2 previewSize;
                if (mapAspectRatio > 1.0f) {
                    previewSize = ImVec2(tileMapPreviewMaxSize.x, tileMapPreviewMaxSize.y / mapAspectRatio);
                } else {
                    previewSize = ImVec2(tileMapPreviewMaxSize.x * mapAspectRatio, tileMapPreviewMaxSize.y);
                }

                ImVec2 uv0(0, 1);
                ImVec2 uv1(1, 0);
                ImGui::Image(tileMapPreview.getTexture().getNativeHandle(), previewSize, uv0, uv1);

                ImGui::SameLine();
                ImGui::BeginChild("Tile Map Properties", ImVec2(400.0f, 0.0f));

                ImGui::Text("Map name: %s", tileMap.Name.c_str());
                ImGui::Text("Alias: %s", tileMapAlias.c_str());
                ImGui::Separator();
                ImGui::Text("Map size:  %dx%d pixels", tileMap.Size.x, tileMap.Size.y);
                ImGui::Text("           %dx%d cells", tileMap.TerrainLayer.CellCount.x, tileMap.TerrainLayer.CellCount.y);
                ImGui::Text("Cell size: %d pixels", tileMap.TerrainLayer.CellSize);
                ImGui::Separator();

                ImGui::Dummy(ImVec2(0.0f, 25.0f));
                ImGui::Text("Layers:");
                ImGui::Separator();

                if (ImGui::BeginTabBar("LayersTabBar")) {
                    if (ImGui::BeginTabItem("Terrain")) {
                        ShowTileMapLayerEditor(game, tileMap.TerrainLayer);
                        ImGui::EndTabItem();
                    }
                    if (ImGui::BeginTabItem("Features")) {
                        ShowTileMapLayerEditor(game, tileMap.FeaturesLayer);
                        ImGui::EndTabItem();
                    }
                    ImGui::EndTabBar();
                }

                ImGui::Dummy(ImVec2(0.0f, 25.0f));
                ImGui::Text("Preview settings:");
                ImGui::Separator();
                ImGui::Text("Show layers:");
                ImGui::Checkbox("Terrain", &showTerrainPreview);
                ImGui::Checkbox("Features", &showFeaturesPreview);

                ImGui::EndChild();
            }

            ImGui::End();
        }
    }

    static void DisplayTileMapBrowser(const Game& game, ImVec2 size) {
        static bool showTileMapEditor = false;
        static std::string selectedTileMapAlias = "";
        static std::filesystem::path newTileMapFile;

        ImGui::BeginChild("TileMapBrowser", size);

        if (ImGui::Button("Add Tile Map")) {
            IGFD::FileDialogConfig config;
            config.path = ".";
            config.fileName = "";
            ImGuiFileDialog::Instance()->OpenDialog("SelectTileMapToAdd", "Select LDtk tile map to add...",
                                                    ".ldtkl", config);
        }
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        auto addTextureSize = ImVec2(800, 600);
        ImGui::SetNextWindowPos(ImVec2(center.x - addTextureSize.x / 2, center.y - addTextureSize.y / 2),
                                ImGuiCond_Appearing);
        ImGui::SetNextWindowSize(addTextureSize, ImGuiCond_Appearing);
        if (ImGuiFileDialog::Instance()->Display("SelectTileMapToAdd")) {
            if (ImGuiFileDialog::Instance()->IsOk()) {
                selectedTileMapAlias.clear();
                newTileMapFile = std::filesystem::path(ImGuiFileDialog::Instance()->GetCurrentPath()) /
                                 std::filesystem::path(ImGuiFileDialog::Instance()->GetCurrentFileName());
                showTileMapEditor = true;
            }
            ImGuiFileDialog::Instance()->Close();
        }

        if (ImGui::BeginTable("TileMapsTable", 7,
                              ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY)) {
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch, 100.0f);
            ImGui::TableSetupColumn("Alias", ImGuiTableColumnFlags_WidthStretch, 100.0f);
            ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthStretch, 100.0f);
            ImGui::TableSetupColumn("Grid size", ImGuiTableColumnFlags_WidthFixed, 100.0f);
            ImGui::TableSetupColumn("Terrain layer", ImGuiTableColumnFlags_WidthFixed, 125.0f);
            ImGui::TableSetupColumn("Features layer", ImGuiTableColumnFlags_WidthFixed, 125.0f);
            ImGui::TableSetupColumn("NavGrid", ImGuiTableColumnFlags_WidthFixed, 125.0f);
            ImGui::TableHeadersRow();

            auto tileMapAliases = Assets::GetTileMapAliases();
            std::ranges::sort(tileMapAliases);

            for (const std::string& tileMapAlias : tileMapAliases) {
                ImGui::TableNextRow();
                auto& tileMap = Assets::GetTileMap(tileMapAlias);

                bool isSelected = false;
                ImGui::TableNextColumn();
                if (ImGui::Selectable(tileMap.Name.c_str(), &isSelected, ImGuiSelectableFlags_SpanAllColumns)) {
                    selectedTileMapAlias = tileMapAlias;
                    showTileMapEditor = true;
                }

                ImGui::TableNextColumn();
                ImGui::Text("%s", tileMapAlias.c_str());

                ImGui::TableNextColumn();
                ImGui::Text("%dx%d pixels", tileMap.Size.x, tileMap.Size.y);

                ImGui::TableNextColumn();
                bool hasTerrainLayer = tileMap.TerrainLayer.CellSize > 0;
                if (hasTerrainLayer) {
                    ImGui::Text("%dx%d cells", tileMap.TerrainLayer.CellCount.x, tileMap.TerrainLayer.CellCount.y);
                } else {
                    ImGui::Text("[no terrain layer]");
                }

                ImGui::TableNextColumn();
                ImGui::Checkbox("##TerrainLayer", &hasTerrainLayer);

                ImGui::TableNextColumn();
                bool hasFeaturesLayer = tileMap.FeaturesLayer.CellSize > 0;
                ImGui::Checkbox("##FeaturesLayer", &hasFeaturesLayer);

                ImGui::TableNextColumn();
                bool hasNavGrid = !tileMap.NavGrid.Cells.empty();
                ImGui::Checkbox("##NavigationData", &hasNavGrid);
            }

            ImGui::EndTable();
        }

        ImGui::EndChild();

        if (showTileMapEditor) {
            ShowTileMapEditor(game, selectedTileMapAlias, newTileMapFile, showTileMapEditor);
        }
    }

    // -------------------------------------------------------------------------
    // Sounds
    // -------------------------------------------------------------------------

    static void DisplaySoundBrowser(const Game& game, ImVec2 size) {
        int columnsCount = 8;
        int thumbnailSize = 100;

        static bool showAddNewSoundDialog = false;
        static std::filesystem::path newSoundFile;
        static bool showSoundPreviewWindow = false;
        static std::string selectedSoundAlias;

        ImGui::BeginChild("SoundBrowser", size);

        if (ImGui::Button("Add Sound")) {
            IGFD::FileDialogConfig config;
            config.path = ".";
            config.fileName = "";
            ImGuiFileDialog::Instance()->OpenDialog("SelectSoundToAdd", "Select sound to add...", ".wav", config);
        }

        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        auto addSoundSize = ImVec2(800, 600);
        ImGui::SetNextWindowPos(ImVec2(center.x - addSoundSize.x / 2, center.y - addSoundSize.y / 2),
                                ImGuiCond_Appearing);
        ImGui::SetNextWindowSize(addSoundSize, ImGuiCond_Appearing);
        if (ImGuiFileDialog::Instance()->Display("SelectSoundToAdd")) {
            if (ImGuiFileDialog::Instance()->IsOk()) {
                newSoundFile = std::filesystem::path(ImGuiFileDialog::Instance()->GetCurrentPath()) /
                               std::filesystem::path(ImGuiFileDialog::Instance()->GetCurrentFileName());
                showAddNewSoundDialog = true;
            }
            ImGuiFileDialog::Instance()->Close();
        }

        auto aliases = Assets::GetSoundAliases();
        std::ranges::sort(aliases);

        ImGui::Columns(columnsCount, "SoundBrowserColumnsLayout", false);

        for (auto& alias : aliases) {
            if (ImGui::ImageButton(alias.c_str(), EditorAssets::SoundIconTexture()->getNativeHandle(),
                                   ImVec2(thumbnailSize, thumbnailSize), ImVec2(0.0f, 0.0f),
                                   ImVec2(1.0f, 1.0f))) {
                selectedSoundAlias = alias;
                showSoundPreviewWindow = true;
            }
            ImGui::TextWrapped("%s", alias.c_str());
            ImGui::NextColumn();
        }

        ImGui::Columns(1);
        ImGui::EndChild();

        if (showSoundPreviewWindow) {
            ImGui::SetNextWindowSize(ImVec2(530, 590), ImGuiCond_Appearing);
            if (ImGui::Begin("Sound Preview", &showSoundPreviewWindow,
                             ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize)) {
                auto& sound = Assets::GetSound(selectedSoundAlias);

                ImGui::Text("Preview: %s", selectedSoundAlias.c_str());
                ImGui::Separator();
                if (ImGui::ImageButton(selectedSoundAlias.c_str(), EditorAssets::SoundIconTexture()->getNativeHandle(),
                                       ImVec2(504.0f, 504.0f))) {
                    static sf::Sound soundPreview(sound);
                    soundPreview.setBuffer(sound);
                    soundPreview.play();
                }

                if (selectedSoundAlias != Config::DEFAULT_SOUND_ALIAS) {
                    if (ImGui::Button("Delete", ImVec2(512.0f, 0.0f))) {
                        std::filesystem::remove(sound.Path);
                        Assets::UnloadSound(selectedSoundAlias);
                        showSoundPreviewWindow = false;
                    }
                }

                ImGui::End();
            }
        }

        if (showAddNewSoundDialog) {
            ImVec2 addNewSoundDialogCenter = ImGui::GetMainViewport()->GetCenter();
            auto addSoundDialogSize = ImVec2(750, 250);
            ImGui::SetNextWindowPos(ImVec2(addNewSoundDialogCenter.x - addSoundSize.x / 2,
                                           addNewSoundDialogCenter.y - addSoundSize.y / 2),
                                    ImGuiCond_Appearing);
            ImGui::SetNextWindowSize(ImVec2(addSoundDialogSize.x, addSoundDialogSize.y), ImGuiCond_Appearing);
            if (ImGui::Begin("Add New Sound", &showAddNewSoundDialog,
                             ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize)) {
                ImGui::Text("Selected file: %s", newSoundFile.string().c_str());

                static char aliasBuffer[255] = "";
                ImGui::Text("Alias:");
                ImGui::SameLine();
                ImGui::InputText("##Alias", aliasBuffer, sizeof(aliasBuffer));

                if (std::strlen(aliasBuffer) == 0) {
                    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Alias cannot be empty!");
                } else if (Assets::SoundExists(aliasBuffer)) {
                    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Sound with this alias already exists!");
                } else {
                    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Alias is valid.");
                }

                if (ImGui::Button("Add Sound")) {
                    std::filesystem::path targetPath = std::filesystem::path(game.ProjectDirectory) /
                                                       std::filesystem::path(Config::ASSETS_FOLDER_NAME) /
                                                       std::filesystem::path(Config::SOUNDS_FOLDER_NAME) /
                                                       newSoundFile.filename();
                    std::filesystem::create_directories(targetPath.parent_path());
                    if (std::filesystem::exists(targetPath)) {
                        std::filesystem::remove(targetPath);
                        _log->warn("File '{}' already exists. It will be overwritten.", targetPath.string());
                    }
                    std::filesystem::copy_file(newSoundFile, targetPath);
                    _log->info("Sound '{}' copied to: {}", newSoundFile.filename().string(), targetPath.string());

                    Assets::LoadSound(aliasBuffer, targetPath.string());
                    _log->info("Sound '{}' added successfully from file: {}", aliasBuffer, newSoundFile.string());

                    showAddNewSoundDialog = false;
                    ImGui::CloseCurrentPopup();
                }

                if (ImGui::Button("Cancel")) {
                    showAddNewSoundDialog = false;
                    ImGui::CloseCurrentPopup();
                }

                ImGui::End();
            }
        }
    }

    // -------------------------------------------------------------------------
    // Music
    // -------------------------------------------------------------------------

    static void DisplayMusicBrowser(Game& game, ImVec2 size) {
        int columnsCount = 6;
        int thumbnailSize = 100;

        static bool showAddNewMusicDialog = false;
        static std::filesystem::path newMusicFile;

        ImGui::BeginChild("MusicBrowser", size);

        if (ImGui::Button("Add Music")) {
            IGFD::FileDialogConfig config;
            config.path = ".";
            config.fileName = "";
            ImGuiFileDialog::Instance()->OpenDialog("SelectMusicToAdd", "Select music to add...", ".mp3, .wav", config);
        }

        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        auto addMusicSize = ImVec2(800, 600);
        ImGui::SetNextWindowPos(ImVec2(center.x - addMusicSize.x / 2, center.y - addMusicSize.y / 2),
                                ImGuiCond_Appearing);
        ImGui::SetNextWindowSize(addMusicSize, ImGuiCond_Appearing);
        if (ImGuiFileDialog::Instance()->Display("SelectMusicToAdd")) {
            if (ImGuiFileDialog::Instance()->IsOk()) {
                newMusicFile = std::filesystem::path(ImGuiFileDialog::Instance()->GetCurrentPath()) /
                               std::filesystem::path(ImGuiFileDialog::Instance()->GetCurrentFileName());
                showAddNewMusicDialog = true;
            }
            ImGuiFileDialog::Instance()->Close();
        }

        auto aliases = Assets::GetMusicAliases();
        std::ranges::sort(aliases);

        if (ImGui::BeginTable("MusicBrowserLayoutTable", 2,
                              ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_ScrollX,
                              ImVec2(1150.0f, 0.0f))) {
            ImGui::TableSetupColumn("MusicBrowserColumn",
                                    ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_NoResize, 800.0f);
            ImGui::TableSetupColumn("MusicPlayerColumn",
                                    ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 400.0f);

            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            ImGui::BeginChild("MusicBrowserPanel");
            ImGui::Columns(columnsCount, "MusicBrowserColumnsLayout", false);

            for (auto alias : aliases) {
                if (ImGui::ImageButton(alias.c_str(), EditorAssets::MusicIconTexture()->getNativeHandle(),
                                       ImVec2(thumbnailSize, thumbnailSize), ImVec2(0.0f, 0.0f),
                                       ImVec2(1.0f, 1.0f))) {
                    game.Music.AddMusicToQueue(alias);
                    if (!game.Music.IsMusicPlaying()) {
                        game.Music.PlayNextQueued();
                    }
                }
                ImGui::TextWrapped("%s", alias.c_str());
                ImGui::NextColumn();
            }

            ImGui::Columns(1);
            ImGui::EndChild();

            ImGui::TableNextColumn();
            ImGui::BeginChild("MusicPlayerPanel");
            if (game.Music.IsMusicPlaying() || game.Music.IsMusicPaused()) {
                auto alias = game.Music.GetCurrentMusicAlias();
                auto& music = Assets::GetMusic(alias);
                auto queue = game.Music.GetQueuedMusic();

                ImGui::Text("Currently playing: %s", alias.c_str());
                ImGui::Separator();
                ImGui::Image(EditorAssets::MusicIconTexture()->getNativeHandle(), ImVec2(200.0f, 200.0f));
                ImGui::Separator();
                bool isPaused = music.getStatus() == sf::Music::Status::Paused;
                bool isLooping = music.isLooping();

                if (!isPaused) ImGui::BeginDisabled();
                if (ImGui::ImageButton("playButton", EditorAssets::PlayTexture()->getNativeHandle(),
                                       ImVec2(40.0f, 40.0f))) {
                    music.play();
                }
                if (!isPaused) ImGui::EndDisabled();
                ImGui::SameLine();
                if (isPaused) ImGui::BeginDisabled();
                if (ImGui::ImageButton("pauseButton", EditorAssets::PauseTexture()->getNativeHandle(),
                                       ImVec2(40.0f, 40.0f))) {
                    music.pause();
                }
                if (isPaused) ImGui::EndDisabled();
                ImGui::SameLine();
                if (ImGui::ImageButton("stopButton", EditorAssets::StopTexture()->getNativeHandle(),
                                       ImVec2(40.0f, 40.0f))) {
                    music.stop();
                    game.Music.ClearQueuedMusic();
                }
                ImGui::SameLine();
                if (queue.empty()) ImGui::BeginDisabled();
                if (ImGui::ImageButton("nextButton", EditorAssets::NextTexture()->getNativeHandle(),
                                       ImVec2(40.0f, 40.0f))) {
                    game.Music.PlayNextQueued();
                }
                if (queue.empty()) ImGui::EndDisabled();
                ImGui::SameLine();
                ImVec4 tint = isLooping
                                  ? ImVec4(1.0f, 0.3f, 0.3f, 1.0f)
                                  : ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
                if (ImGui::ImageButton("loopButton", EditorAssets::LoopTexture()->getNativeHandle(),
                                       ImVec2(40.0f, 40.0f), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f),
                                       ImVec4(0.0f, 0.0f, 0.0f, 0.0f), tint)) {
                    music.setLooping(!isLooping);
                }
                float currentSeek = music.getPlayingOffset().asSeconds();
                ImGui::SetNextItemWidth(400.0f);
                if (ImGui::SliderFloat("##MusicSeekerSlider", &currentSeek, 0, music.getDuration().asSeconds())) {
                    music.setPlayingOffset(sf::seconds(currentSeek));
                }
                ImGui::Text("Time: %s / %s", FormatHelpers::ToString(music.getPlayingOffset()).c_str(),
                            FormatHelpers::ToString(music.getDuration()).c_str());
                ImGui::Separator();
                ImGui::Text("Queue:");

                for (size_t index = 0; index < queue.size(); index++) {
                    auto& queueAlias = queue[index];
                    ImGui::PushID(("remove_" + queueAlias + std::to_string(index)).c_str());
                    if (ImGui::Button("-", ImVec2(20.0f, 20.0f))) {
                        game.Music.RemoveMusicFromQueue(index);
                    }
                    ImGui::PopID();
                    ImGui::SameLine();
                    ImGui::PushID(("select_" + queueAlias + std::to_string(index)).c_str());
                    bool isSelected = queueAlias == game.Music.GetCurrentMusicAlias();
                    if (ImGui::Selectable(queueAlias.c_str(), isSelected)) {
                        game.Music.PlayMusic(queueAlias);
                    }
                    ImGui::PopID();
                }
            } else {
                ImGui::Text("No music is currently playing.");
            }
            ImGui::EndChild();

            ImGui::EndTable();
        }

        ImGui::EndChild();

        if (showAddNewMusicDialog) {
            ImVec2 addNewMusicDialogCenter = ImGui::GetMainViewport()->GetCenter();
            auto addMusicDialogSize = ImVec2(750, 250);
            ImGui::SetNextWindowPos(ImVec2(addNewMusicDialogCenter.x - addMusicSize.x / 2,
                                           addNewMusicDialogCenter.y - addMusicSize.y / 2),
                                    ImGuiCond_Appearing);
            ImGui::SetNextWindowSize(ImVec2(addMusicDialogSize.x, addMusicDialogSize.y), ImGuiCond_Appearing);
            if (ImGui::Begin("Add New Musi", &showAddNewMusicDialog,
                             ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize)) {
                ImGui::Text("Selected file: %s", newMusicFile.string().c_str());

                static char aliasBuffer[255] = "";
                ImGui::Text("Alias:");
                ImGui::SameLine();
                ImGui::InputText("##Alias", aliasBuffer, sizeof(aliasBuffer));

                if (std::strlen(aliasBuffer) == 0) {
                    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Alias cannot be empty!");
                } else if (Assets::MusicExists(aliasBuffer)) {
                    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Music with this alias already exists!");
                } else {
                    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Alias is valid.");
                }

                if (ImGui::Button("Add Music")) {
                    std::filesystem::path targetPath = std::filesystem::path(game.ProjectDirectory) /
                                                       std::filesystem::path(Config::ASSETS_FOLDER_NAME) /
                                                       std::filesystem::path(Config::MUSIC_FOLDER_NAME) /
                                                       newMusicFile.filename();
                    std::filesystem::create_directories(targetPath.parent_path());
                    if (std::filesystem::exists(targetPath)) {
                        std::filesystem::remove(targetPath);
                        _log->warn("File '{}' already exists. It will be overwritten.", targetPath.string());
                    }
                    std::filesystem::copy_file(newMusicFile, targetPath);
                    _log->info("Music '{}' copied to: {}", newMusicFile.filename().string(), targetPath.string());

                    Assets::LoadMusic(aliasBuffer, targetPath.string());
                    _log->info("Music '{}' added successfully from file: {}", aliasBuffer, newMusicFile.string());

                    showAddNewMusicDialog = false;
                    ImGui::CloseCurrentPopup();
                }

                if (ImGui::Button("Cancel")) {
                    showAddNewMusicDialog = false;
                    ImGui::CloseCurrentPopup();
                }

                ImGui::End();
            }
        }
    }

    // -------------------------------------------------------------------------
    // Asset Browser (public entry point)
    // -------------------------------------------------------------------------

    void DrawAssetBrowser(Game& game, bool& isVisible) {
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        auto size = ImVec2(1224, 768);

        ImGui::SetNextWindowPos(ImVec2(center.x - size.x / 2, center.y - size.y / 2));
        ImGui::SetNextWindowSize(size);

        ImGui::OpenPopup("Asset Browser");
        if (ImGui::BeginPopupModal("Asset Browser", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            if (ImGui::BeginTabBar("AssetBrowserTabs")) {
                if (ImGui::BeginTabItem("Textures")) {
                    DisplayTextureBrowser(game, ImVec2(0.0f, 680.0f));
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Sprite sheets")) {
                    DisplaySpriteSheetBrowser(game, ImVec2(0.0f, 680.0f));
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Animation clips")) {
                    DisplayAnimationClipsBrowser(game, ImVec2(0.0f, 680.0f));
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Tile maps")) {
                    DisplayTileMapBrowser(game, ImVec2(0.0f, 680.0f));
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Sounds")) {
                    DisplaySoundBrowser(game, ImVec2(0.0f, 680.0f));
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Music")) {
                    DisplayMusicBrowser(game, ImVec2(0.0f, 680.0f));
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }

            ImGui::Separator();

            if (ImGui::Button("Close")) {
                isVisible = false;
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }

}