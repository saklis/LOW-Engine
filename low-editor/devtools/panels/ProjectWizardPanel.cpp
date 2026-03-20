#include "ProjectWizardPanel.h"

#include <filesystem>
#include <fstream>

#include <imgui.h>
#include <ImGuiFileDialog.h>
#include <nlohmann/json.hpp>

#include "Game.h"
#include "EngineConfig.h"
#include "EditorConfig.h"
#include "devtools/Assets.h"
#include "log/Log.h"

namespace LowEngine::Panels {

    static void InitStaticCharArray(char* arr, size_t length, const std::string& value) {
        if (std::strlen(arr) == 0) {
            std::strncpy(arr, value.c_str(), length);
            arr[length - 1] = '\0';
        }
    }

    static void CreateNewProject(Game& game, const std::string& projectName, const std::string& projectPath,
                                 bool& isVisible) {
        if (!projectName.empty() && !projectPath.empty()) {
            _log->info("Creating new project: {} at {}", projectName, projectPath);

            auto projectDir = std::filesystem::path(projectPath) / std::filesystem::path(projectName);
            std::filesystem::create_directories(projectDir);

            auto projectFilePath = std::filesystem::path(projectDir) / std::filesystem::path(
                                       projectName + Config::PROJECT_FILE_EXTENSION);

            std::ofstream projectFile(projectFilePath.string());
            if (!projectFile.is_open()) {
                _log->error("Failed to create project file: {}", projectFilePath.string());
                ImGui::CloseCurrentPopup();
                return;
            }

            nlohmann::ordered_json projectJson;
            projectJson["title"] = projectName;

            projectFile << projectJson.dump(4);
            projectFile.close();
            if (projectFile.fail()) {
                _log->error("Failed to write project data to file: {}", projectFilePath.string());
                ImGui::CloseCurrentPopup();
                return;
            }

            _log->info("New project created: {}", projectFilePath.string());

            game.CloseProject();
            game.LoadProject(projectFilePath.string());
            game.SaveProject(projectFilePath.string());

            game.Scenes.GetCurrentScene()->IsPaused = true;
            game.Window.setTitle("LOWEditor: " + game.Title);

            isVisible = false;
            ImGui::CloseCurrentPopup();
        } else {
            _log->error("Failed to create project: Project name and path shouldn't be empty.");
        }
    }

    void DrawProjectWizard(Game& game, bool& isVisible) {
        static char projectName[150] = "";
        InitStaticCharArray(projectName, 150, LowEditor::Config::DEFAULT_NEW_PROJECT_NAME);
        static char projectPath[256] = "";
        InitStaticCharArray(projectPath, 256,
                            std::filesystem::path(
                                std::filesystem::current_path() / std::filesystem::path(
                                    LowEditor::Config::PROJECT_DIRECTORY)).string());

        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        auto size = ImVec2(1024, 768);

        ImGui::SetNextWindowPos(ImVec2(center.x - size.x / 2, center.y - size.y / 2));
        ImGui::SetNextWindowSize(size);

        ImGui::OpenPopup("LOWEngine Project Wizard");
        if (ImGui::BeginPopupModal("LOWEngine Project Wizard", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Create a new project");
            ImGui::Separator();

            ImGui::Text("Project Directory:");
            ImGui::SameLine();

            ImGui::SetNextItemWidth(400);
            ImGui::InputText("##ProjectPath", projectPath, sizeof(projectPath));
            ImGui::SameLine();
            if (ImGui::Button("Browse")) {
                IGFD::FileDialogConfig config;
                config.path = ".";
                config.fileName = projectName;
                ImGuiFileDialog::Instance()->OpenDialog("ChooseProjectPath", "Choose Project Path",
                                                        Config::PROJECT_FILE_EXTENSION.c_str(), config);
            }

            center = ImGui::GetMainViewport()->GetCenter();
            size = ImVec2(800, 600);
            ImGui::SetNextWindowPos(ImVec2(center.x - size.x / 2, center.y - size.y / 2), ImGuiCond_Appearing);
            ImGui::SetNextWindowSize(size, ImGuiCond_Appearing);
            if (ImGuiFileDialog::Instance()->Display("ChooseProjectPath")) {
                if (ImGuiFileDialog::Instance()->IsOk()) {
                    std::string currentPath = ImGuiFileDialog::Instance()->GetCurrentPath();
                    strncpy(projectPath, currentPath.c_str(), sizeof(projectPath) - 1);
                    projectPath[sizeof(projectPath) - 1] = '\0';

                    std::string currentFileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
                    currentFileName.erase(currentFileName.size() - Config::PROJECT_FILE_EXTENSION.size());
                    strncpy(projectName, currentFileName.c_str(), sizeof(projectName) - 1);
                    projectName[sizeof(projectName) - 1] = '\0';
                }
                ImGuiFileDialog::Instance()->Close();
            }

            ImGui::Text("Project Name:");
            ImGui::SameLine();

            ImGui::SetNextItemWidth(400);
            ImGui::InputText("##ProjectName", projectName, sizeof(projectName));

            bool projectNameExists = false;
            auto theoreticalProjectFile = std::filesystem::path(projectPath) / std::filesystem::path(projectName)
                                          / std::filesystem::path(projectName).concat(Config::PROJECT_FILE_EXTENSION);

            if (std::filesystem::exists(theoreticalProjectFile)) {
                projectNameExists = true;
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Project with this name already exists!");
            } else {
                projectNameExists = false;
                ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Project name is available.");
            }

            if (projectNameExists) ImGui::BeginDisabled();
            if (ImGui::Button("Create new project")) {
                CreateNewProject(game, projectName, projectPath, isVisible);
            }
            if (projectNameExists) ImGui::EndDisabled();

            ImGui::Dummy(ImVec2{0.0f, 50.0f});

            ImGui::Text("Open existing project from selected directory:");
            ImGui::Separator();

            ImGui::Columns(8, "ProjectColumns", false);

            std::vector<std::filesystem::path> projects;
            if (auto projectsDir = std::filesystem::path(projectPath); std::filesystem::exists(projectsDir) &&
                                                                       std::filesystem::is_directory(projectsDir)) {
                for (const auto& entry : std::filesystem::directory_iterator(projectsDir)) {
                    if (entry.is_directory()) {
                        for (const auto& file : std::filesystem::directory_iterator(entry.path())) {
                            if (file.path().extension() == Config::PROJECT_FILE_EXTENSION) {
                                projects.push_back(file.path());
                            }
                        }
                    }
                }
            }

            for (const auto& project : projects) {
                std::string localProjectName = project.filename().replace_extension("").string();
                std::string localProjectPath = project.parent_path().string();

                if (ImGui::ImageButton(localProjectName.c_str(), EditorAssets::ProjectIconTexture()->getNativeHandle(),
                                       ImVec2(50, 50), ImVec2(0.0f, 0.0f),
                                       ImVec2(1.0f, 1.0f))) {
                    game.CloseProject();
                    game.LoadProject(project.string());

                    game.Scenes.GetCurrentScene()->IsPaused = true;
                    game.Window.setTitle("LOWEditor: " + game.Title);

                    isVisible = false;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::TextWrapped("%s", localProjectName.c_str());

                ImGui::NextColumn();
            }

            ImGui::Columns(1);

            ImGui::Dummy(ImVec2{0.0f, 50.0f});
            ImGui::Separator();

            if (!game.ProjectDirectory.empty()) {
                if (ImGui::Button("Close")) {
                    isVisible = false;
                    ImGui::CloseCurrentPopup();
                }
            }

            ImGui::EndPopup();
        }
    }

}