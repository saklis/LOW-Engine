#include "AssetBrowserPanel.h"

#include <imgui.h>

#include "asset_browser/TexturesBrowser.h"
#include "asset_browser/SpriteSheetsBrowser.h"
#include "asset_browser/AnimationClipsBrowser.h"
#include "asset_browser/EmittersBrowser.h"
#include "asset_browser/SoundsBrowser.h"
#include "asset_browser/MusicBrowser.h"

namespace LowEngine::Panels {

    void DrawAssetBrowser(Game& game, bool& isVisible) {
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
                if (ImGui::BeginTabItem("Emitters")) {
                    DisplayEmitterBrowser(game, ImVec2(0.0f, 680.0f));
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
                isVisible = false;
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }

}