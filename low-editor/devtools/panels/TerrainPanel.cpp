#include "TerrainPanel.h"

#include <algorithm>
#include <cstring>

#include <imgui.h>

#include "devtools/Action.h"
#include "devtools/controls/TexturePicker.h"
#include "ecs/Entity.h"
#include "ecs/components/CameraComponent.h"
#include "Game.h"
#include "devtools/tools/TerrainBrushManager.h"
#include "log/Log.h"
#include "scene/Scene.h"

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

	void DrawTerrainProperties(Scene* scene, int posX, int posY, int width, int height,
	                           size_t& selectedLayerIndex, int resetLayoutFrames) {
		const ImGuiCond layoutCond = (resetLayoutFrames > 0) ? ImGuiCond_Always : ImGuiCond_FirstUseEver;
		ImGui::SetNextWindowPos(ImVec2(posX, posY), layoutCond);
		ImGui::SetNextWindowSize(ImVec2(width, height), layoutCond);
		if (resetLayoutFrames > 0) {
			ImGui::SetNextWindowDockID(0, ImGuiCond_Always);
		}

		ImGui::Begin("Terrain Properties");

		if (ImGui::CollapsingHeader("Layers", ImGuiTreeNodeFlags_DefaultOpen)) {
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
		}

		ImGui::End();
	}

	void DrawTerrainNavigationEditor(Scene* scene, int posX, int posY, int width, int height,
	                                 bool& navOverlayVisible, int resetLayoutFrames) {
		const ImGuiCond layoutCond = (resetLayoutFrames > 0) ? ImGuiCond_Always : ImGuiCond_FirstUseEver;
		ImGui::SetNextWindowPos(ImVec2(posX, posY), layoutCond);
		ImGui::SetNextWindowSize(ImVec2(width, height), layoutCond);
		if (resetLayoutFrames > 0) {
			ImGui::SetNextWindowDockID(0, ImGuiCond_Always);
		}

		ImGui::Begin("Navigation Editor");

		auto& bounds = scene->Terrain.NavBounds;

		int boundsPos[2] = {bounds.position.x, bounds.position.y};
		ImGui::Text("Bounds origin (cells):");
		ImGui::SetNextItemWidth(-1);
		if (ImGui::InputInt2("##NavBoundsPos", boundsPos)) {
			bounds.position.x = boundsPos[0];
			bounds.position.y = boundsPos[1];
			scene->Terrain.MarkNavigationDirty();
		}

		int boundsSize[2] = {bounds.size.x, bounds.size.y};
		ImGui::Text("Bounds size (cells):");
		ImGui::SetNextItemWidth(-1);
		if (ImGui::InputInt2("##NavBoundsSize", boundsSize)) {
			bounds.size.x = std::max(0, boundsSize[0]);
			bounds.size.y = std::max(0, boundsSize[1]);
			scene->Terrain.MarkNavigationDirty();
		}

		if (ImGui::Button("Bake Now", ImVec2(-1, 0))) {
			scene->Terrain.BakeNavGrid();
		}

		ImGui::Checkbox("Show Nav Overlay", &navOverlayVisible);
		auto& nav = scene->Terrain.GetNavGrid();
		if (nav.Width == 0 || nav.Height == 0) {
			ImGui::TextDisabled("Grid empty — set bounds size > 0.");
		} else {
			bool anyContrib = false;
			bool anyTileSize = false;
			for (auto& layer : *scene->Terrain.GetLayers()) {
				if (layer.ContributesToNavigation) {
					anyContrib = true;
					if (layer.TileSize.x != 0 && layer.TileSize.y != 0) anyTileSize = true;
				}
			}
			if (!anyContrib) ImGui::TextDisabled("No layer marked Contributes to Nav.");
			else if (!anyTileSize) ImGui::TextDisabled("Nav layer has no TileSize — assign a sprite-sheet texture.");
			else ImGui::Text("Nav: %zux%zu cells", nav.Width, nav.Height);
		}


		ImGui::End();
	}

	void DrawTerrainBrushManager(Scene* scene, int posX, int posY, int width, int height, size_t& selectedLayerIndex,
	                             Tools::TerrainBrushManager& brushManager,
	                             sf::IntRect& selectedTerrainLayerTile, TileMap::TileType& selectedTileType,
	                             std::string& selectedAnimClipName,
	                             std::uint8_t& brushTraversalMask, std::uint8_t& brushEntryCost,
	                             bool& brushHasCollision, size_t& selectedBrushIndex, int resetLayoutFrames) {
		const ImGuiCond layoutCond = (resetLayoutFrames > 0) ? ImGuiCond_Always : ImGuiCond_FirstUseEver;
		ImGui::SetNextWindowPos(ImVec2(posX, posY), layoutCond);
		ImGui::SetNextWindowSize(ImVec2(width, height), layoutCond);
		if (resetLayoutFrames > 0) {
			ImGui::SetNextWindowDockID(0, ImGuiCond_Always);
		}

		ImGui::Begin("Terrain Brushes");

		if (selectedLayerIndex != static_cast<size_t>(-1)) {
			auto& layer = scene->Terrain.GetLayers()->at(selectedLayerIndex);
			auto* brushes = brushManager.GetBrushes(layer.Id);

			ImGui::Text("Layer: %s", layer.Name.c_str());
			ImGui::Separator();
			if (ImGui::Button("Make Brush from current settings", ImVec2(-1, 0))) {
				Tools::TerrainBrush brush;
				brush.Name = std::format("Brush {}", brushes ? brushes->size() + 1 : 1);
				brush.Type = selectedTileType;
				brush.SpriteRect = selectedTerrainLayerTile;
				brush.AnimationClipName = selectedAnimClipName;
				brush.TraversalMask = brushTraversalMask;
				brush.EntryCost = brushEntryCost;
				brush.HasCollision = brushHasCollision;

				brushManager.AddBrush(layer.Id, brush);
				brushes = brushManager.GetBrushes(layer.Id);
				selectedBrushIndex = brushes ? brushes->size() - 1 : static_cast<size_t>(-1);

				_log->info("New brush '{}' saved.", brush.Name);
			}

			if (brushes != nullptr && !brushes->empty()) {
				// unselect if out-of-scope. Likely caused by brush deletion
				if (selectedBrushIndex >= brushes->size()) {
					selectedBrushIndex = static_cast<size_t>(-1);
				}

				ImGui::BeginChild("TerrainBrushList", ImVec2(0, 140), true);
				for (int i = 0; i < brushes->size(); ++i) {
					auto& brush = brushes->at(i);
					bool isSelected = selectedBrushIndex == i;

					ImGui::PushID(i);
					if (ImGui::Selectable(brush.Name.c_str(), isSelected, ImGuiSelectableFlags_SpanAllColumns)) {
						selectedBrushIndex = i;
						ApplyBrushAsCurrentPaint(brush, selectedTerrainLayerTile, selectedTileType,
						                         selectedAnimClipName,
						                         brushTraversalMask, brushEntryCost, brushHasCollision);
					}
					ImGui::PopID();
				}
				ImGui::EndChild();

				// brush preview
				if (selectedBrushIndex != static_cast<size_t>(-1)) {
					auto& selectedBrush = brushes->at(selectedBrushIndex);

					char nameBuffer[255];
					std::strncpy(nameBuffer, selectedBrush.Name.c_str(), sizeof(nameBuffer));
					nameBuffer[sizeof(nameBuffer) - 1] = '\0';
					ImGui::Text("Name:");
					ImGui::SameLine();
					ImGui::SetNextItemWidth(-1);
					if (ImGui::InputText("##TerrainBrushName", nameBuffer, sizeof(nameBuffer),
					                     ImGuiInputTextFlags_EnterReturnsTrue)) {
						selectedBrush.Name = nameBuffer;
					}

					ImGui::Text("Type: %s", selectedBrush.Type == TileMap::TileType::Animated ? "Animated" : "Static");
					if (selectedBrush.Type == TileMap::TileType::Animated) {
						ImGui::Text("Animation: %s", selectedBrush.AnimationClipName.c_str());
					} else {
						ImGui::Text("Sprite Rect: %d, %d, %d, %d",
						            selectedBrush.SpriteRect.position.x,
						            selectedBrush.SpriteRect.position.y,
						            selectedBrush.SpriteRect.size.x,
						            selectedBrush.SpriteRect.size.y);
					}
					ImGui::Text("Traversal Mask: %u", selectedBrush.TraversalMask);
					ImGui::Text("Entry Cost: %u", selectedBrush.EntryCost);
					ImGui::Text("Collision: %s", selectedBrush.HasCollision ? "Yes" : "No");

					if (ImGui::Button("Delete Brush", ImVec2(-1, 0))) {
						if (brushManager.DeleteBrush(layer.Id, selectedBrushIndex)) {
							selectedBrushIndex = static_cast<size_t>(-1);
						}
					}
				}
			} else {
				selectedBrushIndex = static_cast<size_t>(-1);
				ImGui::Text("No brushes for this layer");
			}
		} else {
			ImGui::Text("Select layer from layer's list...");
		}

		ImGui::End();
	}

	void DrawTerrainLayerEditor(Scene* scene, int posX, int posY, int width, int height,
	                            size_t& selectedLayerIndex, sf::IntRect& selectedTerrainLayerTile,
	                            TileMap::TileType& selectedTileType, std::string& selectedAnimClipName,
	                            std::uint8_t& brushTraversalMask, std::uint8_t& brushEntryCost,
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
			char nameBuffer[255];
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

			if (ImGui::CollapsingHeader("Navigation", ImGuiTreeNodeFlags_DefaultOpen)) {
				ImGui::Text("Contributes to Navigation? ");
				ImGui::SameLine();
				ImGui::SetNextItemWidth(-1);
				if (ImGui::Checkbox("##ContributesToNav", &layer.ContributesToNavigation)) {
					scene->Terrain.MarkNavigationDirty();
				}

				ImGui::Text("Navigation brush:");
				bool walkFlag = (brushTraversalMask & TileMap::Traversal::Walk) != 0;
				bool swimFlag = (brushTraversalMask & TileMap::Traversal::Swim) != 0;
				bool flyFlag = (brushTraversalMask & TileMap::Traversal::Fly) != 0;
				if (ImGui::Checkbox("Walk", &walkFlag)) {
					brushTraversalMask = walkFlag
						                     ? (brushTraversalMask | TileMap::Traversal::Walk)
						                     : (brushTraversalMask & ~TileMap::Traversal::Walk);
				}
				ImGui::SameLine();
				if (ImGui::Checkbox("Swim", &swimFlag)) {
					brushTraversalMask = swimFlag
						                     ? (brushTraversalMask | TileMap::Traversal::Swim)
						                     : (brushTraversalMask & ~TileMap::Traversal::Swim);
				}
				ImGui::SameLine();
				if (ImGui::Checkbox("Fly", &flyFlag)) {
					brushTraversalMask = flyFlag
						                     ? (brushTraversalMask | TileMap::Traversal::Fly)
						                     : (brushTraversalMask & ~TileMap::Traversal::Fly);
				}

				ImGui::Text("Entry Cost:");
				ImGui::SameLine();
				ImGui::SetNextItemWidth(-1);
				int costValue = brushEntryCost;
				if (ImGui::DragInt("##BrushEntryCost", &costValue, 1.0f, 0, 255)) {
					brushEntryCost = static_cast<std::uint8_t>(std::clamp(costValue, 0, 255));
				}

				ImGui::Separator();
			}

			if (ImGui::CollapsingHeader("Collision", ImGuiTreeNodeFlags_DefaultOpen)) {
				ImGui::Text("Contributes to Collision? ");
				ImGui::SameLine();
				ImGui::SetNextItemWidth(-1);
				if (ImGui::Checkbox("##ContributesToCol", &layer.ContributesToCollision)) {
					scene->Terrain.MarkCollisionsDirty();
				}

				ImGui::Separator();
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
				scene->Terrain.MarkNavigationDirty();
				scene->Terrain.MarkCollisionsDirty();
			}

			if (Assets::HasSpriteSheet(layer.GetTextureId())) {
				auto& texture = Assets::GetTexture(layer.GetTextureId());
				auto& spriteSheet = Assets::GetSpriteSheet(layer.GetTextureId());

				layer.TileSize = spriteSheet.FrameSize;

				ImGui::Text("Tile size: ");
				ImGui::SameLine();
				ImGui::Text("%zu x %zu pixels", layer.TileSize.x, layer.TileSize.y);

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
							for (auto& name : clipNames) {
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
	                                  std::string& selectedAnimClipName,
	                                  std::uint8_t brushTraversalMask, std::uint8_t brushEntryCost, bool brushHasCollision) {
		static size_t popupLayerIndex = static_cast<size_t>(-1);
		static sf::Vector2i popupCellCoords;
		static bool popupHasTile = false;

		auto resolveClickedCell = [&](sf::Vector2i mouseScreenPos, sf::Vector2i& cellCoords) -> bool {
			if (selectedLayerIndex == static_cast<size_t>(-1)) return false;

			auto& layer = scene->Terrain.GetLayers()->at(selectedLayerIndex);
			if (layer.TileSize.x == 0 || layer.TileSize.y == 0) return false;

			sf::Vector2f worldPos = game.Window.mapPixelToCoords(mouseScreenPos);
			cellCoords = {
				static_cast<int>(floor(worldPos.x / layer.TileSize.x)),
				static_cast<int>(floor(worldPos.y / layer.TileSize.y))
			};
			return true;
		};

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

			if (auto* tile = const_cast<TileMap::Tile*>(layer.FindTile(tileGridPos))) {
				tile->TraversalMask = brushTraversalMask;
				tile->EntryCost = brushEntryCost;
				tile->HasCollision = brushHasCollision;
			}

			scene->Terrain.MarkNavigationDirty();
			scene->Terrain.MarkCollisionsDirty();
		}

		if (EditorAction::Action(MouseAction::Secondary)->Started) {
			sf::Vector2i tileGridPos;
			if (resolveClickedCell(EditorAction::Action(MouseAction::Secondary)->MouseScreenPosition, tileGridPos)) {
				auto& layer = scene->Terrain.GetLayers()->at(selectedLayerIndex);
				popupLayerIndex = selectedLayerIndex;
				popupCellCoords = tileGridPos;
				popupHasTile = layer.FindTile(tileGridPos) != nullptr;
				ImGui::OpenPopup("TerrainTilePropertiesPopup");
			}
		}

		ImGui::SetNextWindowSizeConstraints(ImVec2(260.0f, 0.0f), ImVec2(360.0f, 600.0f));
		if (ImGui::BeginPopup("TerrainTilePropertiesPopup")) {
			if (popupLayerIndex == static_cast<size_t>(-1) || popupLayerIndex >= scene->Terrain.GetLayers()->size()) {
				ImGui::TextDisabled("Layer no longer exists.");
				if (ImGui::Button("Close", ImVec2(-1, 0))) {
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
				return;
			}

			auto& layer = scene->Terrain.GetLayers()->at(popupLayerIndex);
			auto* tile = layer.FindTile(popupCellCoords);
			popupHasTile = tile != nullptr;

			ImGui::Text("Cell: %d, %d", popupCellCoords.x, popupCellCoords.y);
			ImGui::Text("Layer: %s", layer.Name.c_str());
			ImGui::Separator();

			if (!popupHasTile) {
				ImGui::TextDisabled("No tile at this cell.");
				if (ImGui::Button("Close", ImVec2(-1, 0))) {
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
				return;
			}

			ImGui::Text("Type: %s", tile->Type == TileMap::TileType::Animated ? "Animated" : "Static");
			if (tile->Type == TileMap::TileType::Animated) {
				ImGui::Text("Animation: %s", tile->AnimationClipName.c_str());
			} else {
				ImGui::Text("Sprite Rect: %d, %d, %d, %d",
				            tile->SpriteRect.position.x,
				            tile->SpriteRect.position.y,
				            tile->SpriteRect.size.x,
				            tile->SpriteRect.size.y);
			}

			ImGui::Separator();
			ImGui::Text("Top-down Navigation");
			bool walkFlag = (tile->TraversalMask & TileMap::Traversal::Walk) != 0;
			bool swimFlag = (tile->TraversalMask & TileMap::Traversal::Swim) != 0;
			bool flyFlag = (tile->TraversalMask & TileMap::Traversal::Fly) != 0;
			bool navChanged = false;
			if (ImGui::Checkbox("Walk##TileNav", &walkFlag)) {
				tile->TraversalMask = walkFlag
					                      ? (tile->TraversalMask | TileMap::Traversal::Walk)
					                      : (tile->TraversalMask & ~TileMap::Traversal::Walk);
				navChanged = true;
			}
			ImGui::SameLine();
			if (ImGui::Checkbox("Swim##TileNav", &swimFlag)) {
				tile->TraversalMask = swimFlag
					                      ? (tile->TraversalMask | TileMap::Traversal::Swim)
					                      : (tile->TraversalMask & ~TileMap::Traversal::Swim);
				navChanged = true;
			}
			ImGui::SameLine();
			if (ImGui::Checkbox("Fly##TileNav", &flyFlag)) {
				tile->TraversalMask = flyFlag
					                      ? (tile->TraversalMask | TileMap::Traversal::Fly)
					                      : (tile->TraversalMask & ~TileMap::Traversal::Fly);
				navChanged = true;
			}

			int entryCost = tile->EntryCost;
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
			if (ImGui::DragInt("Entry Cost##TileEntryCost", &entryCost, 1.0f, 0, 255)) {
				tile->EntryCost = static_cast<std::uint8_t>(std::clamp(entryCost, 0, 255));
				navChanged = true;
			}
			if (navChanged) {
				scene->Terrain.MarkNavigationDirty();
			}

			ImGui::Separator();
			ImGui::Text("Collision");
			bool hasCollision = tile->HasCollision;
			if (ImGui::Checkbox("Has Collision##TileCollision", &hasCollision)) {
				tile->HasCollision = hasCollision;
				scene->Terrain.MarkCollisionsDirty();
			}

			ImGui::Separator();
			if (ImGui::Button("Delete Tile", ImVec2(-1, 0))) {
				if (layer.DeleteTile(popupCellCoords)) {
					scene->Terrain.MarkNavigationDirty();
					scene->Terrain.MarkCollisionsDirty();
				}
				popupHasTile = false;
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}

	void DrawNavOverlay(sf::RenderWindow& window, Scene* scene, size_t selectedLayerIndex) {
		auto& nav = scene->Terrain.GetNavGrid();
		const auto& bounds = scene->Terrain.NavBounds;

		// Pick a cell size for visualization: selected layer's TileSize first, then
		// first ContributesToNav layer's. Bail if nothing resolves (can't draw).
		sf::Vector2<std::size_t> tileSize{0, 0};
		if (selectedLayerIndex != static_cast<size_t>(-1)) {
			auto& selLayer = scene->Terrain.GetLayers()->at(selectedLayerIndex);
			tileSize = selLayer.TileSize;
		}
		if (tileSize.x == 0 || tileSize.y == 0) {
			for (auto& layer : *scene->Terrain.GetLayers()) {
				if (layer.ContributesToNavigation && layer.TileSize.x != 0 && layer.TileSize.y != 0) {
					tileSize = layer.TileSize;
					break;
				}
			}
		}
		if (tileSize.x == 0 || tileSize.y == 0) return;

		const float tw = static_cast<float>(tileSize.x);
		const float th = static_cast<float>(tileSize.y);

		std::vector<sf::Vertex> quads;
		quads.reserve(nav.Cells.size() * 6);

		for (std::size_t y = 0; y < nav.Height; ++y) {
			for (std::size_t x = 0; x < nav.Width; ++x) {
				const auto& cell = nav.Cells[x + y * nav.Width];

				std::uint8_t mask = 0;
				if (cell.IsWalkable) mask |= TileMap::Traversal::Walk;
				if (cell.IsSwimmable) mask |= TileMap::Traversal::Swim;
				if (cell.IsFlyable) mask |= TileMap::Traversal::Fly;

				sf::Color color;
				switch (mask) {
				case 0: color = sf::Color(255, 40, 40, 90);
					break; // blocked
				case TileMap::Traversal::Walk: color = sf::Color(40, 220, 40, 70);
					break;
				case TileMap::Traversal::Swim: color = sf::Color(40, 80, 220, 70);
					break;
				case TileMap::Traversal::Fly: color = sf::Color(40, 220, 220, 70);
					break;
				default: color = sf::Color(220, 220, 40, 70);
					break; // multi
				}

				const float wx = static_cast<float>(bounds.position.x + static_cast<int>(x)) * tw;
				const float wy = static_cast<float>(bounds.position.y + static_cast<int>(y)) * th;

				quads.push_back(sf::Vertex{sf::Vector2f(wx, wy), color});
				quads.push_back(sf::Vertex{sf::Vector2f(wx + tw, wy), color});
				quads.push_back(sf::Vertex{sf::Vector2f(wx, wy + th), color});
				quads.push_back(sf::Vertex{sf::Vector2f(wx + tw, wy), color});
				quads.push_back(sf::Vertex{sf::Vector2f(wx + tw, wy + th), color});
				quads.push_back(sf::Vertex{sf::Vector2f(wx, wy + th), color});
			}
		}

		if (!quads.empty()) {
			window.draw(quads.data(), quads.size(), sf::PrimitiveType::Triangles);
		}

		// NavBounds outline
		if (bounds.size.x > 0 && bounds.size.y > 0) {
			const float x0 = static_cast<float>(bounds.position.x) * tw;
			const float y0 = static_cast<float>(bounds.position.y) * th;
			const float x1 = x0 + static_cast<float>(bounds.size.x) * tw;
			const float y1 = y0 + static_cast<float>(bounds.size.y) * th;
			const sf::Color boundsColor(255, 255, 255, 200);
			std::vector<sf::Vertex> outline = {
				sf::Vertex{sf::Vector2f(x0, y0), boundsColor}, sf::Vertex{sf::Vector2f(x1, y0), boundsColor},
				sf::Vertex{sf::Vector2f(x1, y0), boundsColor}, sf::Vertex{sf::Vector2f(x1, y1), boundsColor},
				sf::Vertex{sf::Vector2f(x1, y1), boundsColor}, sf::Vertex{sf::Vector2f(x0, y1), boundsColor},
				sf::Vertex{sf::Vector2f(x0, y1), boundsColor}, sf::Vertex{sf::Vector2f(x0, y0), boundsColor}
			};
			window.draw(outline.data(), outline.size(), sf::PrimitiveType::Lines);
		}
	}

	void ApplyBrushAsCurrentPaint(const Tools::TerrainBrush& brush, sf::IntRect& selectedTerrainLayerTile,
	                              TileMap::TileType& selectedTileType, std::string& selectedAnimClipName,
	                              std::uint8_t& brushTraversalMask,
	                              std::uint8_t& brushEntryCost, bool& brushHasCollision) {
		selectedTileType = brush.Type;
		selectedTerrainLayerTile = brush.SpriteRect;
		selectedAnimClipName = brush.AnimationClipName;
		brushTraversalMask = brush.TraversalMask;
		brushEntryCost = brush.EntryCost;
		brushHasCollision = brush.HasCollision;
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
