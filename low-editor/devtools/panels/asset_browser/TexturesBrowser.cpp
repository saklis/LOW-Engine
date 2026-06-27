#include "TexturesBrowser.h"

#include <filesystem>

#include <imgui.h>
#include <imgui-SFML.h>
#include <ImGuiFileDialog.h>

#include "Game.h"
#include "EngineConfig.h"
#include "assets/Assets.h"
#include "log/Log.h"

namespace LowEngine::Panels {

    void DisplayTextureBrowser(Game& game, ImVec2 size) {
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

}