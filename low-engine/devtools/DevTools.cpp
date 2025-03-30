#include "DevTools.h"

namespace LowEngine {
    bool DevTools::Initialize(sf::RenderWindow& window) {
        return ImGui::SFML::Init(window);
    }

    void DevTools::Free() {
        ImGui::SFML::Shutdown();
    }

    void DevTools::BeginReadInput() {
    }

    void DevTools::EndReadInput() {
    }

    void DevTools::ReadInput(const sf::RenderWindow& window, const std::optional<sf::Event>& event) {
        ImGui::SFML::ProcessEvent(window, *event);
    }

    void DevTools::Update(sf::RenderWindow& window, sf::Time deltaTime) {
        ImGui::SFML::Update(window, deltaTime);
    }

    void DevTools::Build(Game& game) {
        auto displaySize = game.Window.getSize();
        auto& scene = game.Scenes.GetCurrentScene();

        DisplayWorldOutliner(scene, 10, 10, 250, displaySize.y - 20);
        DisplayProperties(scene, displaySize.x - 260, 10, 250, displaySize.y - 20);
    }

    void DevTools::Render(sf::RenderWindow& window) {
        ImGui::SFML::Render(window);
    }

    void DevTools::DisplayWorldOutliner(Scene& scene, int posX, int posY, int width, int height) {
        ImGui::SetNextWindowPos(ImVec2(posX, posY), ImGuiCond_Appearing);
        ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_Appearing);
        ImGui::Begin(std::format("Scene Outliner: '{}'", scene.Name).c_str());

        auto entities = scene.GetEntities();
        for (auto& entity: *entities) {
            std::string label = std::format("[{}] {}", entity.get()->Id, entity.get()->Name);
            bool selected = _selectedEntityId != -1 && entity.get()->Id == _selectedEntityId;

            if (ImGui::Selectable(label.c_str(), selected)) {
                _selectedEntityId = entity.get()->Id;
            }
        }

        ImGui::End();
    }

    void DevTools::DisplayProperties(Scene& scene, int posX, int posY, int width, int height) {
        ImGui::SetNextWindowPos(ImVec2(posX, posY), ImGuiCond_Appearing);
        ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_Appearing);
        if (_selectedEntityId != -1) {
            auto entity = scene.GetEntity(_selectedEntityId);
            ImGui::Begin(std::format("Properties: '{}'", entity->Name).c_str());

            DisplayTransformComponentProperties(scene);
            DisplayAnimatedSpriteComponentProperties(scene);
            DisplayCameraComponentProperties(scene);
        }else {
            ImGui::Begin("Properties:");
        }

        ImGui::End();
    }

    void DevTools::DisplayTransformComponentProperties(Scene& scene) {
        auto entity = scene.GetEntity(_selectedEntityId);

        auto tc = scene.GetComponent<ECS::TransformComponent>(entity->Id);
        if (tc == nullptr) return;

        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Text("Position:");
            ImGui::SameLine();
            float position[2] = {tc->Position.x, tc->Position.y};
            if (ImGui::DragFloat2("##Position", position, 1.0f, 0, 0, "%.3f")) {
                tc->Position = {position[0], position[1]};
            }

            ImGui::Text("Rotation:");
            ImGui::SameLine();
            float rotation = tc->Rotation.asDegrees();
            if (ImGui::DragFloat("##Rotation", &rotation, 1.0f, 0, 0, "%.3f")) {
                tc->Rotation = sf::degrees(rotation);
            }

            ImGui::Text("Scale:");
            ImGui::SameLine();
            float scale[2] = {tc->Scale.x, tc->Scale.y};
            if (ImGui::DragFloat2("##Scale", scale, 0.1f, 0, 0, "%.3f")) {
                tc->Scale = {scale[0], scale[1]};
            }
        }
    }

    void DevTools::DisplayAnimatedSpriteComponentProperties(Scene& scene) {
        auto entity = scene.GetEntity(_selectedEntityId);

        auto asc = scene.GetComponent<ECS::AnimatedSpriteComponent>(entity->Id);
        if (asc == nullptr) return;

        auto clipNames = asc->Sheet->GetAnimationClipNames();
        std::vector<const char*> clipNamesCStr;
        for (const auto& name : clipNames) {
            clipNamesCStr.push_back(name.c_str());
        }

        int currentItem = 0;
        for (size_t i = 0; i < clipNames.size(); ++i) {
            if (clipNames[i] == asc->Clip->Name) {
                currentItem = i;
                break;
            }
        }

        if (ImGui::CollapsingHeader("Animated Sprite", ImGuiTreeNodeFlags_DefaultOpen)) {
            if (asc->Clip != nullptr) {
                ImGui::Text("Clip name:");
                ImGui::SameLine();
                if (ImGui::Combo("##Clip name:", &currentItem, clipNamesCStr.data(), clipNamesCStr.size())) {
                    asc->Play(clipNames[currentItem], asc->Loop);
                }
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

        if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Text("Zoom factor:");
            ImGui::SameLine();
            float zoomFactor = cc->ZoomFactor;
            if (ImGui::DragFloat("##Zoom", &zoomFactor, 0.1f, 0, 0, "%.3f")) {
                cc->ZoomFactor = zoomFactor;
            }
        }
    }

    const char* DevTools::DemangledTypeName(const std::type_index& type) {
        const char* typeName = type.name();
#if defined(__GNUC__) || defined(__clang__)
        static thread_local std::string buffer; // Thread-safe, persists across calls in the same thread
        int status;
        char* demangledName = abi::__cxa_demangle(typeName, nullptr, nullptr, &status);
        if (status == 0 && demangledName != nullptr) {
            buffer = demangledName; // Store the demangled name in the buffer
            free(demangledName); // Free the demangled name
            return buffer.c_str(); // Return the pointer to the buffer content
        }
#endif
        return typeName; // Return the original name if demangling is not supported or failed
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
}
