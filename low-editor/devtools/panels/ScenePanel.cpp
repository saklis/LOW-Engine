#include "ScenePanel.h"

#include <imgui.h>

#include "devtools/Action.h"
#include "ecs/components/CameraComponent.h"
#include "Game.h"
#include "scene/Scene.h"
#include "EngineConfig.h"
#include "EditorConfig.h"
#include "devtools/Assets.h"
#include "devtools/ComponentEditorBinding.h"
#include "log/Log.h"

namespace LowEngine::Panels {

    void DrawLog(size_t posX, size_t posY, size_t sizeX, size_t sizeY, int resetLayoutFrames) {
        static size_t lastLogSize = 0;

        const ImGuiCond layoutCond = (resetLayoutFrames > 0) ? ImGuiCond_Always : ImGuiCond_FirstUseEver;
        ImGui::SetNextWindowPos(ImVec2(posX, posY), layoutCond);
        ImGui::SetNextWindowSize(ImVec2(sizeX, sizeY), layoutCond);
        if (resetLayoutFrames > 0) {
            ImGui::SetNextWindowDockID(0, ImGuiCond_Always);
        }

        ImGui::Begin("Log###Log");

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

    void DrawToolbar(Game& game, const sf::Vector2u& displaySize, float yOffset, size_t& selectedEntityId) {
        float buttonSize = 30.0f;

        ImVec2 windowPadding = ImGui::GetStyle().WindowPadding;
        ImVec2 itemSpacing = ImGui::GetStyle().ItemSpacing;

        float toolbarWidth = buttonSize * 3 + itemSpacing.x * 2;
        float windowWidth = toolbarWidth + windowPadding.x * 2 + 24;
        float windowHeight = buttonSize + windowPadding.y * 2 + 8;

        float toolbarX = (displaySize.x - windowWidth) / 2;
        float toolbarY = yOffset;

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

        if (isCurrentSceneTemporary) ImGui::BeginDisabled();
        if (ImGui::ImageButton("##Play", EditorAssets::PlayTexture()->getNativeHandle(), ImVec2{buttonSize, buttonSize},
                               uv0, uv1)) {
            selectedEntityId = -1;
            size_t tempSceneIndex = game.Scenes.CreateCopySceneFromCurrent(LowEditor::Config::TEMPORARY_SCENE_SUFFIX);
            if (tempSceneIndex != Config::INVALID_ID) {
                game.Scenes.SelectScene(tempSceneIndex);
                game.Scenes.GetCurrentScene()->IsPaused = false;
            } else { _log->error("Failed to create temporary scene"); }
        }
        if (isCurrentSceneTemporary) ImGui::EndDisabled();

        ImGui::SameLine(0);

        ImVec4 tint = game.Scenes.GetCurrentScene()->IsPaused
                          ? ImVec4(1.0f, 0.3f, 0.3f, 1.0f)
                          : ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        if (!isCurrentSceneTemporary) ImGui::BeginDisabled();
        if (ImGui::ImageButton("##Pause", EditorAssets::PauseTexture()->getNativeHandle(),
                               ImVec2{buttonSize, buttonSize}, uv0, uv1,
                               ImVec4(0, 0, 0, 0), tint)) {
            game.Scenes.GetCurrentScene()->IsPaused = !game.Scenes.GetCurrentScene()->IsPaused;
        }

        ImGui::SameLine(0);

        if (ImGui::ImageButton("##Stop", EditorAssets::StopTexture()->getNativeHandle(), ImVec2{buttonSize, buttonSize},
                               uv0, uv1)) {
            selectedEntityId = -1;
            game.Scenes.DestroyCurrentScene();
        }
        if (!isCurrentSceneTemporary) ImGui::EndDisabled();

        ImGui::End();
    }

    void DrawWorldOutliner(Scene* scene, int posX, int posY, int width, int height,
                           size_t& selectedEntityId, int resetLayoutFrames) {
        const ImGuiCond layoutCond = (resetLayoutFrames > 0) ? ImGuiCond_Always : ImGuiCond_FirstUseEver;
        ImGui::SetNextWindowPos(ImVec2(posX, posY), layoutCond);
        ImGui::SetNextWindowSize(ImVec2(width, height), layoutCond);
        if (resetLayoutFrames > 0) {
            ImGui::SetNextWindowDockID(0, ImGuiCond_Always);
        }

        const std::string windowTitle = std::format("Scene: '{}'###WorldOutliner", scene->Name);
        ImGui::Begin(windowTitle.c_str());

        if (ImGui::Button("+", ImVec2(20, 20))) { ImGui::OpenPopup("Create Entity"); }

        if (ImGui::BeginPopup("Create Entity")) {
            static char nameBuffer[255] = "New Entity";

            ImGui::Text("Name of the new Entity:");
            ImGui::SameLine();
            ImGui::InputText("##Name of the new Entity", nameBuffer, sizeof(nameBuffer));
            if (ImGui::Button("Create")) {
                if (std::strlen(nameBuffer) > 0) {
                    auto newEntity = scene->AddEntity(nameBuffer);
                    if (newEntity) { selectedEntityId = newEntity->Id; }
                }
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        ImGui::SameLine();

        if (selectedEntityId == static_cast<size_t>(-1)) ImGui::BeginDisabled();
        if (ImGui::Button("-", ImVec2(20, 20))) { ImGui::OpenPopup("Destroy Entity"); }
        if (selectedEntityId == static_cast<size_t>(-1)) ImGui::EndDisabled();

        if (ImGui::BeginPopup("Destroy Entity")) {
            ImGui::Text("Are you sure you want to destroy the selected Entity?");
            if (ImGui::Button("Yes")) {
                if (selectedEntityId != static_cast<size_t>(-1)) {
                    scene->DestroyEntity(selectedEntityId);
                    selectedEntityId = -1;
                }
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        ImGui::Separator();

        auto entities = scene->GetEntities();
        for (auto& entity : *entities) {
            if (entity == nullptr) continue;

            std::string label = std::format("[{}] {}", entity.get()->Id, entity.get()->Name);
            bool selected = selectedEntityId != static_cast<size_t>(-1) && entity.get()->Id == selectedEntityId;

            if (ImGui::Selectable(label.c_str(), selected)) { selectedEntityId = entity.get()->Id; }
        }

        ImGui::End();
    }

    void DrawProperties(Scene* scene, int posX, int posY, int width, int height,
                        size_t& selectedEntityId, int resetLayoutFrames,
                        const std::vector<ComponentEditorBinding>& bindings) {
        const ImGuiCond layoutCond = (resetLayoutFrames > 0) ? ImGuiCond_Always : ImGuiCond_FirstUseEver;
        ImGui::SetNextWindowPos(ImVec2(posX, posY), layoutCond);
        ImGui::SetNextWindowSize(ImVec2(width, height), layoutCond);
        if (resetLayoutFrames > 0) {
            ImGui::SetNextWindowDockID(0, ImGuiCond_Always);
        }

        if (selectedEntityId != static_cast<size_t>(-1)) {
            auto entity = scene->GetEntity(selectedEntityId);
            const std::string windowTitle = std::format("Properties: '{}'###Properties", entity->Name);
            ImGui::Begin(windowTitle.c_str());

            ImGui::Text("Name:");
            ImGui::SameLine();
            auto nameBuffer = new char[255];
            std::strncpy(nameBuffer, entity->Name.c_str(), 255);
            if (ImGui::InputText("##Name", nameBuffer, 255, ImGuiInputTextFlags_EnterReturnsTrue)) {
                entity->Name = nameBuffer;
                scene->Update(0.0f);
            }

            if (ImGui::Button("(+) Add Component", ImVec2(width - 15, 20))) { ImGui::OpenPopup("Add component"); }

            for (auto& binding : bindings) {
                if (entity->HasComponent(binding.ComponentType)) {
                    binding.DrawEditor(scene, selectedEntityId);
                }
            }

            if (ImGui::BeginPopup("Add component")) {
                for (auto& binding : bindings) {
                    if (ImGui::MenuItem(binding.Label.c_str())) {
                        binding.AddComponent(scene, selectedEntityId);
                        scene->Update(0.0f);
                    }
                }
                ImGui::EndPopup();
            }
        } else { ImGui::Begin("Properties###Properties"); }

        ImGui::End();
    }

    void CurrentCameraControls(Game& game, Scene* scene) {
        auto* tertiary = EditorAction::Action(MouseAction::Tertiary);
        if (tertiary->Pressed) {
            auto cameraEntity = scene->GetCurrentCamera();
            auto transform = scene->GetComponent<ECS::TransformComponent>(cameraEntity->Id);

            static sf::Vector2i prevMousePos;
            sf::Vector2i mousePos = sf::Mouse::getPosition(game.Window);

            if (tertiary->Started) {
                prevMousePos = mousePos;
            }

            sf::Vector2i mousePosDelta = mousePos - prevMousePos;
            transform->Position.x -= mousePosDelta.x;
            transform->Position.y -= mousePosDelta.y;

            prevMousePos = mousePos;
        }

        if (EditorAction::Action(MouseScrollAction::Up)->Started) {
            auto cameraEntity = scene->GetCurrentCamera();
            auto camera = scene->GetComponent<ECS::CameraComponent>(cameraEntity->Id);

            camera->ZoomFactor = std::clamp(camera->ZoomFactor - 0.1f, 0.0f, 1000.0f);
        }
        if (EditorAction::Action(MouseScrollAction::Down)->Started) {
            auto cameraEntity = scene->GetCurrentCamera();
            auto camera = scene->GetComponent<ECS::CameraComponent>(cameraEntity->Id);

            camera->ZoomFactor += 0.1;
        }
    }

}