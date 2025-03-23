#include "DevTools.h"

namespace LowEngine {
    void DevTools::Display(Game& game) {
        auto displaySize = game.Window.getSize();

        auto& scene = game.Scenes.GetCurrentScene();

        // Statistics - maybe FPS and stuff
        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Appearing);
        ImGui::SetNextWindowSize(ImVec2(displaySize.x - 20, 50), ImGuiCond_Appearing);
        ImGui::Begin("Statistics");
        ImGui::Text("FPS: %f", 1.0f / game.DeltaTime.asSeconds());
        ImGui::End();

        // World Outliner - current scene's hierarchy
        ImGui::SetNextWindowPos(ImVec2(10, 70), ImGuiCond_Appearing);
        ImGui::SetNextWindowSize(ImVec2(250, displaySize.y - 80), ImGuiCond_Appearing);
        ImGui::Begin(std::format("Scene Outliner: '{}'", scene.Name).c_str());
        DisplayWorldOutliner(scene);
        ImGui::End();

        // Properties - properties of currently selected entity
        ImGui::SetNextWindowPos(ImVec2(displaySize.x - 250 - 10, 70), ImGuiCond_Appearing);
        ImGui::SetNextWindowSize(ImVec2(250, displaySize.y - 80), ImGuiCond_Appearing);
        if (_selectedEntity != nullptr) {
            ImGui::Begin(std::format("Properties: '{}'", _selectedEntity->Name).c_str());
        }else {
            ImGui::Begin("Properties:");
        }
        DisplayProperties(scene);
        ImGui::End();
    }

    void DevTools::DisplayWorldOutliner(Scene& scene) {
        auto& entites = scene.GetEntities();

        for (auto& entity: entites) {
            if (ImGui::Selectable(std::format("[{}] {} ", entity.Id, entity.Name).c_str())) {
                _selectedEntity = &entity;
            }
        }
    }

    void DevTools::DisplayProperties(Scene& scene) {
        if (_selectedEntity == nullptr) return;

        auto& entity = *_selectedEntity;
        auto types = entity.GetComponentTypes();

        for (int i = types.size() - 1; i >= 0; i--) {
            auto type = types[i];

            std::string demangledName = DemangledTypeName(type);
            auto pos = demangledName.find_last_of("::");
            std::string label = (pos != std::string::npos) ? demangledName.substr(pos + 1) : demangledName;
            label = InsertSpaces(label);

            if (ImGui::CollapsingHeader(label.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
                if (std::type_index(typeid(ECS::TransformComponent)) == type) {
                    DisplayTransformComponentProperties(scene, entity);
                }

                if (std::type_index(typeid(ECS::SpriteComponent)) == type) {

                }

                if (std::type_index(typeid(ECS::AnimatedSpriteComponent)) == type) {
                    DisplayAnimatedSpriteComponentProperties(scene, entity);
                }
            }
        }
    }

    void DevTools::DisplayTransformComponentProperties(Scene& scene, ECS::Entity& entity) {
        auto tc = scene.GetComponent<ECS::TransformComponent>(entity.Id);
        if (tc == nullptr) return;

        ImGui::Text("Position:");
        ImGui::SameLine();
        float position[2] = { tc->Position.x, tc->Position.y };
        if (ImGui::DragFloat2("##Position", position, 1.0f, 0, 0, "%.3f")) {
            tc->Position = { position[0], position[1] };
        }

        ImGui::Text("Rotation:");
        ImGui::SameLine();
        float rotation = tc->Rotation.asDegrees();
        if (ImGui::DragFloat("##Rotation", &rotation, 1.0f, 0, 0, "%.3f")) {
            tc->Rotation = sf::degrees(rotation);
        }

        ImGui::Text("Scale:");
        ImGui::SameLine();
        float scale[2] = { tc->Scale.x, tc->Scale.y };
        if (ImGui::DragFloat2("##Scale", scale, 0.1f, 0, 0, "%.3f")) {
            tc->Scale = { scale[0], scale[1] };
        }
    }

    void DevTools::DisplayAnimatedSpriteComponentProperties(Scene& scene, ECS::Entity& entity) {
        auto asc = scene.GetComponent<ECS::AnimatedSpriteComponent>(entity.Id);
        if (asc == nullptr) return;

        if (asc->Clip != nullptr) {
            ImGui::Text("Start frame: %i", asc->Clip->StartFrame);
            ImGui::Text("End frame: %i", asc->Clip->EndFrame);
            ImGui::Text("Frame count: %i", asc->Clip->FrameCount);
            ImGui::Text("Frame duration: %.3f", asc->Clip->FrameDuration);
        } else {}
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
        for (std::size_t i = 0; i < str.size(); ++i)
        {
            if (i > 0 && std::isupper(static_cast<unsigned char>(str[i])))
            {
                result.push_back(' ');
            }
            result.push_back(str[i]);
        }
        return result;
    }
}
