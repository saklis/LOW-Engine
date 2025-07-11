// #include "DevTools.h"
//
// namespace LowEngine {
//     void DevTools::Initialize(sf::RenderWindow& window) {
//         GetInstance()->setWindow(window);
//         tgui::Theme::setDefault("assets/themes/BabyBlue/BabyBlue.txt");;
//     }
//
//     void DevTools::ReadInput(const std::optional<sf::Event>& event) {
//         if (event.has_value()) {
//             GetInstance()->handleEvent(event.value());
//         }
//     }
//
//     void DevTools::Draw(sf::RenderWindow& window) {
//         GetInstance()->draw();
//     }
//
//     void DevTools::Free() {
//         GetInstance()->removeAllWidgets();
//     }
//
//     void DevTools::Build(Game& game) {
//         BuildMainMenuBar();
//         BuildMainToolbar();
//         BuildSceneOutliner(game);
//     }
//
//     void DevTools::Update(Game& game) {
//         UpdateSceneOutliner(game);
//     }
//
//     void DevTools::BuildMainMenuBar() {
//         auto gui = GetInstance();
//
//         auto mainMenu = tgui::MenuBar::create();
//
//         mainMenu->addMenu("File");
//         mainMenu->addMenuItem("New");
//         mainMenu->addMenuItem("Open");
//         mainMenu->addMenuItem("Save");
//
//         mainMenu->addMenu("Assets");
//         mainMenu->addMenuItem("Add...");
//         mainMenu->addMenuItem({"Assets", "Add...", "Texture"});
//         mainMenu->addMenuItem({"Assets", "Add...", "Sprite Sheet"});
//         mainMenu->addMenuItem({"Assets", "Add...", "Animation Clip"});
//         mainMenu->addMenuItem({"Assets", "Add...", "Tile Map"});
//         mainMenu->addMenuItem({"Assets", "Add...", "Font"});
//         mainMenu->addMenuItem("Browser");
//
//         gui->add(mainMenu);
//     }
//
//     void DevTools::BuildMainToolbar() {
//         auto gui = GetInstance();
//         auto windowSize = gui->getWindow()->getSize();
//
//         int toolbarWidth = 110;
//         int toolbarHeight = 40;
//
//         int buttonSize = 30;
//
//         auto toolbarPanel = tgui::Panel::create();
//         toolbarPanel->setSize(toolbarWidth, toolbarHeight);
//         toolbarPanel->setPosition(windowSize.x / 2 - toolbarWidth / 2, 20);
//
//         auto playButton = tgui::BitmapButton::create();
//         playButton->setImage("assets/editor/icons/forward.png");
//         playButton->setSize(buttonSize, buttonSize);
//         playButton->setPosition(5, 5);
//         toolbarPanel->add(playButton);
//
//         auto pauseButton = tgui::BitmapButton::create();
//         pauseButton->setImage("assets/editor/icons/pause.png");
//         pauseButton->setSize(buttonSize, buttonSize);;
//         pauseButton->setPosition(10+buttonSize, 5);
//         toolbarPanel->add(pauseButton);
//
//         auto stopButton = tgui::BitmapButton::create();
//         stopButton->setImage("assets/editor/icons/stop.png");
//         stopButton->setSize(buttonSize, buttonSize);
//         stopButton->setPosition(15+buttonSize*2, 5);
//         toolbarPanel->add(stopButton);
//
//         gui->add(toolbarPanel);
//     }
//
//     void DevTools::BuildSceneOutliner(Game& game) {
//         // build
//         auto gui = GetInstance();
//         auto windowSize = gui->getWindow()->getSize();
//
//         int outlinerWidth = 200;
//         int outlinerHeight = windowSize.y - 35;
//
//         auto sceneOutliner = tgui::ChildWindow::create();
//         sceneOutliner->setTitle("Scene");
//         sceneOutliner->setTitleButtons(tgui::ChildWindow::TitleButton::None);
//         sceneOutliner->setResizable(true);
//         sceneOutliner->setKeepInParent(true);
//
//         // outlinerWindow->setSize(outlinerWidth, outlinerHeight);
//         // outlinerWindow->setPosition(5, 30);
//
//         sceneOutliner->setWidth(outlinerWidth);
//         sceneOutliner->setAutoLayout(tgui::AutoLayout::Left);
//
//         auto entitiesList = tgui::ListBox::create();
//         entitiesList->setAutoLayout(tgui::AutoLayout::Fill);
//         sceneOutliner->add(entitiesList, SCENEOUTLINER_ENTITIESLISTBOX);
//
//         gui->add(sceneOutliner, SCENEOUTLINER);
//
//         // set default values
//         auto scene = game.Scenes.GetCurrentScene();
//         sceneOutliner->setTitle("Scene: " + scene->Name);
//
//         auto entities = scene->GetEntities();
//         for (auto& entity: *entities) {
//             std::string label = std::format("[{}] {}", entity.get()->Id, entity.get()->Name);
//             entitiesList->addItem(label, std::to_string(entity.get()->Id));
//         }
//     }
//
//     void DevTools::UpdateSceneOutliner(Game& game) {
//         // auto gui = GetInstance();
//         // auto sceneOutliner = gui->get<tgui::ChildWindow>(SCENEOUTLINER);
//         //
//         // auto scene = game.Scenes.GetCurrentScene();
//         // sceneOutliner->setTitle("Scene: " + scene->Name);
//         //
//         // auto entitiesList = sceneOutliner->get<tgui::ListBox>(SCENEOUTLINER_ENTITIESLISTBOX);
//         // if (entitiesList->getItemCount() > 0) {
//         //     entitiesList->removeAllItems();
//         // }
//         //
//         // auto entities = scene->GetEntities();
//         // for (auto& entity: *entities) {
//         //     std::string label = std::format("[{}] {}", entity.get()->Id, entity.get()->Name);
//         //     entitiesList->addItem(label, std::to_string(entity.get()->Id));
//         // }
//     }
// }


