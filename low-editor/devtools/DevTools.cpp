#include "DevTools.h"

#include <filesystem>
#include <fstream>
#include <sstream>

#include <ImGuiFileDialog.h>

#include "Game.h"
#include "scene/Scene.h"
#include "EditorConfig.h"
#include "FormatHelpers.h"
#include "editors/cameraEditor.h"
#include "editors/SpriteEditor.h"

#include "editors/TransformEditor.h"

namespace LowEngine {
    sf::Texture DevTools::playTexture;
    sf::Texture DevTools::pauseTexture;
    sf::Texture DevTools::stopTexture;
    sf::Texture DevTools::nextTexture;
    sf::Texture DevTools::loopTexture;
    sf::Texture DevTools::trashTexture;
    sf::Texture DevTools::projectIconTexture;
    sf::Texture DevTools::soundIconTexture;
    sf::Texture DevTools::musicIconTexture;
}

namespace LowEngine {
    std::vector<ComponentEditorBinding> DevTools::ComponentEditorBindings;

    bool DevTools::Initialize(sf::RenderWindow& window) {
        bool result = ImGui::SFML::Init(window);
        if (!result) {
            _log->error("Failed to initialize ImGui.");
            return false;
        }

        result = LoadIcons();
        if (!result) {
            _log->error("Failed to load icons for DevTools.");
            return false;
        }

		// build list of component editor bindings
        ComponentEditorBindings.emplace_back("Transform", typeid(LowEngine::ECS::TransformComponent),
                                             LowEngine::Editors::AddTransformComponent,
                                             LowEngine::Editors::DrawTransformEditor);
        ComponentEditorBindings.emplace_back("Camera", typeid(LowEngine::ECS::CameraComponent),
                                             LowEngine::Editors::AddCameraComponent,
                                             LowEngine::Editors::DrawCameraEditor);
        ComponentEditorBindings.emplace_back("Sprite", typeid(LowEngine::ECS::SpriteComponent),
                                             LowEngine::Editors::AddSpriteComponent,
                                             LowEngine::Editors::DrawSpriteEditor);

        return true;
    }

    void DevTools::Free() { ImGui::SFML::Shutdown(); }

    void DevTools::ReadInput(const sf::RenderWindow& window, const std::optional<sf::Event>& event) {
        ImGui::SFML::ProcessEvent(window, *event);
    }

    void DevTools::Update(sf::RenderWindow& window, sf::Time deltaTime) {
        ImGui::SFML::Update(window, deltaTime);
    }

    void DevTools::Build(Game& game) {
        auto displaySize = game.Window.getSize();

        DisplayMainMenu(game);
        DisplayToolbar(game, displaySize, 25);

        auto scene = game.Scenes.GetCurrentScene();
        DisplayWorldOutliner(scene, 10, 30, 250, displaySize.y - 40);
        DisplayProperties(scene, displaySize.x - 260, 30, 250, displaySize.y - 40);

        DisplayLog(270, displaySize.y - 260, displaySize.x - 540, 260);

        if (_isInputEditorVisible) { DisplayInputEditor(game); }
        if (_isAssetBrowserVisible) { DisplayAssetBrowser(game); }
        if (_isNewProjectWizardVisible) { DisplayProjectWizard(game); }
    }

    void DevTools::Render(sf::RenderWindow& window) { ImGui::SFML::Render(window); }

    void DevTools::DisplayLog(size_t posX, size_t posY, size_t sizeX, size_t sizeY) {
        static size_t lastLogSize = 0;

        ImGui::SetNextWindowPos(ImVec2(posX, posY));
        ImGui::SetNextWindowSize(ImVec2(sizeX, sizeY));
        ImGui::Begin("Log");

        ImGui::BeginChild("Log Content", ImVec2(0, 0), true,
                          ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_AlwaysVerticalScrollbar);
        ImGui::TextUnformatted(_logContent.data(), _logContent.data() + _logContent.size());

        if (_logContent.size() != lastLogSize) {
            ImGui::SetScrollHereY(1.0f);
            lastLogSize = _logContent.size();
        }

        ImGui::EndChild();

        ImGui::End();
    }

    void DevTools::DisplayMainMenu(Game& game) {
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("Project")) {
                if (ImGui::MenuItem("New Project")) { _isNewProjectWizardVisible = true; }

                if (ImGui::MenuItem("Load project")) {
                    IGFD::FileDialogConfig config;
                    config.path = "projects";
                    config.fileName = "";
                    ImGuiFileDialog::Instance()->OpenDialog("LoadProject", "Load project",
                        LowEngine::Config::PROJECT_FILE_EXTENSION.c_str(), config);
                }

                if (ImGui::MenuItem("Save Project")) {
                    std::filesystem::path projectDirectory = std::filesystem::path(LowEditor::Config::PROJECT_DIRECTORY) / game.Title;

                    try {
                        std::filesystem::create_directories(projectDirectory);

                        std::filesystem::path projectFilePath = projectDirectory / std::filesystem::path(
                                                                    game.Title + LowEngine::Config::PROJECT_FILE_EXTENSION);

                        bool success = game.SaveProject(projectFilePath.string());
                        if (!success) { _log->error("Failed to save project to: {}", projectFilePath.string()); }
                    } catch (const std::filesystem::filesystem_error& e) {
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

    void DevTools::DisplayProjectWizard(Game& game) {
        static char projectName[150] = "";
        InitStaticCharArray(projectName, 150, LowEditor::Config::DEFAULT_NEW_PROJECT_NAME);
        static char projectPath[256] = "";
        InitStaticCharArray(projectPath, 256,
                            std::filesystem::path(std::filesystem::current_path() / std::filesystem::path(LowEditor::Config::PROJECT_DIRECTORY)).
                            string());

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
                                                        LowEngine::Config::PROJECT_FILE_EXTENSION.c_str(), config);
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
                    currentFileName.erase(currentFileName.size() - LowEngine::Config::PROJECT_FILE_EXTENSION.size());
                    // Remove file extension
                    strncpy(projectName, currentFileName.c_str(), sizeof(projectName) - 1);
                    projectName[sizeof(projectName) - 1] = '\0';
                }
                ImGuiFileDialog::Instance()->Close();
            }

            ImGui::Text("Project Name:");
            ImGui::SameLine();

            ImGui::SetNextItemWidth(400);
            ImGui::InputText("##ProjectName", projectName, sizeof(projectName));

            // check if project with current name already exists
            bool projectNameExists = false;
            auto theoreticalProjectFile = std::filesystem::path(projectPath) / std::filesystem::path(projectName)
                                          / std::filesystem::path(projectName).concat(LowEngine::Config::PROJECT_FILE_EXTENSION);

            if (std::filesystem::exists(theoreticalProjectFile)) {
                projectNameExists = true;
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Project with this name already exists!");
            } else {
                projectNameExists = false;
                ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Project name is available.");
            }

            if (projectNameExists) ImGui::BeginDisabled();
            if (ImGui::Button("Create new project")) {
                CreateNewProject(game, projectName, projectPath);
            }
            if (projectNameExists) ImGui::EndDisabled();

            ImGui::Dummy(ImVec2{0.0f, 50.0f});

            ImGui::Text("Open existing project from selected directory:");
            ImGui::Separator();

            ImGui::Columns(8, "ProjectColumns", false);

            std::vector<std::filesystem::path> projects;
            if (auto projectsDir = std::filesystem::path(projectPath); std::filesystem::exists(projectsDir) && std::filesystem::is_directory(projectsDir)) {
                for (const auto& entry: std::filesystem::directory_iterator(projectsDir)) {
                    if (entry.is_directory()) {
                        for (const auto& file: std::filesystem::directory_iterator(entry.path())) {
                            if (file.path().extension() == LowEngine::Config::PROJECT_FILE_EXTENSION) {
                                projects.push_back(file.path());
                            }
                        }
                    }
                }
            }

            for (auto const& project: projects) {
                std::string localProjectName = project.filename().replace_extension("").string();
                std::string localProjectPath = project.parent_path().string();

                if (ImGui::ImageButton(localProjectName.c_str(), projectIconTexture.getNativeHandle(), ImVec2(50, 50), ImVec2(0.0f, 0.0f),
                                       ImVec2(1.0f, 1.0f))) {
                    game.CloseProject();
                    game.LoadProject(project.string());

                    game.Scenes.GetCurrentScene()->IsPaused = true;
                    game.Window.setTitle("LOWEditor: " + game.Title);

                    _isNewProjectWizardVisible = false;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::TextWrapped("%s", localProjectName.c_str());

                ImGui::NextColumn();
            }

            ImGui::Columns(1);

            ImGui::Dummy(ImVec2{0.0f, 50.0f});
            ImGui::Separator();

            // On start, the editor creates a "default" scene, just so it could display UI without any special hacks.
            // It does not create any temporary project folder, so managing assets in an environment like that would require a lot of special flows.
            // The easiest solution to this is to prevent user from accessing assets managers when the project directory is not fully set.
            // This check prevents the user from closing the Project Wizard until Project Directory is selected by creating a new project or opening an existing one.
            if (!game.ProjectDirectory.empty()) {
                if (ImGui::Button("Close")) {
                    _isNewProjectWizardVisible = false;
                    ImGui::CloseCurrentPopup();
                }
            }

            ImGui::EndPopup();
        }
    }

