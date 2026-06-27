#include "MusicBrowser.h"

#include <filesystem>

#include <imgui.h>
#include <imgui-SFML.h>
#include <ImGuiFileDialog.h>

#include "Game.h"
#include "EngineConfig.h"
#include "assets/Assets.h"
#include "devtools/Assets.h"
#include "devtools/FormatHelpers.h"
#include "log/Log.h"

namespace LowEngine::Panels {

    void DisplayMusicBrowser(Game& game, ImVec2 size) {
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
            if (ImGui::Begin("Add New Music", &showAddNewMusicDialog,
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

}