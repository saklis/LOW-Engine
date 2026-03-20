#include "InputEditorPanel.h"

#include <imgui.h>

#include "Game.h"
#include "devtools/Assets.h"
#include "log/Log.h"

namespace LowEngine::Panels {

    void DrawInputEditor(Game& game, bool& isVisible, std::string& actionBeingBound) {
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
                static char nameBuffer[255] = "New Action";
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

                for (auto& [actionName, action] : *actions) {
                    ImGui::TableNextRow();

                    ImGui::TableNextColumn();
                    ImGui::Text("%s", actionName.c_str());

                    ImGui::TableNextColumn();
                    ImGui::Text("%s", action.Type == Input::ActionType::Keyboard ? "Keyboard" : "Mouse");

                    ImGui::TableNextColumn();
                    if (actionBeingBound == actionName) {
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.2f, 1.0f));
                        ImGui::Text("Press any key...");
                        ImGui::PopStyleColor();

                        for (int i = 0; i < 5; i++) {
                            if (ImGui::IsMouseClicked(i)) {
                                action.Type = Input::ActionType::Mouse;
                                action.MouseButton = static_cast<sf::Mouse::Button>(i);
                                actionBeingBound = "";
                                break;
                            }
                        }

                        for (int k = 0; k < sf::Keyboard::KeyCount; k++) {
                            if (sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(k))) {
                                action.Type = Input::ActionType::Keyboard;
                                action.Key = static_cast<sf::Keyboard::Key>(k);
                                actionBeingBound = "";
                                break;
                            }
                        }
                    } else {
                        std::string inputName;
                        if (action.Type == Input::ActionType::Keyboard) {
                            inputName = game.Input.GetKeyName(action.Key);
                        } else { inputName = game.Input.GetMouseButtonName(action.MouseButton); }

                        ImGui::PushID(actionName.c_str());
                        if (ImGui::Button(inputName.c_str(), ImVec2(-1, 0))) {
                            actionBeingBound = actionName;
                        }
                        ImGui::PopID();
                    }

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
                    if (ImGui::ImageButton(("##Delete" + actionName).c_str(),
                                           EditorAssets::TrashTexture()->getNativeHandle(),
                                           ImVec2{15.0f, 15.0f}, ImVec2{0.0f, 0.0f}, ImVec2{1.0f, 1.0f})) {
                        game.Input.RemoveAction(actionName.data());
                        break;
                    }
                }

                ImGui::EndTable();
            }

            ImGui::EndChild();

            ImGui::Separator();

            if (ImGui::Button("Close")) {
                isVisible = false;
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }

}