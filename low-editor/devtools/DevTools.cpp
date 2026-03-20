#include "DevTools.h"

#include <filesystem>
#include <fstream>

#include <ImGuiFileDialog.h>

#include "Assets.h"
#include "Game.h"
#include "scene/Scene.h"
#include "EditorConfig.h"
#include "EngineConfig.h"
#include "editors/cameraEditor.h"
#include "editors/SoundCueEditor.h"
#include "editors/SoundEditor.h"
#include "editors/SpriteEditor.h"
#include "editors/AnimatedSpriteEditor.h"
#include "editors/ColliderEditor.h"
#include "editors/TransformEditor.h"

#include "panels/ProjectWizardPanel.h"
#include "panels/ScenePanel.h"
#include "panels/TerrainPanel.h"
#include "panels/InputEditorPanel.h"
#include "panels/AssetBrowserPanel.h"

namespace LowEngine {
    std::vector<ComponentEditorBinding> DevTools::ComponentEditorBindings;

    bool DevTools::Initialize(sf::RenderWindow &window) {
        bool result = ImGui::SFML::Init(window);
        if (!result) {
            _log->error("Failed to initialize ImGui.");
            return false;
        }

        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        result = EditorAssets::LoadEditorAssets();
        if (!result) {
            _log->error("Failed to load icons for DevTools.");
            return false;
        }

        ComponentEditorBindings.emplace_back("Transform", typeid(ECS::TransformComponent),
                                             Editors::AddTransformComponent,
                                             Editors::DrawTransformEditor);
        ComponentEditorBindings.emplace_back("Camera", typeid(ECS::CameraComponent),
                                             Editors::AddCameraComponent,
                                             Editors::DrawCameraEditor);
        ComponentEditorBindings.emplace_back("Sprite", typeid(ECS::SpriteComponent),
                                             Editors::AddSpriteComponent,
                                             Editors::DrawSpriteEditor);
        ComponentEditorBindings.emplace_back("Animated Sprite", typeid(ECS::AnimatedSpriteComponent),
                                             Editors::AddAnimatedSpriteComponent,
                                             Editors::DrawAnimatedSpriteEditor);
        ComponentEditorBindings.emplace_back("Sound", typeid(ECS::SoundComponent),
                                             Editors::AddSoundComponent,
                                             Editors::DrawSoundEditor);
        ComponentEditorBindings.emplace_back("Sound Cue", typeid(ECS::SoundCueComponent),
                                             Editors::AddSoundCueComponent,
                                             Editors::DrawSoundCueEditor);
        ComponentEditorBindings.emplace_back("Collider", typeid(ECS::ColliderComponent),
                                             Editors::AddColliderComponent,
                                             Editors::DrawColliderEditor);

        return true;
    }

    void DevTools::Free() {
        ImGui::SFML::Shutdown();
        EditorAssets::UnloadEditorAssets();
    }

    void DevTools::ReadInput(const sf::RenderWindow &window, const std::optional<sf::Event> &event) {
        ImGui::SFML::ProcessEvent(window, *event);
    }

    void DevTools::Update(sf::RenderWindow &window, sf::Time deltaTime) {
        DeltaTime = deltaTime;
        ImGui::SFML::Update(window, deltaTime);
    }

    void DevTools::BuildDockingHost() {
        ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGuiWindowFlags hostFlags =
                ImGuiWindowFlags_NoTitleBar |
                ImGuiWindowFlags_NoCollapse |
                ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoBringToFrontOnFocus |
                ImGuiWindowFlags_NoNavFocus |
                ImGuiWindowFlags_NoBackground |
                ImGuiWindowFlags_MenuBar;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpaceHost", nullptr, hostFlags);
        ImGui::PopStyleVar(3);

        ImGuiID dockspaceID = ImGui::GetID("DockSpaceHostID");
        ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
        ImGui::End();
    }