#include "DevTools.h"

namespace LowEngine {
    sf::Texture DevTools::playTexture;
    sf::Texture DevTools::pauseTexture;
    sf::Texture DevTools::stopTexture;
}

namespace LowEngine {
    bool DevTools::Initialize(sf::RenderWindow& window) {
        bool result = ImGui::SFML::Init(window);
        if (!result) {
            //TODO: "Failed to initialize ImGui-SFML"
            return false;
        }

        result = LoadIcons();
        if (!result) {
            //TODO: "Failed to load icons"
            return false;
        }

        return true;
    }

    void DevTools::Free() {
        ImGui::SFML::Shutdown();
    }

    void DevTools::ReadInput(const sf::RenderWindow& window, const std::optional<sf::Event>& event) {
        ImGui::SFML::ProcessEvent(window, *event);
    }

    void DevTools::Update(sf::RenderWindow& window, sf::Time deltaTime) {
        ImGui::SFML::Update(window, deltaTime);
    }

    void DevTools::Build(Game& game) {
        auto displaySize = game.Window.getSize();

        DisplayMenuBar(game);
        DisplayToolbar(game, displaySize, 25);

        auto scene = game.Scenes.GetCurrentScene();
        DisplayWorldOutliner(scene, 10, 30, 250, displaySize.y - 40);
        DisplayProperties(scene, displaySize.x - 260, 30, 250, displaySize.y - 40);

        DisplayLog(270, displaySize.y - 260, displaySize.x - 540, 260);
    }

    void DevTools::Render(sf::RenderWindow& window) {
        ImGui::SFML::Render(window);
    }

    void DevTools::DisplayLog(size_t posX, size_t posY, size_t sizeX, size_t sizeY) {
        static size_t lastLogSize = 0;

        ImGui::SetNextWindowPos(ImVec2(posX, posY));
        ImGui::SetNextWindowSize(ImVec2(sizeX, sizeY));
        ImGui::Begin("Log");

        std::ifstream logFile("engine.log");
        std::stringstream buffer;
        if (logFile.is_open()) {
            buffer << logFile.rdbuf();
            logFile.close();
        } else {
            ImGui::Text("Failed to open log file.");
        }

        std::string logContent = buffer.str();

        ImGui::BeginChild("Log Content", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_AlwaysVerticalScrollbar);
        ImGui::TextUnformatted(logContent.c_str());

        if (logContent.size() != lastLogSize) {
            ImGui::SetScrollHereY(1.0f);
            lastLogSize = logContent.size();
        }

        ImGui::EndChild();

        ImGui::End();
    }