    void DevTools::DisplayToolbar(Game& game, const sf::Vector2u& displaySize, float YOffset) {
        float buttonSize = 30.0f;

        ImVec2 windowPadding = ImGui::GetStyle().WindowPadding;
        ImVec2 itemSpacing = ImGui::GetStyle().ItemSpacing;

        float toolbarWidth = buttonSize * 3 + itemSpacing.x * 2;
        float windowWidth = toolbarWidth + windowPadding.x * 2 + 24;
        float windowHeight = buttonSize + windowPadding.y * 2 + 8;

        float toolbarX = (displaySize.x - windowWidth) / 2;
        float toolbarY = YOffset;

        ImVec2 uv0(0.0f, 0.0f);
        ImVec2 uv1(1.0f, 1.0f);

        ImGui::SetNextWindowPos(ImVec2{toolbarX, toolbarY});
        ImGui::SetNextWindowSize(ImVec2{windowWidth, windowHeight});

        bool isCurrentSceneTemporary = game.Scenes.GetCurrentScene()->IsTemporary;

        ImGui::Begin("Toolbar", nullptr,
                     ImGuiWindowFlags_NoTitleBar |
                     ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoScrollbar |
                     ImGuiWindowFlags_NoSavedSettings);

        // PLAY button
        if (isCurrentSceneTemporary) ImGui::BeginDisabled();
        if (ImGui::ImageButton("##Play", playTexture.getNativeHandle(), ImVec2{buttonSize, buttonSize}, uv0, uv1)) {
            _selectedEntityId = -1;
            size_t tempSceneIndex = game.Scenes.CreateCopySceneFromCurrent(LowEditor::Config::TEMPORARY_SCENE_SUFFIX);
            if (tempSceneIndex != Config::MAX_SIZE) {
                game.Scenes.SelectScene(tempSceneIndex);
                game.Scenes.GetCurrentScene()->IsPaused = false;
            } else { _log->error("Failed to create temporary scene"); }
        }
        if (isCurrentSceneTemporary) ImGui::EndDisabled();

        ImGui::SameLine(0);

        // PAUSE button
        ImVec4 tint = game.Scenes.GetCurrentScene()->IsPaused
                          ? ImVec4(1.0f, 0.3f, 0.3f, 1.0f)
                          : ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        if (!isCurrentSceneTemporary) ImGui::BeginDisabled();
        if (ImGui::ImageButton("##Pause", pauseTexture.getNativeHandle(), ImVec2{buttonSize, buttonSize}, uv0, uv1,
                               ImVec4(0, 0, 0, 0), tint)) {
            game.Scenes.GetCurrentScene()->IsPaused = !game.Scenes.GetCurrentScene()->IsPaused;
        }

        ImGui::SameLine(0);

        // STOP button
        if (ImGui::ImageButton("##Stop", stopTexture.getNativeHandle(), ImVec2{buttonSize, buttonSize}, uv0, uv1)) {
            _selectedEntityId = -1;
            game.Scenes.DestroyCurrentScene();
        }
        if (!isCurrentSceneTemporary) ImGui::EndDisabled();

        ImGui::End();
    }

