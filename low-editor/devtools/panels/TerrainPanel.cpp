#include "TerrainPanel.h"

#include <imgui.h>

#include "scene/Scene.h"
#include "log/Log.h"

namespace LowEngine::Panels {

    void DrawTerrainToolbar(const sf::Vector2u& displaySize, bool& isInTerrainEditMode) {
        ImVec2 windowPadding = ImGui::GetStyle().WindowPadding;

        float buttonWidth = 160.0f;
        float buttonHeight = 30.0f;
        float windowWidth = buttonWidth + windowPadding.x * 2;
        float windowHeight = buttonHeight + windowPadding.y * 2;

        float windowX = (displaySize.x - windowWidth) / 2;
        float windowY = 25;

        ImGui::SetNextWindowPos(ImVec2{windowX, windowY});
        ImGui::SetNextWindowSize(ImVec2{windowWidth, windowHeight});
        ImGui::Begin("TerrainToolbar", nullptr,
                     ImGuiWindowFlags_NoTitleBar |
                     ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoScrollbar |
                     ImGuiWindowFlags_NoSavedSettings);

        if (ImGui::Button("Close Terrain Editor", ImVec2{buttonWidth, buttonHeight})) {
            isInTerrainEditMode = false;
        }

        ImGui::End();
    }

    void DrawTerrainLayersList(Scene* scene, int posX, int posY, int width, int height,
                               size_t& selectedLayerIndex, int resetLayoutFrames) {
        const ImGuiCond layoutCond = (resetLayoutFrames > 0) ? ImGuiCond_Always : ImGuiCond_FirstUseEver;
        ImGui::SetNextWindowPos(ImVec2(posX, posY), layoutCond);
        ImGui::SetNextWindowSize(ImVec2(width, height), layoutCond);
        if (resetLayoutFrames > 0) {
            ImGui::SetNextWindowDockID(0, ImGuiCond_Always);
        }

        ImGui::Begin("Terrain Layers");

        if (ImGui::Button("+", ImVec2(-1, 0))) {
            scene->Terrain.AddEmptyLayer();
        }

        auto layers = scene->Terrain.GetLayers();

        for (auto& layer : *layers) {
            int currentLayerIndex = std::distance(layers->data(), &layer);
            bool isSelected = selectedLayerIndex == static_cast<size_t>(currentLayerIndex);

            ImGui::PushID(currentLayerIndex);
            if (ImGui::Selectable(layer.Name.c_str(), isSelected,
                                  ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap,
                                  ImVec2(0, 0))) {
                selectedLayerIndex = currentLayerIndex;
                _log->debug("Current Terrain layer: {}", selectedLayerIndex);
            }
            if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
                ImGui::OpenPopup("TerrainLayerContextMenu");
            }
            if (ImGui::BeginPopup("TerrainLayerContextMenu")) {
                if (ImGui::MenuItem("Delete")) {
                    selectedLayerIndex = -1;
                    scene->Terrain.DeleteLayer(currentLayerIndex);
                    _log->debug("Terrain layer {} deleted!", currentLayerIndex);
                }
                ImGui::EndPopup();
            }

            ImGui::PopID();
        }

        ImGui::End();
    }

    void DrawTerrainLayerEditor(Scene* scene, int posX, int posY, int width, int height,
                                size_t& selectedLayerIndex, int resetLayoutFrames) {
        const ImGuiCond layoutCond = (resetLayoutFrames > 0) ? ImGuiCond_Always : ImGuiCond_FirstUseEver;
        ImGui::SetNextWindowPos(ImVec2(posX, posY), layoutCond);
        ImGui::SetNextWindowSize(ImVec2(width, height), layoutCond);
        if (resetLayoutFrames > 0) {
            ImGui::SetNextWindowDockID(0, ImGuiCond_Always);
        }

        ImGui::Begin("Layer Editor");

        if (selectedLayerIndex != static_cast<size_t>(-1)) {
            auto& layer = scene->Terrain.GetLayers()->at(selectedLayerIndex);

            ImGui::Text("Name:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(-1);
            auto nameBuffer = new char[255];
            std::strncpy(nameBuffer, layer.Name.c_str(), 255);
            if (ImGui::InputText("##Name", nameBuffer, 255, ImGuiInputTextFlags_EnterReturnsTrue)) {
                layer.Name = nameBuffer;
            }

            ImGui::Text("Draw Order:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(-1);
            int drawOrder = layer.DrawOrder;
            if (ImGui::DragInt("##DrawOrder", &drawOrder)) {
                layer.DrawOrder = drawOrder;
            }
        } else {
            ImGui::Text("Select layer from layer's list...");
        }

        ImGui::End();
    }

}