#include "TerrainPanel.h"

#include <imgui.h>

#include "devtools/Action.h"
#include "devtools/controls/TexturePicker.h"
#include "ecs/Entity.h"
#include "ecs/components/CameraComponent.h"
#include "Game.h"
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

        for (auto& layer: *layers) {
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
                                size_t& selectedLayerIndex, sf::IntRect& selectedTerrainLayerTile,
                                TileMap::TileType& selectedTileType, std::string& selectedAnimClipName,
                                int resetLayoutFrames) {
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

            ImGui::Text("Is Visible? ");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(-1);
            ImGui::Checkbox("##IsVisible", &layer.IsVisible);

            ImGui::Text("Draw Order:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(-1);
            int drawOrder = layer.GetDrawOrder();
            if (ImGui::DragInt("##DrawOrder", &drawOrder)) {
                layer.SetDrawOrder(drawOrder);
            }

            ImGui::Text("Texture:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(-1);
            auto textureAlias = Assets::GetTextureAlias(layer.GetTextureId());
            if (ImGui::Button(textureAlias.c_str())) {
                ImGui::OpenPopup("SelectTexture");
            }
            std::size_t newTextureId = 0;
            if (Controls::TexturePickerPopup("SelectTexture", newTextureId)) {
                layer.SetTextureId(newTextureId);
                selectedTerrainLayerTile.position.x = 0;
                selectedTerrainLayerTile.position.y = 0;
                selectedTerrainLayerTile.size.x = layer.TileSize.x;
                selectedTerrainLayerTile.size.y = layer.TileSize.y;
                selectedTileType = TileMap::TileType::Static;
                selectedAnimClipName = "";
            }

            if (Assets::HasSpriteSheet(layer.GetTextureId())) {
                auto& texture = Assets::GetTexture(layer.GetTextureId());
                auto& spriteSheet = Assets::GetSpriteSheet(layer.GetTextureId());

                layer.TileSize = spriteSheet.FrameSize;

                ImGui::Text("Tile size: ");
                ImGui::SameLine();
                ImGui::Text("%llu x %llu pixels", layer.TileSize.x, layer.TileSize.y);

                // mode toggle
                int tileMode = (selectedTileType == TileMap::TileType::Animated) ? 1 : 0;
                if (ImGui::RadioButton("Static", &tileMode, 0)) selectedTileType = TileMap::TileType::Static;
                ImGui::SameLine();
                if (ImGui::RadioButton("Animated", &tileMode, 1)) selectedTileType = TileMap::TileType::Animated;

                if (selectedTileType == TileMap::TileType::Animated) {
                    auto clipNames = spriteSheet.GetAnimationClipNames();
                    if (clipNames.empty()) {
                        ImGui::Text("No animation clips defined...");
                    } else {
                        ImGui::Text("Animation:");
                        ImGui::SameLine();
                        ImGui::SetNextItemWidth(-1);
                        const char* preview = selectedAnimClipName.empty()
                                                  ? "(select clip)"
                                                  : selectedAnimClipName.c_str();
                        if (ImGui::BeginCombo("##AnimClips", preview)) {
                            for (auto& name: clipNames) {
                                bool isSelected = selectedAnimClipName == name;
                                if (ImGui::Selectable(name.c_str(), isSelected)) {
                                    selectedAnimClipName = name;
                                    if (isSelected) ImGui::SetItemDefaultFocus();
                                }
                            }
                            ImGui::EndCombo();
                        }
                    }
                } else {
                    ImGui::Text("Tiles:");
                    // display texture
                    auto textureSize = texture.getSize();
                    float availWidth = ImGui::GetContentRegionAvail().x;
                    ImVec2 maxPreviewSize{availWidth, availWidth};
                    float aspectRatio = static_cast<float>(textureSize.x) / static_cast<float>(textureSize.y);
                    ImVec2 previewSize;
                    if (aspectRatio > 1.0f) {
                        previewSize = ImVec2(maxPreviewSize.x, maxPreviewSize.y / aspectRatio);
                    } else {
                        previewSize = ImVec2(maxPreviewSize.x * aspectRatio, maxPreviewSize.y);
                    }
                    ImGui::Image(texture.getNativeHandle(), previewSize);

                    ImVec2 imagePos = ImGui::GetItemRectMin();
                    ImVec2 imageSize = ImGui::GetItemRectSize();

                    if (spriteSheet.FrameCount.x > 0 && spriteSheet.FrameCount.y > 0) {
                        // draw grid to visualize sprite sheet's tiles.
                        float cellWidth = imageSize.x / spriteSheet.FrameCount.x;
                        float cellHeight = imageSize.y / spriteSheet.FrameCount.y;
                        ImDrawList* drawList = ImGui::GetWindowDrawList();

                        for (size_t i = 1; i < spriteSheet.FrameCount.x; ++i) {
                            float x = imagePos.x + i * cellWidth;
                            drawList->AddLine(ImVec2(x, imagePos.y), ImVec2(x, imagePos.y + imageSize.y),
                                              IM_COL32(0, 255, 255, 255), 1.0f);
                        }
                        for (size_t j = 1; j < spriteSheet.FrameCount.y; ++j) {
                            float y = imagePos.y + j * cellHeight;
                            drawList->AddLine(ImVec2(imagePos.x, y), ImVec2(imagePos.x + imageSize.x, y),
                                              IM_COL32(0, 255, 255, 255), 1.0f);
                        }

                        // click on tile to select it
                        ImVec2 mousePos = ImGui::GetMousePos();
                        bool mouseOverImage = mousePos.x >= imagePos.x && mousePos.x < imagePos.x + imageSize.x &&
                                              mousePos.y >= imagePos.y && mousePos.y < imagePos.y + imageSize.y;

                        if (mouseOverImage && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                            int col = static_cast<int>((mousePos.x - imagePos.x) / cellWidth);
                            int row = static_cast<int>((mousePos.y - imagePos.y) / cellHeight);
                            int fw = static_cast<int>(spriteSheet.FrameSize.x);
                            int fh = static_cast<int>(spriteSheet.FrameSize.y);
                            selectedTerrainLayerTile = sf::IntRect(
                                {col * fw, row * fh},
                                {fw, fh}
                            );
                        }

                        // highlight selected tile
                        if (selectedTerrainLayerTile.size.x > 0 && selectedTerrainLayerTile.size.y > 0) {
                            int selCol = selectedTerrainLayerTile.position.x / static_cast<int>(spriteSheet.FrameSize.
                                             x);
                            int selRow = selectedTerrainLayerTile.position.y / static_cast<int>(spriteSheet.FrameSize.
                                             y);
                            ImVec2 highlightMin(imagePos.x + selCol * cellWidth, imagePos.y + selRow * cellHeight);
                            ImVec2 highlightMax(highlightMin.x + cellWidth, highlightMin.y + cellHeight);
                            drawList->AddRectFilled(highlightMin, highlightMax, IM_COL32(0, 255, 255, 80));
                            drawList->AddRect(highlightMin, highlightMax, IM_COL32(0, 255, 255, 255), 0.0f, 0, 2.0f);
                        }
                    }
                }
            }
        } else {
            ImGui::Text("Select layer from layer's list...");
        }

        ImGui::End();
    }

    void Terrain2GameSceneInteraction(Game& game, Scene* scene, size_t selectedLayerIndex,
                                      sf::IntRect selectedTerrainLayerTile, TileMap::TileType& selectedTileType,
                                      std::string& selectedAnimClipName) {
        if (EditorAction::Action(MouseAction::Primary)->Started) {
            if (selectedLayerIndex == static_cast<size_t>(-1)) return; // skip if no layer is selected

            auto& layer = scene->Terrain.GetLayers()->at(selectedLayerIndex);
            if (layer.TileSize.x == 0 || layer.TileSize.y == 0) return; // guard from layers without spritesheet

            if (selectedTileType == TileMap::TileType::Animated && selectedAnimClipName.empty()) return;
            // for animated - must have clip selected
            if (selectedTileType == TileMap::TileType::Static && selectedTerrainLayerTile.size.x == 0 &&
                selectedTerrainLayerTile.size.y == 0)
                return; // for static - must have rect selected

            sf::Vector2i mouseScreenPos = EditorAction::Action(MouseAction::Primary)->MouseScreenPosition;
            sf::Vector2f worldPos = game.Window.mapPixelToCoords(mouseScreenPos);

            sf::Vector2i tileGridPos = {
                static_cast<int>(floor(worldPos.x / layer.TileSize.x)),
                static_cast<int>(floor(worldPos.y / layer.TileSize.y))
            };

            switch (selectedTileType) {
                case TileMap::TileType::Static:
                    layer.AddTile(tileGridPos, selectedTerrainLayerTile);
                    break;
                case TileMap::TileType::Animated:
                    layer.AddTile(tileGridPos, selectedAnimClipName);
                    break;
            }
        }
    }

    void DrawTerrainWorldGrid(sf::RenderWindow& window, Scene* scene, size_t selectedLayerIndex) {
        auto& layer = scene->Terrain.GetLayers()->at(selectedLayerIndex);
        if (layer.TileSize.x == 0 || layer.TileSize.y == 0) return;

        sf::View view = window.getView();
        sf::Vector2f center = view.getCenter();
        sf::Vector2f halfSize = view.getSize() * 0.5f;

        float left = center.x - halfSize.x;
        float right = center.x + halfSize.x;
        float top = center.y - halfSize.y;
        float bottom = center.y + halfSize.y;

        float tw = static_cast<float>(layer.TileSize.x);
        float th = static_cast<float>(layer.TileSize.y);

        float startX = std::floor(left / tw) * tw;
        float startY = std::floor(top / th) * th;

        sf::Color gridColor(255, 255, 255, 50);
        std::vector<sf::Vertex> lines;

        for (float x = startX; x <= right; x += tw) {
            lines.push_back(sf::Vertex{sf::Vector2f(x, top), gridColor});
            lines.push_back(sf::Vertex{sf::Vector2f(x, bottom), gridColor});
        }
        for (float y = startY; y <= bottom; y += th) {
            lines.push_back(sf::Vertex{sf::Vector2f(left, y), gridColor});
            lines.push_back(sf::Vertex{sf::Vector2f(right, y), gridColor});
        }

        window.draw(lines.data(), lines.size(), sf::PrimitiveType::Lines);
    }

}