    void DevTools::Build(Game &game) {
        BuildDockingHost();

        if (_resetLayoutFrames > 0) {
            // layout reset was requested - spend few frames on moving panels to default positions
            _resetLayoutFrames--;
            if (_resetLayoutFrames == 0 && _saveLayoutAfterReset) {
                if (ImGui::GetIO().IniFilename != nullptr) {
                    ImGui::SaveIniSettingsToDisk(ImGui::GetIO().IniFilename);
                }
                _saveLayoutAfterReset = false;
            }
        }

        auto displaySize = game.Window.getSize();

        DisplayMainMenu(game);

        auto scene = game.Scenes.GetCurrentScene();

        if (_isInTerrainEditMode) {
            Panels::DrawTerrainToolbar(displaySize, _isInTerrainEditMode);
            Panels::DrawTerrainLayersList(scene, 10, 30, 250, 200, _selectedTerrainLayerIndex, _resetLayoutFrames);
            Panels::DrawTerrainLayerEditor(scene, 10, 240, 250, displaySize.y - 250, _selectedTerrainLayerIndex,
                                           _resetLayoutFrames);
        } else {
            Panels::DrawToolbar(game, displaySize, 25, _selectedEntityId);
            Panels::DrawWorldOutliner(scene, 10, 30, 250, displaySize.y - 40, _selectedEntityId, _resetLayoutFrames);
            Panels::DrawProperties(scene, displaySize.x - 260, 30, 250, displaySize.y - 40, _selectedEntityId,
                                   _resetLayoutFrames, ComponentEditorBindings);
            Panels::DrawLog(270, displaySize.y - 260, displaySize.x - 540, 260, _resetLayoutFrames);
        }

        if (_isInputEditorVisible) { Panels::DrawInputEditor(game, _isInputEditorVisible, _actionBeingBound); }
        if (_isAssetBrowserVisible) { Panels::DrawAssetBrowser(game, _isAssetBrowserVisible); }
        if (_isNewProjectWizardVisible) { Panels::DrawProjectWizard(game, _isNewProjectWizardVisible); }
    }

    void DevTools::Render(sf::RenderWindow &window) { ImGui::SFML::Render(window); }

    void DevTools::DisplayMainMenu(Game &game) {
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("Project")) {
                if (ImGui::MenuItem("New Project")) { _isNewProjectWizardVisible = true; }

                if (ImGui::MenuItem("Load project")) {
                    IGFD::FileDialogConfig config;
                    config.path = "projects";
                    config.fileName = "";
                    ImGuiFileDialog::Instance()->OpenDialog("LoadProject", "Load project",
                                                            Config::PROJECT_FILE_EXTENSION.c_str(), config);
                }

                if (ImGui::MenuItem("Save Project")) {
                    std::filesystem::path projectDirectory = std::filesystem::path(LowEditor::Config::PROJECT_DIRECTORY)
                                                             / game.Title;

                    try {
                        std::filesystem::create_directories(projectDirectory);

                        std::filesystem::path projectFilePath = projectDirectory / std::filesystem::path(
                                                                    game.Title + Config::PROJECT_FILE_EXTENSION);

                        bool success = game.SaveProject(projectFilePath.string());
                        if (!success) { _log->error("Failed to save project to: {}", projectFilePath.string()); }
                    } catch (const std::filesystem::filesystem_error &e) {
                        _log->error("Failed to create project directory '{}': {} (Error code: {})",
                                    projectDirectory.string(), e.what(), e.code().value());
                    }
                }

                ImGui::Separator();

                if (ImGui::MenuItem("Publish")) {
                }

                ImGui::Separator();

                if (ImGui::MenuItem("Assets")) { _isAssetBrowserVisible = true; }

                if (ImGui::MenuItem("Input")) { _isInputEditorVisible = true; }

                if (ImGui::MenuItem("Properties")) {
                }

                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Scene")) {
                if (ImGui::MenuItem("New Scene")) {
                }
                if (ImGui::MenuItem("Load Scene")) { game.LoadScene("default"); }
                if (ImGui::MenuItem("Save Scene")) { game.SaveCurrentScene(); }

                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Editor")) {
                if (ImGui::MenuItem("Terrain Editor")) { _isInTerrainEditMode = true; }

                ImGui::Separator();

                if (ImGui::MenuItem("Reset Layout")) {
                    ImGui::LoadIniSettingsFromMemory("", 0);
                    _resetLayoutFrames = 2;
                    _saveLayoutAfterReset = true;
                }

                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }

        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        auto size = ImVec2(800, 600);
        ImGui::SetNextWindowPos(ImVec2(center.x - size.x / 2, center.y - size.y / 2), ImGuiCond_Appearing);
        ImGui::SetNextWindowSize(size, ImGuiCond_Appearing);
        if (ImGuiFileDialog::Instance()->Display("LoadProject")) {
            if (ImGuiFileDialog::Instance()->IsOk()) {
                std::filesystem::path projectDirectory = ImGuiFileDialog::Instance()->GetCurrentPath();
                std::filesystem::path projectFileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
                std::filesystem::path fullPath = projectDirectory / projectFileName;

                game.CloseProject();
                game.LoadProject(fullPath.string());

                game.Scenes.GetCurrentScene()->IsPaused = true;
                game.Window.setTitle("LOWEditor: " + game.Title);
            }
            ImGuiFileDialog::Instance()->Close();
        }
    }
}