    void DevTools::DisplayMenuBar(LowEngine::Game& game) {
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("Scene")) {
                if (ImGui::MenuItem("New")) {
                }
                if (ImGui::MenuItem("Open")) {
                }
                if (ImGui::MenuItem("Save")) {
                }

                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Assets")) {
                if (ImGui::BeginMenu("Add...")) {
                    if (ImGui::MenuItem("Texture")) {
                    }
                    if (ImGui::MenuItem("Animation Sheet")) {
                    }
                    if (ImGui::MenuItem("Animation Clip")) {
                    }
                    if (ImGui::MenuItem("Tile Map")) {
                    }
                    if (ImGui::MenuItem("Font")) {
                    }

                    ImGui::EndMenu();
                }
                if (ImGui::MenuItem("Browser")) {
                }

                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
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
            size_t tempSceneIndex = game.Scenes.CreateCopySceneFromCurrent(" (TEMPORARY)");
            if (tempSceneIndex != Config::MAX_SIZE) {
                game.Scenes.SelectScene(tempSceneIndex);
                game.Scenes.GetCurrentScene()->IsPaused = false;
            } else {
                //TODO: "Failed to create temporary scene"
            }
        }
        if (isCurrentSceneTemporary) ImGui::EndDisabled();

        ImGui::SameLine(0);

        // PAUSE button
        ImVec4 tint = game.Scenes.GetCurrentScene()->IsPaused ? ImVec4(1.0f, 0.3f, 0.3f, 1.0f) : ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        if (!isCurrentSceneTemporary) ImGui::BeginDisabled();
        if (ImGui::ImageButton("##Pause", pauseTexture.getNativeHandle(), ImVec2{buttonSize, buttonSize}, uv0, uv1, ImVec4(0, 0, 0, 0), tint)) {
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

        if (ImGui::Button("+", ImVec2(20, 20))) {
            ImGui::OpenPopup("Create Entity");
        }

        if (ImGui::BeginPopup("Create Entity")) {
            static char nameBuffer[255] = "New Entity"; // 'static' to retain value between frames

            ImGui::Text("Name of the new Entity:");
            ImGui::SameLine();
            ImGui::InputText("##Name of the new Entity", nameBuffer, sizeof(nameBuffer));
            if (ImGui::Button("Create")) {
                if (std::strlen(nameBuffer) > 0) {
                    auto newEntity = scene->AddEntity(nameBuffer);
                    if (newEntity) {
                        _selectedEntityId = newEntity->Id;
                    }
                }
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        ImGui::SameLine();

        if (_selectedEntityId == -1) ImGui::BeginDisabled();
        if (ImGui::Button("-", ImVec2(20, 20))) {
            ImGui::OpenPopup("Destroy Entity");
        }
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

            if (ImGui::Selectable(label.c_str(), selected)) {
                _selectedEntityId = entity.get()->Id;
            }
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
            char* nameBuffer = new char[255];
            std::strncpy(nameBuffer, entity->Name.c_str(), 255);
            if (ImGui::InputText("##Name", nameBuffer, 255, ImGuiInputTextFlags_EnterReturnsTrue)) {
                entity->Name = nameBuffer;
                scene->Update(0.0f);
            }

            if (ImGui::Button("(+) Add Component", ImVec2(width - 15, 20))) {
                ImGui::OpenPopup("Add component");
            }

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
        } else {
            ImGui::Begin("Properties:");
        }

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
        for (const auto& name: clipNames) {
            clipNamesCStr.push_back(name.c_str());
        }

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
            if (ImGui::MenuItem("Delete")) {
                entity->DestroyComponent<ECS::AnimatedSpriteComponent>();
            }
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
            if (ImGui::MenuItem("Delete")) {
                entity->DestroyComponent<ECS::CameraComponent>();
            }
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

    std::string DevTools::InsertSpaces(const std::string& str) {
        std::string result;
        for (std::size_t i = 0; i < str.size(); ++i) {
            if (i > 0 && std::isupper(static_cast<unsigned char>(str[i]))) {
                result.push_back(' ');
            }
            result.push_back(str[i]);
        }
        return result;
    }

    bool DevTools::LoadIcons() {
        if (!playTexture.loadFromFile("assets/editor/icons/forward.png"))
            return false;
        if (!pauseTexture.loadFromFile("assets/editor/icons/pause.png"))
            return false;
        if (!stopTexture.loadFromFile("assets/editor/icons/stop.png"))
            return false;
        return true;
    }
}