    void DevTools::DisplayWorldOutliner(Scene* scene, int posX, int posY, int width, int height) {
        ImGui::SetNextWindowPos(ImVec2(posX, posY));
        ImGui::SetNextWindowSize(ImVec2(width, height));
        ImGui::Begin(std::format("Scene: '{}'", scene->Name).c_str());

        if (ImGui::Button("+", ImVec2(20, 20))) { ImGui::OpenPopup("Create Entity"); }

        if (ImGui::BeginPopup("Create Entity")) {
            static char nameBuffer[255] = "New Entity"; // 'static' to retain value between frames

            ImGui::Text("Name of the new Entity:");
            ImGui::SameLine();
            ImGui::InputText("##Name of the new Entity", nameBuffer, sizeof(nameBuffer));
            if (ImGui::Button("Create")) {
                if (std::strlen(nameBuffer) > 0) {
                    auto newEntity = scene->AddEntity(nameBuffer);
                    if (newEntity) { _selectedEntityId = newEntity->Id; }
                }
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        ImGui::SameLine();

        if (_selectedEntityId == -1) ImGui::BeginDisabled();
        if (ImGui::Button("-", ImVec2(20, 20))) { ImGui::OpenPopup("Destroy Entity"); }
        if (_selectedEntityId == -1) ImGui::EndDisabled();

        if (ImGui::BeginPopup("Destroy Entity")) {
            ImGui::Text("Are you sure you want to destroy the selected Entity?");
            if (ImGui::Button("Yes")) {
                if (_selectedEntityId != -1) {
                    scene->DestroyEntity(_selectedEntityId);
                    _selectedEntityId = -1;
                }
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        ImGui::Separator();

        auto entities = scene->GetEntities();
        for (auto& entity: *entities) {
            if (entity == nullptr) continue;

            std::string label = std::format("[{}] {}", entity.get()->Id, entity.get()->Name);
            bool selected = _selectedEntityId != -1 && entity.get()->Id == _selectedEntityId;

            if (ImGui::Selectable(label.c_str(), selected)) { _selectedEntityId = entity.get()->Id; }
        }

        ImGui::End();
    }

    void DevTools::DisplayProperties(Scene* scene, int posX, int posY, int width, int height) {
        ImGui::SetNextWindowPos(ImVec2(posX, posY));
        ImGui::SetNextWindowSize(ImVec2(width, height));
        if (_selectedEntityId != -1) {
            auto entity = scene->GetEntity(_selectedEntityId);
            ImGui::Begin(std::format("Properties: '{}'", entity->Name).c_str());

            ImGui::Text("Name:");
            ImGui::SameLine();
            auto nameBuffer = new char[255];
            std::strncpy(nameBuffer, entity->Name.c_str(), 255);
            if (ImGui::InputText("##Name", nameBuffer, 255, ImGuiInputTextFlags_EnterReturnsTrue)) {
                entity->Name = nameBuffer;
                scene->Update(0.0f);
            }

            if (ImGui::Button("(+) Add Component", ImVec2(width - 15, 20))) { ImGui::OpenPopup("Add component"); }
            
            for (auto& binding : ComponentEditorBindings) {
                if (entity->HasComponent(binding.ComponentType)) {
	                binding.DrawEditor(scene, _selectedEntityId);
                }
            }

            if (ImGui::BeginPopup("Add component")) {
                for (auto& binding : ComponentEditorBindings) {
                    if (ImGui::MenuItem(binding.Label.c_str())) {
						binding.AddComponent(scene, _selectedEntityId);
                        scene->Update(0.0f);
                    }
                }
	            ImGui::EndPopup();
            }

            

   //          if (ImGui::BeginPopup("Add component")) {
   //              if (ImGui::MenuItem("Transform")) {
   //                  scene->AddComponent<ECS::TransformComponent>(_selectedEntityId);
   //                  scene->Update(0.0f);
   //              }
   //              if (ImGui::MenuItem("Sprite")) {
   //                  scene->AddComponent<ECS::SpriteComponent>(_selectedEntityId);
   //                  scene->Update(0.0f);
   //              }
   //              if (ImGui::MenuItem("Animated Sprite")) {
   //                  scene->AddComponent<ECS::AnimatedSpriteComponent>(_selectedEntityId);
   //                  scene->Update(0.0f);
   //              }
   //              if (ImGui::MenuItem("Camera")) {
   //                  scene->AddComponent<ECS::CameraComponent>(_selectedEntityId);
   //                  scene->Update(0.0f);
   //              }
   //              if (ImGui::MenuItem("Collider")) {
   //                  scene->AddComponent<ECS::ColliderComponent>(_selectedEntityId);
   //                  scene->Update(0.0f);
   //              }
   //              ImGui::EndPopup();
   //          }
   //          
			// DisplayTransformComponentProperties(*scene);
   //          DisplayAnimatedSpriteComponentProperties(*scene);
   //          DisplayCameraComponentProperties(*scene);
   //          DisplayColliderComponentProperties(*scene);
        } else { ImGui::Begin("Properties:"); }

        ImGui::End();
    }

    void DevTools::DisplayTransformComponentProperties(Scene& scene) {
        auto entity = scene.GetEntity(_selectedEntityId);

        auto tc = scene.GetComponent<ECS::TransformComponent>(entity->Id);
        if (tc == nullptr) return;

        bool opened = ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen);

        if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
            ImGui::OpenPopup("TransformComponentContextMenu");
        }

        if (ImGui::BeginPopup("TransformComponentContextMenu")) {
            if (ImGui::MenuItem("Delete")) {
                if (scene.IsComponentSafeToDestroy<ECS::TransformComponent>(entity->Id)) {
                    //entity->DestroyComponent<ECS::TransformComponent>();
                }
            }
            ImGui::EndPopup();
        }

        if (opened) {
            ImGui::Text("Position:");
            ImGui::SameLine();
            float position[2] = {tc->Position.x, tc->Position.y};
            if (ImGui::DragFloat2("##Position", position, 1.0f, 0, 0, "%.3f")) {
                tc->Position = {position[0], position[1]};
                scene.Update(0.0f);
            }

            ImGui::Text("Rotation:");
            ImGui::SameLine();
            float rotation = tc->Rotation.asDegrees();
            if (ImGui::DragFloat("##Rotation", &rotation, 1.0f, 0, 0, "%.3f")) {
                tc->Rotation = sf::degrees(rotation);
                scene.Update(0.0f);
            }

            ImGui::Text("Scale:");
            ImGui::SameLine();
            float scale[2] = {tc->Scale.x, tc->Scale.y};
            if (ImGui::DragFloat2("##Scale", scale, 0.1f, 0, 0, "%.3f")) {
                tc->Scale = {scale[0], scale[1]};
                scene.Update(0.0f);
            }
        }
    }

    void DevTools::DisplayAnimatedSpriteComponentProperties(Scene& scene) {
        auto entity = scene.GetEntity(_selectedEntityId);

        auto asc = scene.GetComponent<ECS::AnimatedSpriteComponent>(entity->Id);
        if (asc == nullptr) return;

        std::vector<std::string> clipNames;
        std::vector<std::string> definedClipNames;

        if (Assets::HasSpriteSheet(asc->TextureId)) {
            auto& sheet = Assets::GetSpriteSheet(asc->TextureId);
            auto animClipNames = sheet.GetAnimationClipNames();
            definedClipNames.insert(definedClipNames.end(), animClipNames.begin(), animClipNames.end());
        }

        clipNames.insert(clipNames.end(), definedClipNames.begin(), definedClipNames.end());

        const auto& textureAliases = Assets::GetTextureAliases();
        std::vector<const char*> textureAliasesCStr;
        for (const auto& alias: textureAliases) {
            textureAliasesCStr.push_back(alias.c_str());
        }

        int currentTextureId = -1;
        if (asc->TextureId > 0) {
            std::string currentTextureAlias = Assets::GetTextureAlias(asc->TextureId);
            for (size_t i = 0; i < textureAliases.size(); ++i) {
                if (textureAliases[i] == currentTextureAlias) {
                    currentTextureId = static_cast<int>(i);
                    break;
                }
            }
        }

        std::vector<const char*> clipNamesCStr;
        for (const auto& name: clipNames) { clipNamesCStr.push_back(name.c_str()); }

        int currentClip = 0;

        if (!asc->CurrentClipName.empty()) {
            for (size_t i = 0; i < clipNames.size(); ++i) {
                if (clipNames[i] == asc->CurrentClipName) {
                    currentClip = i;
                    break;
                }
            }
        }

        bool opened = ImGui::CollapsingHeader("Animated Sprite", ImGuiTreeNodeFlags_DefaultOpen);

        if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
            ImGui::OpenPopup("AnimatedSpriteComponentContextMenu");
        }

        if (ImGui::BeginPopup("AnimatedSpriteComponentContextMenu")) {
            //if (ImGui::MenuItem("Delete")) { entity->DestroyComponent<ECS::AnimatedSpriteComponent>(); }
            ImGui::EndPopup();
        }

        if (opened) {
            ImGui::Text("Texture:");
            ImGui::SameLine();
            if (ImGui::Combo("##Texture:", &currentTextureId, textureAliasesCStr.data(), textureAliasesCStr.size())) {
                std::string textureAlias = textureAliasesCStr[currentTextureId];
                asc->SetTexture(textureAlias);
                scene.Update(0.0f);
            }

            if (asc->CurrentClipName.empty()) currentClip = -1;

            ImGui::Text("Clip name:");
            ImGui::SameLine();
            if (ImGui::Combo("##Clip name:", &currentClip, clipNamesCStr.data(), clipNamesCStr.size())) {
                asc->Play(clipNames[currentClip], asc->Loop);
                scene.Update(0.0f);
            }

            if (!asc->CurrentClipName.empty()) {
                // auto& clip = sheet.GetAnimationClip(asc->CurrentClipName);
                // ImGui::Separator();
                // ImGui::Text("Start frame: %i", clip.StartFrame);
                // ImGui::Text("End frame: %i", clip.EndFrame);
                // ImGui::Text("Frame count: %i", clip.FrameCount);
                // ImGui::Text("Frame duration: %.3f", clip.FrameDuration);
            } else {
            }
        }
    }

    void DevTools::DisplayCameraComponentProperties(Scene& scene) {
        auto entity = scene.GetEntity(_selectedEntityId);

        auto cc = scene.GetComponent<ECS::CameraComponent>(entity->Id);
        if (cc == nullptr) return;

        bool opened = ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen);

        if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
            ImGui::OpenPopup("CameraComponentContextMenu");
        }

        if (ImGui::BeginPopup("CameraComponentContextMenu")) {
            //if (ImGui::MenuItem("Delete")) { entity->DestroyComponent<ECS::CameraComponent>(); }
            ImGui::EndPopup();
        }

