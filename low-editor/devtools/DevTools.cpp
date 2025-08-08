#include "DevTools.h"

#include <filesystem>

#include "Config.h"

namespace LowEngine {
    sf::Texture DevTools::playTexture;
    sf::Texture DevTools::pauseTexture;
    sf::Texture DevTools::stopTexture;
    sf::Texture DevTools::trashTexture;
    sf::Texture DevTools::projectIconTexture;
}

namespace LowEngine {
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

        return true;
    }

    void DevTools::Free() { ImGui::SFML::Shutdown(); }

    void DevTools::ReadInput(const sf::RenderWindow& window, const std::optional<sf::Event>& event) {
        ImGui::SFML::ProcessEvent(window, *event);
    }

    void DevTools::Update(sf::RenderWindow& window, sf::Time deltaTime) { ImGui::SFML::Update(window, deltaTime); }

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
                    std::string projectFileExtension = ".lowproj";
                    ImGuiFileDialog::Instance()->OpenDialog("LoadProject", "Load project",
                                                            projectFileExtension.c_str(), config);
                }

                if (ImGui::MenuItem("Save Project")) {
                    std::filesystem::path projectDirectory = std::filesystem::path("projects") / game.Title;

                    try {
                        std::filesystem::create_directories(projectDirectory);

                        std::filesystem::path projectFilePath = projectDirectory / std::filesystem::path(
                                                                    game.Title + ".lowproj");

                        bool success = game.SaveProject(projectFilePath.string());
                        if (!success) { _log->error("Failed to save project to: {}", projectFilePath.string()); }
                    } catch (const std::filesystem::filesystem_error& e) {
                        _log->error("Failed to create project directory '{}': {} (Error code: {})",
                                    projectDirectory.string(), e.what(), e.code().value());
                    }
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
                if (ImGui::MenuItem("Open Scene")) {
                }
                if (ImGui::MenuItem("Save Scene")) {
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
                    strncpy_s(projectPath, currentPath.c_str(), sizeof(projectPath) - 1);
                    projectPath[sizeof(projectPath) - 1] = '\0';

                    std::string currentFileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
                    currentFileName.erase(currentFileName.size() - LowEngine::Config::PROJECT_FILE_EXTENSION.size());
                    // Remove file extension
                    strncpy_s(projectName, currentFileName.c_str(), sizeof(projectName) - 1);
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
            std::filesystem::path projectsDir = std::filesystem::path(projectPath);
            for (const auto& entry: std::filesystem::directory_iterator(projectsDir)) {
                if (entry.is_directory()) {
                    for (const auto& file: std::filesystem::directory_iterator(entry.path())) {
                        if (file.path().extension() == LowEngine::Config::PROJECT_FILE_EXTENSION) {
                            projects.push_back(file.path());
                        }
                    }
                }
            }

            for (auto& project: projects) {
                std::string projectName = project.filename().replace_extension("").string();
                std::string projectPath = project.parent_path().string();

                if (ImGui::ImageButton(projectName.c_str(), projectIconTexture.getNativeHandle(), ImVec2(50, 50), ImVec2(0.0f, 0.0f),
                                       ImVec2(1.0f, 1.0f))) {
                    game.CloseProject();
                    game.LoadProject(project.string());
                    game.Window.setTitle("LOWEditor: " + game.Title);
                    _isNewProjectWizardVisible = false;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::TextWrapped("%s", projectName.c_str());

                ImGui::NextColumn();
            }

            ImGui::Columns(1);

            ImGui::Dummy(ImVec2{0.0f, 50.0f});
            ImGui::Separator();

            if (ImGui::Button("Close")) {
                _isNewProjectWizardVisible = false;
                ImGui::CloseCurrentPopup();
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

            if (ImGui::BeginPopup("Add component")) {
                if (ImGui::MenuItem("Transform")) {
                    scene->AddComponent<ECS::TransformComponent>(_selectedEntityId);
                    scene->Update(0.0f);
                }
                if (ImGui::MenuItem("Animated Sprite")) {
                    scene->AddComponent<ECS::AnimatedSpriteComponent>(_selectedEntityId);
                    scene->Update(0.0f);
                }
                if (ImGui::MenuItem("Camera")) {
                    scene->AddComponent<ECS::CameraComponent>(_selectedEntityId);
                    scene->Update(0.0f);
                }
                ImGui::EndPopup();
            }

            DisplayTransformComponentProperties(*scene);
            DisplayAnimatedSpriteComponentProperties(*scene);
            DisplayCameraComponentProperties(*scene);
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
                    entity->DestroyComponent<ECS::TransformComponent>();
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
        if (asc->Sheet != nullptr) {
            const auto& definedClipNames = asc->Sheet->GetAnimationClipNames();
            clipNames.insert(clipNames.end(), definedClipNames.begin(), definedClipNames.end());
        }

        const auto& textureAliases = Assets::GetTextureAliases();
        std::vector<const char*> textureAliasesCStr;
        for (const auto& alias: textureAliases) { textureAliasesCStr.push_back(alias.c_str()); }

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

        if (asc->Clip != nullptr) {
            for (size_t i = 0; i < clipNames.size(); ++i) {
                if (clipNames[i] == asc->Clip->Name) {
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
            if (ImGui::MenuItem("Delete")) { entity->DestroyComponent<ECS::AnimatedSpriteComponent>(); }
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

            if (asc->Clip == nullptr) currentClip = -1;

            ImGui::Text("Clip name:");
            ImGui::SameLine();
            if (ImGui::Combo("##Clip name:", &currentClip, clipNamesCStr.data(), clipNamesCStr.size())) {
                asc->Play(clipNames[currentClip], asc->Loop);
                scene.Update(0.0f);
            }

            if (asc->Clip != nullptr) {
                ImGui::Separator();
                ImGui::Text("Start frame: %i", asc->Clip->StartFrame);
                ImGui::Text("End frame: %i", asc->Clip->EndFrame);
                ImGui::Text("Frame count: %i", asc->Clip->FrameCount);
                ImGui::Text("Frame duration: %.3f", asc->Clip->FrameDuration);
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
            if (ImGui::MenuItem("Delete")) { entity->DestroyComponent<ECS::CameraComponent>(); }
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
                ImGui::TableSetupColumn("##Delete", ImGuiTableColumnFlags_WidthFixed, 50.0f);
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
                    ImGui::Text("Manage Sprite sheets here");

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

                if (ImGui::Button("Delete", ImVec2(512.0f, 0.0f))) {
                    auto pathToTexture = texture.Path; // copy the path for logging purposes
                    std::filesystem::remove(texture.Path);
                    Assets::UnloadTexture(selectedTextureAlias);
                    _log->info("Texture '{}' deleted successfully from file: {}", selectedTextureAlias, pathToTexture.string());
                    showPreviewWindow = false;
                }

                ImGui::End();
            }
        }

        if (showAddNewTextureDialog) {
            ImVec2 addNewTextureDialogCenter = ImGui::GetMainViewport()->GetCenter();
            ImVec2 addTextureDialogSize = ImVec2(750, 250);
            ImGui::SetNextWindowPos(ImVec2(addNewTextureDialogCenter.x - addTextureSize.x / 2, addNewTextureDialogCenter.y - addTextureSize.y / 2), ImGuiCond_Appearing);
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
                    std::filesystem::path targetPath = std::filesystem::path(game.ProjectDirectory) / std::filesystem::path(LowEngine::Config::ASSETS_FOLDER_NAME) /
                                                      std::filesystem::path(LowEngine::Config::TEXTURES_FOLDER_NAME) / newTextureFile.filename();
                    std::filesystem::create_directories(targetPath.parent_path());
                    std::filesystem::copy(newTextureFile, targetPath, std::filesystem::copy_options::overwrite_existing);
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
        if (!trashTexture.loadFromFile("assets/editor/icons/trashcan.png"))
            return false;
        if (!projectIconTexture.loadFromFile("assets/editor/icons/projecticon.png"))
            return false;
        return true;
    }
}
