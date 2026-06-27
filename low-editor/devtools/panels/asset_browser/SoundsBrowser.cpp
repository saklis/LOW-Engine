#include "SoundsBrowser.h"

#include <filesystem>

#include <imgui.h>
#include <imgui-SFML.h>
#include <ImGuiFileDialog.h>

#include "Game.h"
#include "EngineConfig.h"
#include "assets/Assets.h"
#include "devtools/Assets.h"
#include "log/Log.h"

namespace LowEngine::Panels {

    void DisplaySoundBrowser(const Game& game, ImVec2 size) {
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

}