        if (opened) {
            ImGui::Text("Zoom factor:");
            ImGui::SameLine();
            float zoomFactor = cc->ZoomFactor;
            if (ImGui::DragFloat("##Zoom", &zoomFactor, 0.01f, 0, 0, "%.3f")) {
                cc->ZoomFactor = zoomFactor;
                cc->Update(0.0f);
            }
        }
    }

    void DevTools::DisplayColliderComponentProperties(Scene& scene) {
        auto entity = scene.GetEntity(_selectedEntityId);

        auto cc = scene.GetComponent<ECS::ColliderComponent>(entity->Id);
        if (cc == nullptr) return;

        bool opened = ImGui::CollapsingHeader("Collider", ImGuiTreeNodeFlags_DefaultOpen);

        if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
            ImGui::OpenPopup("ColliderComponentContextMenu");
        }

        if (ImGui::BeginPopup("ColliderComponentContextMenu")) {
            //if (ImGui::MenuItem("Delete")) { entity->DestroyComponent<ECS::ColliderComponent>(); }
            ImGui::EndPopup();
        }

        if (opened) {
            if (ImGui::Button("Create box collider ")) {
                cc->CreateBoxCollider(25.0f, 25.0f);
            }

            ImGui::Text("Show debug overlay:");
            ImGui::SameLine();
            bool showDebugOverlay = cc->DrawCollisionOverlay;
            if (ImGui::Checkbox("##ShowDebugOverlay", &showDebugOverlay)) {
                cc->DrawCollisionOverlay = showDebugOverlay;
            }
        }
    }

    void DevTools::DisplayInputEditor(Game& game) {
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        auto size = ImVec2(1224, 768);

        ImGui::SetNextWindowPos(ImVec2(center.x - size.x / 2, center.y - size.y / 2));
        ImGui::SetNextWindowSize(size);

        ImGui::OpenPopup("Input Editor");
        if (ImGui::BeginPopupModal("Input Editor", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            auto actions = game.Input.GetActions();

            ImGui::Text("Input Actions:");

            ImGui::SameLine();

            if (ImGui::Button("Add New Action")) {
                ImGui::OpenPopup("AddNewAction");
            }
            if (ImGui::BeginPopup("AddNewAction")) {
                static char nameBuffer[255] = "New Action"; // 'static' to retain value between frames
                static std::string errorMessage;

                ImGui::Text("Name of the new Action:");
                ImGui::SameLine();
                ImGui::InputText("##Name of the new Action", nameBuffer, sizeof(nameBuffer));
                if (ImGui::Button("Create")) {
                    errorMessage = "";
                    if (std::strlen(nameBuffer) > 0) {
                        if (!game.Input.HasAction(nameBuffer)) {
                            game.Input.AddAction(nameBuffer, sf::Keyboard::Key::Unknown);
                            _log->info("Action '{}' created successfully.", nameBuffer);

                            // Reset the name buffer
                            std::strncpy(nameBuffer, "New Action", sizeof(nameBuffer) - 1);
                            nameBuffer[sizeof(nameBuffer) - 1] = '\0';

                            ImGui::CloseCurrentPopup();
                        } else {
                            errorMessage = "Action '" + std::string(nameBuffer) + "' already exists.";
                            _log->warn("Action '{}' already exists.", nameBuffer);
                        }
                    } else {
                        errorMessage = "Action name cannot be empty.";
                        _log->warn("Action name cannot be empty.");
                    }
                }

                // Display error message if any
                if (!errorMessage.empty()) {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f));
                    ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "%s", errorMessage.c_str());
                    ImGui::PopStyleColor();
                }

                ImGui::EndPopup();
            }

            float availableHeight = size.y - 120;
            ImGui::BeginChild("TableContainer", ImVec2(0, availableHeight));

            if (ImGui::BeginTable("InputActionsTable", 10,
                                  ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY)) {
                ImGui::TableSetupColumn("Action Name", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 80.0f);
                ImGui::TableSetupColumn("Input", ImGuiTableColumnFlags_WidthFixed, 100.0f);
                ImGui::TableSetupColumn("LShift", ImGuiTableColumnFlags_WidthFixed, 50.0f);
                ImGui::TableSetupColumn("LCtrl", ImGuiTableColumnFlags_WidthFixed, 50.0f);
                ImGui::TableSetupColumn("LAlt", ImGuiTableColumnFlags_WidthFixed, 50.0f);
                ImGui::TableSetupColumn("RShift", ImGuiTableColumnFlags_WidthFixed, 50.0f);
                ImGui::TableSetupColumn("RCtrl", ImGuiTableColumnFlags_WidthFixed, 50.0f);
                ImGui::TableSetupColumn("RAlt", ImGuiTableColumnFlags_WidthFixed, 50.0f);
                ImGui::TableSetupColumn("##Delete", ImGuiTableColumnFlags_WidthFixed, 22.0f);
                ImGui::TableHeadersRow();

                for (auto& [actionName, action]: *actions) {
                    ImGui::TableNextRow();

                    // Action name column
                    ImGui::TableNextColumn();
                    ImGui::Text("%s", actionName.c_str());

                    // Type column
                    ImGui::TableNextColumn();
                    ImGui::Text("%s", action.Type == Input::ActionType::Keyboard ? "Keyboard" : "Mouse");

                    // Input column
                    ImGui::TableNextColumn();
                    if (_actionBeingBound == actionName) {
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.2f, 1.0f)); // Yellow highlight
                        ImGui::Text("Press any key...");
                        ImGui::PopStyleColor();

                        // Check for mouse input
                        for (int i = 0; i < 5; i++) {
                            // Check main mouse buttons
                            if (ImGui::IsMouseClicked(i)) {
                                action.Type = Input::ActionType::Mouse;
                                action.MouseButton = static_cast<sf::Mouse::Button>(i);
                                _actionBeingBound = "";
                                break;
                            }
                        }

                        // Check for keyboard input
                        for (int k = 0; k < sf::Keyboard::KeyCount; k++) {
                            if (sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(k))) {
                                action.Type = Input::ActionType::Keyboard;
                                action.Key = static_cast<sf::Keyboard::Key>(k);
                                _actionBeingBound = "";
                                break;
                            }
                        }
                    } else {
                        std::string inputName;
                        if (action.Type == Input::ActionType::Keyboard) {
                            inputName = game.Input.GetKeyName(action.Key);
                        } else { inputName = game.Input.GetMouseButtonName(action.MouseButton); }

                        ImGui::PushID(actionName.c_str()); // scoped ID to avoid conflicts for actions using the same buttons
                        if (ImGui::Button(inputName.c_str(), ImVec2(-1, 0))) {
                            _actionBeingBound = actionName;
                        }
                        ImGui::PopID();
                    }

                    // Modifiers columns
                    ImGui::TableNextColumn();
                    bool lshift = action.LShift;
                    if (ImGui::Checkbox(("##LShift" + actionName).c_str(), &lshift)) { action.LShift = lshift; }

                    ImGui::TableNextColumn();
                    bool lctrl = action.LCtrl;
                    if (ImGui::Checkbox(("##LCtrl" + actionName).c_str(), &lctrl)) { action.LCtrl = lctrl; }

                    ImGui::TableNextColumn();
                    bool lalt = action.LAlt;
                    if (ImGui::Checkbox(("##LAlt" + actionName).c_str(), &lalt)) { action.LAlt = lalt; }

                    ImGui::TableNextColumn();
                    bool rshift = action.RShift;
                    if (ImGui::Checkbox(("##RShift" + actionName).c_str(), &rshift)) { action.RShift = rshift; }

                    ImGui::TableNextColumn();
                    bool rctrl = action.RCtrl;
                    if (ImGui::Checkbox(("##RCtrl" + actionName).c_str(), &rctrl)) { action.RCtrl = rctrl; }

                    ImGui::TableNextColumn();
                    bool ralt = action.RAlt;
                    if (ImGui::Checkbox(("##RAlt" + actionName).c_str(), &ralt)) { action.RAlt = ralt; }

                    ImGui::TableNextColumn();
                    if (ImGui::ImageButton(("##Delete" + actionName).c_str(), trashTexture.getNativeHandle(),
                                           ImVec2{15.0f, 15.0f}, ImVec2{0.0f, 0.0f}, ImVec2{1.0f, 1.0f})) {
                        game.Input.RemoveAction(actionName.data()); //pass name as copy
                        break;
                    }
                }

                ImGui::EndTable();
            }

            ImGui::EndChild(); // end of TableContainer

            ImGui::Separator();

            if (ImGui::Button("Close")) {
                _isInputEditorVisible = false;
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }

    void DevTools::DisplayAssetBrowser(Game& game) {
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
                _isAssetBrowserVisible = false;
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }

    void DevTools::DisplayTextureBrowser(Game& game, ImVec2 size) {
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
        ImVec2 addTextureSize = ImVec2(800, 600);
        ImGui::SetNextWindowPos(ImVec2(center.x - addTextureSize.x / 2, center.y - addTextureSize.y / 2), ImGuiCond_Appearing);
        ImGui::SetNextWindowSize(addTextureSize, ImGuiCond_Appearing);
        if (ImGuiFileDialog::Instance()->Display("SelectTextureToAdd")) {
            if (ImGuiFileDialog::Instance()->IsOk()) {
                newTextureFile = std::filesystem::path(ImGuiFileDialog::Instance()->GetCurrentPath()) / std::filesystem::path(
                                     ImGuiFileDialog::Instance()->GetCurrentFileName());
                showAddNewTextureDialog = true;
            }
            ImGuiFileDialog::Instance()->Close();
        }

        auto aliases = Assets::GetTextureAliases();
        std::ranges::sort(aliases);

        ImGui::Columns(columnsCount, "TextureBrowserColumnsLayout", false);

        for (auto alias: aliases) {
            auto& texture = Assets::GetTexture(alias);

            if (ImGui::ImageButton(alias.c_str(), texture.getNativeHandle(), ImVec2(thumbnailSize, thumbnailSize), ImVec2(0.0f, 0.0f),
                                   ImVec2(1.0f, 1.0f))) {
                selectedTextureAlias = alias;
                selectedTextureSize = texture.getSize();
                showPreviewWindow = true;
            }

            ImGui::TextWrapped("%s", alias.c_str());

            ImGui::NextColumn();
        }

        ImGui::Columns(1); // reset columns layout

        ImGui::EndChild();

        if (showPreviewWindow) {
            ImGui::SetNextWindowSize(ImVec2(530, 595), ImGuiCond_Appearing);
            if (ImGui::Begin("Texture Preview", &showPreviewWindow, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize)) {
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

                if (selectedTextureAlias != LowEngine::Config::DEFAULT_TEXTURE_ALIAS) {
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
            ImVec2 addTextureDialogSize = ImVec2(750, 250);
            ImGui::SetNextWindowPos(ImVec2(addNewTextureDialogCenter.x - addTextureSize.x / 2, addNewTextureDialogCenter.y - addTextureSize.y / 2),
                                    ImGuiCond_Appearing);
            ImGui::SetNextWindowSize(ImVec2(addTextureDialogSize.x, addTextureDialogSize.y), ImGuiCond_Appearing);
            if (ImGui::Begin("Add New Texture", &showAddNewTextureDialog, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize)) {
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
                    std::filesystem::path targetPath = std::filesystem::path(game.ProjectDirectory) / std::filesystem::path(
                                                           LowEngine::Config::ASSETS_FOLDER_NAME) /
                                                       std::filesystem::path(LowEngine::Config::TEXTURES_FOLDER_NAME) / newTextureFile.filename();
                    std::filesystem::create_directories(targetPath.parent_path());
                    if (std::filesystem::exists(targetPath)) {
                        // manual removal of an existing file is required
                        // because MingGW's implementation of copy_file doesn't respect copy_options::overwrite_existing
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

    void DevTools::DisplaySpriteSheetBrowser(Game& game, ImVec2 size) {
        static bool showSpriteSheetEditor = false;
        static std::string selectedSpriteSheetAlias = ""; // empty string means new sprite sheet should be created
        static std::string spriteSheetAliasToDelete = "";

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

            for (const std::string& textureAlias: textureAliases) {
                if (textureAlias == LowEngine::Config::DEFAULT_TEXTURE_ALIAS) continue;
                size_t textureId = Assets::GetTextureId(textureAlias);

                if (!Assets::HasSpriteSheet(textureId)) continue;
                auto& spriteSheet = Assets::GetSpriteSheet(textureId);

                ImGui::TableNextRow();
                bool isSelected = selectedSpriteSheetAlias == textureAlias;

                // texture alias column
                ImGui::TableNextColumn();
                if (ImGui::Selectable(textureAlias.c_str(), isSelected, ImGuiSelectableFlags_SpanAllColumns)) {
                    selectedSpriteSheetAlias = textureAlias;
                    showSpriteSheetEditor = true;
                }

                // texture id column
                ImGui::TableNextColumn();
                ImGui::Text("%zu", textureId);

                // frame count column
                ImGui::TableNextColumn();
                ImGui::Text("%dx%d", spriteSheet.FrameCount.x, spriteSheet.FrameCount.y);

                // frame size column
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
            if (ImGui::Begin("Sprite Sheet Editor", &showSpriteSheetEditor, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize)) {
                if (selectedSpriteSheetAlias.empty()) {
                    ImGui::Dummy(maxPreviewSize);
                    ImGui::SameLine();
                    ImGui::BeginChild("SpriteSheetProperties", ImVec2(400.0f, 0.0f));
                    ImGui::Text("Sprite Sheet:");

                    auto textureAliases = Assets::GetTextureAliases();
                    std::vector<std::string> aliasesWithoutSpriteSheets;
                    std::ranges::copy_if(textureAliases, std::back_inserter(aliasesWithoutSpriteSheets),
                                         [&](const std::string& alias) {
                                             return !Assets::HasSpriteSheet(alias) && alias != LowEngine::Config::DEFAULT_TEXTURE_ALIAS;
                                         });
                    if (ImGui::BeginCombo("##Select New Sprite Sheet", "Select new sprite sheet...")) {
                        for (const auto& alias: aliasesWithoutSpriteSheets) {
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

                    // Draw grid lines for sprite sheet frames
                    ImVec2 imagePos = ImGui::GetItemRectMin();
                    ImVec2 imageSize = ImGui::GetItemRectSize();

                    if (spriteSheet.FrameCount.x > 0 && spriteSheet.FrameCount.y > 0) {
                        float cellWidth = imageSize.x / spriteSheet.FrameCount.x;
                        float cellHeight = imageSize.y / spriteSheet.FrameCount.y;
                        ImDrawList* drawList = ImGui::GetWindowDrawList();

                        for (size_t i = 1; i < spriteSheet.FrameCount.x; ++i) {
                            float x = imagePos.x + i * cellWidth;
                            drawList->AddLine(ImVec2(x, imagePos.y), ImVec2(x, imagePos.y + imageSize.y), IM_COL32(0, 255, 255, 255), 1.0f);
                        }
                        for (size_t j = 1; j < spriteSheet.FrameCount.y; ++j) {
                            float y = imagePos.y + j * cellHeight;
                            drawList->AddLine(ImVec2(imagePos.x, y), ImVec2(imagePos.x + imageSize.x, y), IM_COL32(0, 255, 255, 255), 1.0f);
                        }
                    }

                    ImGui::SameLine();

                    ImGui::BeginChild("SpriteSheetProperties", ImVec2(400.0f, 0.0f));
                    ImGui::Text("Sprite Sheet: %s", selectedSpriteSheetAlias.c_str());
                    ImGui::Separator();
                    ImGui::Text("Frame Count: ");
                    ImGui::SameLine();
                    static int spriteSheetEditorFrameCount[2] = {
                        static_cast<int>(spriteSheet.FrameCount.x), static_cast<int>(spriteSheet.FrameCount.y)
                    };
                    if (ImGui::DragInt2("##FrameCount", spriteSheetEditorFrameCount, 0.1f, 1, 100)) {
                        if (spriteSheetEditorFrameCount[0] > 0 && spriteSheetEditorFrameCount[1] > 0) {
                            spriteSheet.FrameCount = {
                                static_cast<size_t>(spriteSheetEditorFrameCount[0]), static_cast<size_t>(spriteSheetEditorFrameCount[1])
                            };
                            spriteSheet.FrameSize = {textureSize.x / spriteSheet.FrameCount.x, textureSize.y / spriteSheet.FrameCount.y};
                            _log->debug("Sprite sheet '{}' frame count updated to {}x{}", selectedSpriteSheetAlias, spriteSheet.FrameCount.x,
                                        spriteSheet.FrameCount.y);
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

    void DevTools::DisplayAnimationClipsBrowser(Game& game, ImVec2 size) {
        static bool showAnimationClipEditor = false;
        static std::string selectedAnimationClipAlias = ""; // empty string means new animation clip should be created
        static std::string selectedSpriteSheetAlias = "";

        ImGui::BeginChild("AnimationClipBrowser", size);

        if (ImGui::Button("Create Animation Clip")) {
            selectedSpriteSheetAlias.clear();
            selectedAnimationClipAlias.clear();
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

            for (const std::string& textureAlias: textureAliases) {
                if (textureAlias == LowEngine::Config::DEFAULT_TEXTURE_ALIAS) continue;
                size_t textureId = Assets::GetTextureId(textureAlias);

                if (!Assets::HasSpriteSheet(textureId)) continue;
                auto& spriteSheet = Assets::GetSpriteSheet(textureId);

                auto clipNames = spriteSheet.GetAnimationClipNames();
                std::ranges::sort(clipNames);

                for (const std::string& clipName: clipNames) {
                    ImGui::TableNextRow();

                    auto& animClip = spriteSheet.GetAnimationClip(clipName);

                    // clip name
                    ImGui::TableNextColumn();
                    bool isSelected = clipName == selectedAnimationClipAlias;
                    ImGui::PushID((textureAlias + "_" + clipName).c_str());
                    if (ImGui::Selectable(clipName.c_str(), isSelected, ImGuiSelectableFlags_SpanAllColumns)) {
                        selectedSpriteSheetAlias = textureAlias;
                        selectedAnimationClipAlias = clipName;
                        showAnimationClipEditor = true;
                    }
                    ImGui::PopID();

                    // texture alias
                    ImGui::TableNextColumn();
                    ImGui::Text("%s", textureAlias.c_str());

                    // texture id
                    ImGui::TableNextColumn();
                    ImGui::Text("%zu", textureId);

                    // frame count
                    ImGui::TableNextColumn();
                    ImGui::Text("%d", animClip.FrameCount);

                    // frame time
                    ImGui::TableNextColumn();
                    ImGui::Text("%.2f", animClip.FrameDuration);

                    // animation length
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
            static float currentAnimationFrame = 0;

            ImGui::SetNextWindowPos(pos);
            ImGui::SetNextWindowSize(editorSize, ImGuiCond_Appearing);
            if (ImGui::Begin("Animation Clip Editor", &showAnimationClipEditor, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize)) {
                if (selectedAnimationClipAlias.empty()) {
                    static std::string newAnimationClipError = "";

                    std::vector<std::string> textureAliasesWithSpriteSheets;
                    auto textureAliases = Assets::GetTextureAliases();
                    for (auto& textureAlias: textureAliases) {
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
                    if (ImGui::BeginCombo("##Select Sprite Sheet", "Select sprite sheet...")) {
                        for (const auto& textureAlias: textureAliasesWithSpriteSheets) {
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
                                    spriteSheet.AddAnimationClip(selectedAnimationClipAlias, 0, 1, 1.0f, sf::Vector2<size_t>(0, 0));
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
                    // Draw grid lines for sprite sheet frames
                    ImVec2 imagePos = ImGui::GetItemRectMin();
                    ImVec2 imageSize = ImGui::GetItemRectSize();

                    if (spriteSheet.FrameCount.x > 0 && spriteSheet.FrameCount.y > 0) {
                        float cellWidth = imageSize.x / spriteSheet.FrameCount.x;
                        float cellHeight = imageSize.y / spriteSheet.FrameCount.y;
                        ImDrawList* drawList = ImGui::GetWindowDrawList();

                        for (size_t i = 1; i < spriteSheet.FrameCount.x; ++i) {
                            float x = imagePos.x + i * cellWidth;
                            drawList->AddLine(ImVec2(x, imagePos.y), ImVec2(x, imagePos.y + imageSize.y), IM_COL32(0, 255, 255, 255), 1.0f);
                        }
                        for (size_t j = 1; j < spriteSheet.FrameCount.y; ++j) {
                            float y = imagePos.y + j * cellHeight;
                            drawList->AddLine(ImVec2(imagePos.x, y), ImVec2(imagePos.x + imageSize.x, y), IM_COL32(0, 255, 255, 255), 1.0f);
                        }

                        // Draw grid lines for the current animation
                        for (size_t i = 0; i < animClip.FrameCount; ++i) {
                            auto frame = animClip.Frames[i];
                            ImVec2 p1 = {
                                imagePos.x + (frame.position.x / frame.size.x) * cellWidth,
                                imagePos.y + (frame.position.y / frame.size.y) * cellHeight
                            };
                            ImVec2 p2 = {
                                imagePos.x + (frame.position.x / frame.size.x) * cellWidth,
                                imagePos.y + ((frame.position.y / frame.size.y) + 1) * cellHeight
                            };
                            ImVec2 p3 = {
                                imagePos.x + ((frame.position.x / frame.size.x) + 1) * cellWidth,
                                imagePos.y + ((frame.position.y / frame.size.y) + 1) * cellHeight
                            };
                            ImVec2 p4 = {
                                imagePos.x + ((frame.position.x / frame.size.x) + 1) * cellWidth,
                                imagePos.y + (frame.position.y / frame.size.y) * cellHeight
                            };
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
                    if (ImGui::DragInt("##FirstFrameX", &animClipEditorFirstFrameX, 0.1f, 0, spriteSheet.FrameCount.x - 1)) {
                        animClip.Frames[0].position.x = animClipEditorFirstFrameX * spriteSheet.FrameSize.x;
                        animClip.RecalculateFrames(spriteSheet);
                    }
                    ImGui::SameLine();
                    ImGui::Text("x");
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(100.0f);
                    if (ImGui::DragInt("##FirstFrameY", &animClipEditorFirstFrameY, 0.1f, 0, spriteSheet.FrameCount.y - 1)) {
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

    void DevTools::DisplayTileMapBrowser(const Game& game, ImVec2 size) {
        static bool showTileMapEditor = false;
        static std::string selectedTileMapAlias = ""; // empty string means new tile map should be created

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
        ImVec2 addTextureSize = ImVec2(800, 600);
        ImGui::SetNextWindowPos(ImVec2(center.x - addTextureSize.x / 2, center.y - addTextureSize.y / 2), ImGuiCond_Appearing);
        ImGui::SetNextWindowSize(addTextureSize, ImGuiCond_Appearing);
        if (ImGuiFileDialog::Instance()->Display("SelectTileMapToAdd")) {
            if (ImGuiFileDialog::Instance()->IsOk()) {
                selectedTileMapAlias.clear();
                newTileMapFile = std::filesystem::path(ImGuiFileDialog::Instance()->GetCurrentPath()) / std::filesystem::path(
                                     ImGuiFileDialog::Instance()->GetCurrentFileName());
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

            for (const std::string& tileMapAlias: tileMapAliases) {
                ImGui::TableNextRow();

                auto& tileMap = Assets::GetTileMap(tileMapAlias);

                // name column
                bool isSelected = false;
                ImGui::TableNextColumn();
                if (ImGui::Selectable(tileMap.Name.c_str(), &isSelected, ImGuiSelectableFlags_SpanAllColumns)) {
                    selectedTileMapAlias = tileMapAlias;
                    showTileMapEditor = true;
                }

                // alias column
                ImGui::TableNextColumn();
                ImGui::Text("%s", tileMapAlias.c_str());

                // size column
                ImGui::TableNextColumn();
                ImGui::Text("%dx%d pixels", tileMap.Size.x, tileMap.Size.y);

                // grid size column
                ImGui::TableNextColumn();
                bool hasTerrainLayer = tileMap.TerrainLayer.CellSize > 0;
                if (hasTerrainLayer) {
                    ImGui::Text("%dx%d cells", tileMap.TerrainLayer.CellCount.x, tileMap.TerrainLayer.CellCount.y);
                } else {
                    ImGui::Text("[no terrain layer]");
                }

                // terrain layer column
                ImGui::TableNextColumn();
                ImGui::Checkbox("##TerrainLayer", &hasTerrainLayer);

                // features layer column
                ImGui::TableNextColumn();
                bool hasFeaturesLayer = tileMap.FeaturesLayer.CellSize > 0;
                ImGui::Checkbox("##FeaturesLayer", &hasFeaturesLayer);

                // nav grid column
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

    void DevTools::ShowTileMapEditor(const Game& game, std::string& tileMapAlias, const std::filesystem::path& newTileMapFile,
                                     bool& showEditor) {
        static sf::RenderTexture tileMapPreview;

        ImVec2 editorSize{1200.0f, 800.0f};
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImVec2 pos{center.x - editorSize.x / 2, center.y - editorSize.y / 2};

        ImVec2 tileMapPreviewMaxSize{760.0f, 760.0f};

        ImGui::SetNextWindowPos(pos);
        ImGui::SetNextWindowSize(editorSize, ImGuiCond_Appearing);
        if (ImGui::Begin("Tile Map Layers Editor", &showEditor, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize)) {
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
                    // create a new tile map with empty cell definitions
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

                // sf::RenderTexture stores texture upside-down, so custom UVs are required
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

    void DevTools::ShowTileMapLayerEditor(const Game& game, Terrain::Layer& layer) {
        static size_t selectedCellDefinitionIndex = 0;

        static std::string selectedCellDefinitionNewAnimationName = "";

        ImGui::Text("Texture:");
        ImGui::SameLine();

        auto currentTerrainTextureAlias = Assets::GetTextureAlias(layer.TextureId);

        std::vector<std::string> aliasesWithSheets;
        auto textureAliases = Assets::GetTextureAliases();
        std::ranges::sort(textureAliases);
        for (auto& alias: textureAliases) {
            if (Assets::HasSpriteSheet(alias)) {
                aliasesWithSheets.emplace_back(alias);
            }
        }

        if (ImGui::BeginCombo("##TerrainTexture", currentTerrainTextureAlias.c_str())) {
            for (const auto& alias: aliasesWithSheets) {
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
        ImVec2 texturePreviewSize;
        ImVec2 texturePreviewMaxSize{350.0f, 100.0f};
        float scale = std::min(texturePreviewMaxSize.x / static_cast<float>(texture.getSize().x),
                               texturePreviewMaxSize.y / static_cast<float>(texture.getSize().y));
        texturePreviewSize = ImVec2(static_cast<float>(texture.getSize().x) * scale, static_cast<float>(texture.getSize().y) * scale);

        size_t definitionsCount = layer.Definition.CellDefinitions.size();
        float definitionHeight = definitionsCount > 0 ? texturePreviewSize.y / static_cast<float>(definitionsCount) : 0.0f;

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

                // index column
                ImGui::TableNextColumn();
                bool isSelected = index == selectedCellDefinitionIndex;
                if (ImGui::Selectable(std::to_string(index).c_str(), &isSelected)) {
                    selectedCellDefinitionIndex = index;
                }

                //move cost column
                ImGui::TableNextColumn();
                ImGui::InputFloat(std::format("##MoveCost_{}", index).c_str(), &definition.MoveCost);

                // walkable column
                ImGui::TableNextColumn();
                ImGui::Checkbox(std::format("##Walkable_{}", index).c_str(), &definition.IsWalkable);

                // swimmable column
                ImGui::TableNextColumn();
                ImGui::Checkbox(std::format("##Swimmable_{}", index).c_str(), &definition.IsSwimmable);

                // flyable column
                ImGui::TableNextColumn();
                ImGui::Checkbox(std::format("##Flyable_{}", index).c_str(), &definition.IsFlyable);

                // animation counts
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
                for (const auto& clipName: clipNames) {
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
            std::vector<std::string> availableClipNames; // clip names that are not yet in clipNames
            for (const auto& clipName: animationClipNames) {
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
                for (const auto& clipName: availableClipNames) {
                    bool isSelected = std::ranges::any_of(clipNames, [&](const auto& name) { return name == clipName; });
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

    void DevTools::DisplaySoundBrowser(const Game& game, ImVec2 size) {
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
            ImGuiFileDialog::Instance()->OpenDialog("SelectSoundToAdd", "Select sound to add...",
                                                    ".wav", config);
        }

        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImVec2 addSoundSize = ImVec2(800, 600);
        ImGui::SetNextWindowPos(ImVec2(center.x - addSoundSize.x / 2, center.y - addSoundSize.y / 2), ImGuiCond_Appearing);
        ImGui::SetNextWindowSize(addSoundSize, ImGuiCond_Appearing);
        if (ImGuiFileDialog::Instance()->Display("SelectSoundToAdd")) {
            if (ImGuiFileDialog::Instance()->IsOk()) {
                newSoundFile = std::filesystem::path(ImGuiFileDialog::Instance()->GetCurrentPath()) / std::filesystem::path(
                                   ImGuiFileDialog::Instance()->GetCurrentFileName());
                showAddNewSoundDialog = true;
            }
            ImGuiFileDialog::Instance()->Close();
        }

        auto aliases = Assets::GetSoundAliases();
        std::ranges::sort(aliases);

        ImGui::Columns(columnsCount, "SoundBrowserColumnsLayout", false);

        for (auto alias: aliases) {
            if (ImGui::ImageButton(alias.c_str(), soundIconTexture.getNativeHandle(), ImVec2(thumbnailSize, thumbnailSize), ImVec2(0.0f, 0.0f),
                                   ImVec2(1.0f, 1.0f))) {
                selectedSoundAlias = alias;
                showSoundPreviewWindow = true;
            }

            ImGui::TextWrapped("%s", alias.c_str());

            ImGui::NextColumn();
        }

        ImGui::Columns(1); // reset columns layout

        ImGui::EndChild();

        if (showSoundPreviewWindow) {
            ImGui::SetNextWindowSize(ImVec2(530, 590), ImGuiCond_Appearing);
            if (ImGui::Begin("Sound Preview", &showSoundPreviewWindow, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize)) {
                auto& sound = Assets::GetSound(selectedSoundAlias);

                ImGui::Text("Preview: %s", selectedSoundAlias.c_str());
                ImGui::Separator();
                if (ImGui::ImageButton(selectedSoundAlias.c_str(), soundIconTexture.getNativeHandle(), ImVec2(504.0f, 504.0f))) {
                    static sf::Sound soundPreview(sound);
                    soundPreview.setBuffer(sound);
                    soundPreview.play();
                }

                if (selectedSoundAlias != LowEngine::Config::DEFAULT_SOUND_ALIAS) {
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
            ImVec2 addSoundDialogSize = ImVec2(750, 250);
            ImGui::SetNextWindowPos(ImVec2(addNewSoundDialogCenter.x - addSoundSize.x / 2, addNewSoundDialogCenter.y - addSoundSize.y / 2),
                                    ImGuiCond_Appearing);
            ImGui::SetNextWindowSize(ImVec2(addSoundDialogSize.x, addSoundDialogSize.y), ImGuiCond_Appearing);
            if (ImGui::Begin("Add New Soun", &showAddNewSoundDialog, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize)) {
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
                    std::filesystem::path targetPath = std::filesystem::path(game.ProjectDirectory) / std::filesystem::path(
                                                           LowEngine::Config::ASSETS_FOLDER_NAME) /
                                                       std::filesystem::path(LowEngine::Config::SOUNDS_FOLDER_NAME) / newSoundFile.filename();
                    std::filesystem::create_directories(targetPath.parent_path());
                    if (std::filesystem::exists(targetPath)) {
                        // manual removal of an existing file is required
                        // because MingGW's implementation of copy_file doesn't respect copy_options::overwrite_existing
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

    void DevTools::DisplayMusicBrowser(Game& game, ImVec2 size) {
        int columnsCount = 6;
        int thumbnailSize = 100;

        static bool showAddNewMusicDialog = false;
        static std::filesystem::path newMusicFile;

        ImGui::BeginChild("MusicBrowser", size);

        if (ImGui::Button("Add Music")) {
            IGFD::FileDialogConfig config;
            config.path = ".";
            config.fileName = "";
            ImGuiFileDialog::Instance()->OpenDialog("SelectMusicToAdd", "Select music to add...",
                                                    ".mp3, .wav", config);
        }

        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImVec2 addMusicSize = ImVec2(800, 600);
        ImGui::SetNextWindowPos(ImVec2(center.x - addMusicSize.x / 2, center.y - addMusicSize.y / 2), ImGuiCond_Appearing);
        ImGui::SetNextWindowSize(addMusicSize, ImGuiCond_Appearing);
        if (ImGuiFileDialog::Instance()->Display("SelectMusicToAdd")) {
            if (ImGuiFileDialog::Instance()->IsOk()) {
                newMusicFile = std::filesystem::path(ImGuiFileDialog::Instance()->GetCurrentPath()) / std::filesystem::path(
                                   ImGuiFileDialog::Instance()->GetCurrentFileName());
                showAddNewMusicDialog = true;
            }
            ImGuiFileDialog::Instance()->Close();
        }

        auto aliases = Assets::GetMusicAliases();
        std::ranges::sort(aliases);

        if (ImGui::BeginTable("MusicBrowserLayoutTable", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_ScrollX,
                              ImVec2(1150.0f, 0.0f))) {
            ImGui::TableSetupColumn("MusicBrowserColumn", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_NoResize, 800.0f);
            ImGui::TableSetupColumn("MusicPlayerColumn", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 400.0f);

            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            ImGui::BeginChild("MusicBrowserPanel");
            ImGui::Columns(columnsCount, "MusicBrowserColumnsLayout", false);

            for (auto alias: aliases) {
                if (ImGui::ImageButton(alias.c_str(), musicIconTexture.getNativeHandle(), ImVec2(thumbnailSize, thumbnailSize), ImVec2(0.0f, 0.0f),
                                       ImVec2(1.0f, 1.0f))) {
                    game.Music.AddMusicToQueue(alias);
                    if (!game.Music.IsMusicPlaying()) {
                        game.Music.PlayNextQueued();
                    }
                }

                ImGui::TextWrapped("%s", alias.c_str());

                ImGui::NextColumn();
            }

            ImGui::Columns(1); // reset columns layout
            ImGui::EndChild();

            ImGui::TableNextColumn();
            ImGui::BeginChild("MusicPlayerPanel");
            if (game.Music.IsMusicPlaying() || game.Music.IsMusicPaused()) {
                auto alias = game.Music.GetCurrentMusicAlias();
                auto& music = Assets::GetMusic(alias);
                auto queue = game.Music.GetQueuedMusic();

                ImGui::Text("Currently playing: %s", alias.c_str());
                ImGui::Separator();
                ImGui::Image(musicIconTexture.getNativeHandle(), ImVec2(200.0f, 200.0f));
                ImGui::Separator();
                bool isPaused = music.getStatus() == sf::Music::Status::Paused;
                bool isLooping = music.isLooping();

                if (!isPaused) ImGui::BeginDisabled();
                if (ImGui::ImageButton("playButton", playTexture.getNativeHandle(), ImVec2(40.0f, 40.0f))) {
                    music.play();
                }
                if (!isPaused) ImGui::EndDisabled();
                ImGui::SameLine();
                if (isPaused) ImGui::BeginDisabled();
                if (ImGui::ImageButton("pauseButton", pauseTexture.getNativeHandle(), ImVec2(40.0f, 40.0f))) {
                    music.pause();
                }
                if (isPaused) ImGui::EndDisabled();
                ImGui::SameLine();
                if (ImGui::ImageButton("stopButton", stopTexture.getNativeHandle(), ImVec2(40.0f, 40.0f))) {
                    music.stop();
                    game.Music.ClearQueuedMusic();
                }
                ImGui::SameLine();
                if (queue.empty()) ImGui::BeginDisabled();
                if (ImGui::ImageButton("nextButton", nextTexture.getNativeHandle(), ImVec2(40.0f, 40.0f))) {
                    game.Music.PlayNextQueued();
                }
                if (queue.empty()) ImGui::EndDisabled();
                ImGui::SameLine();
                ImVec4 tint = isLooping
                                  ? ImVec4(1.0f, 0.3f, 0.3f, 1.0f)
                                  : ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
                if (ImGui::ImageButton("loopButton", loopTexture.getNativeHandle(), ImVec2(40.0f, 40.0f), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f),
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
                    auto& alias = queue[index];
                    ImGui::PushID(("remove_" + alias + std::to_string(index)).c_str());
                    if (ImGui::Button("-", ImVec2(20.0f, 20.0f))) {
                        game.Music.RemoveMusicFromQueue(index);
                    }
                    ImGui::PopID();

                    ImGui::SameLine();

                    ImGui::PushID(("select_" + alias + std::to_string(index)).c_str());
                    bool isSelected = alias == game.Music.GetCurrentMusicAlias();
                    if (ImGui::Selectable(alias.c_str(), isSelected)) {
                        game.Music.PlayMusic(alias);
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
            ImVec2 addMusicDialogSize = ImVec2(750, 250);
            ImGui::SetNextWindowPos(ImVec2(addNewMusicDialogCenter.x - addMusicSize.x / 2, addNewMusicDialogCenter.y - addMusicSize.y / 2),
                                    ImGuiCond_Appearing);
            ImGui::SetNextWindowSize(ImVec2(addMusicDialogSize.x, addMusicDialogSize.y), ImGuiCond_Appearing);
            if (ImGui::Begin("Add New Musi", &showAddNewMusicDialog, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize)) {
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
                    std::filesystem::path targetPath = std::filesystem::path(game.ProjectDirectory) / std::filesystem::path(
                                                           LowEngine::Config::ASSETS_FOLDER_NAME) /
                                                       std::filesystem::path(LowEngine::Config::MUSIC_FOLDER_NAME) / newMusicFile.filename();
                    std::filesystem::create_directories(targetPath.parent_path());
                    if (std::filesystem::exists(targetPath)) {
                        // manual removal of an existing file is required
                        // because MingGW's implementation of copy_file doesn't respect copy_options::overwrite_existing
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

    void DevTools::CreateNewProject(Game& game, const std::string& projectName, const std::string& projectPath) {
        if (!projectName.empty() && !projectPath.empty()) {
            _log->info("Creating new project: {} at {}", projectName, projectPath);

            auto projectDir = std::filesystem::path(projectPath) / std::filesystem::path(projectName);
            std::filesystem::create_directories(projectDir);

            auto projectFilePath = std::filesystem::path(projectDir) / std::filesystem::path(projectName + LowEngine::Config::PROJECT_FILE_EXTENSION);

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

            _isNewProjectWizardVisible = false;
            ImGui::CloseCurrentPopup();
        } else {
            _log->error("Failed to create project: Project name and path shouldn't be empty.");
        }
    }

    std::string DevTools::InsertSpaces(const std::string& str) {
        std::string result;
        for (std::size_t i = 0; i < str.size(); ++i) {
            if (i > 0 && std::isupper(static_cast<unsigned char>(str[i]))) { result.push_back(' '); }
            result.push_back(str[i]);
        }
        return result;
    }

    void DevTools::InitStaticCharArray(char* arr, size_t length, const std::string& value) {
        if (std::strlen(arr) == 0) {
            std::strncpy(arr, value.c_str(), length);
            arr[length - 1] = '\0'; // Ensure null-termination
        }
    }

    bool DevTools::LoadIcons() {
        if (!playTexture.loadFromFile("assets/editor/icons/forward.png"))
            return false;
        if (!pauseTexture.loadFromFile("assets/editor/icons/pause.png"))
            return false;
        if (!stopTexture.loadFromFile("assets/editor/icons/stop.png"))
            return false;
        if (!nextTexture.loadFromFile("assets/editor/icons/next.png"))
            return false;
        if (!loopTexture.loadFromFile("assets/editor/icons/return.png"))
            return false;
        if (!trashTexture.loadFromFile("assets/editor/icons/trashcan.png"))
            return false;
        if (!projectIconTexture.loadFromFile("assets/editor/icons/projecticon.png"))
            return false;
        if (!soundIconTexture.loadFromFile("assets/editor/icons/soundicon.png"))
            return false;
        if (!musicIconTexture.loadFromFile("assets/editor/icons/musicicon.png"))
            return false;
        return true;
    }
}
