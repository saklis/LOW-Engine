#include "EmittersBrowser.h"

#include <filesystem>
#include <unordered_map>

#include <imgui.h>
#include <SFML/Graphics/RenderTexture.hpp>

#include "Game.h"
#include "EngineConfig.h"
#include "assets/Assets.h"
#include "assets/animation/SpriteSheet.h"
#include "assets/particles/Emitter.h"
#include "devtools/Assets.h"
#include "ecs/Components/ParticleComponent.h"
#include "log/Log.h"

namespace LowEngine::Panels {

    // Thumbnail cache: emitter alias -> loaded sf::Texture (nullptr = no thumbnail on disk)
    static std::unordered_map<std::string, sf::Texture*> _thumbnailCache;

    static sf::Texture* GetThumbnail(const std::string& alias) {
        auto it = _thumbnailCache.find(alias);
        if (it != _thumbnailCache.end())
            return it->second;

        const auto& emitter = Assets::GetEmitter(alias);
        std::filesystem::path thumbPath = emitter.Path;
        thumbPath.replace_extension(".thumb.png");

        sf::Texture* tex = nullptr;
        if (std::filesystem::exists(thumbPath)) {
            tex = new sf::Texture();
            if (!tex->loadFromFile(thumbPath.string())) {
                delete tex;
                tex = nullptr;
                _log->warn("EmittersBrowser: failed to load thumbnail '{}'", thumbPath.string());
            }
        }

        _thumbnailCache[alias] = tex;
        return tex;
    }

    void InvalidateEmitterThumbnail(const std::string& alias) {
        auto it = _thumbnailCache.find(alias);
        if (it != _thumbnailCache.end()) {
            delete it->second;
            _thumbnailCache.erase(it);
        }
    }

    static void ShowCreateEmitterDialog(Game& game, bool& isOpen) {
        static char aliasBuffer[255]       = "";
        static std::string selectedTexture = "";
        static std::string createError     = "";

        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImVec2 dialogSize{500.0f, 220.0f};
        ImGui::SetNextWindowPos(ImVec2(center.x - dialogSize.x / 2, center.y - dialogSize.y / 2),
                                ImGuiCond_Appearing);
        ImGui::SetNextWindowSize(dialogSize, ImGuiCond_Appearing);

        if (ImGui::Begin("Create Emitter", &isOpen,
                         ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize)) {
            ImGui::Text("Alias:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(350.0f);
            ImGui::InputText("##EmitterAlias", aliasBuffer, sizeof(aliasBuffer));

            ImGui::Text("Texture:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(350.0f);
            const char* comboPreview = selectedTexture.empty() ? "Select texture..." : selectedTexture.c_str();
            if (ImGui::BeginCombo("##EmitterTexture", comboPreview)) {
                auto textureAliases = Assets::GetTextureAliases();
                std::ranges::sort(textureAliases);
                for (const auto& texAlias : textureAliases) {
                    if (texAlias == Config::DEFAULT_TEXTURE_ALIAS) continue;
                    if (ImGui::Selectable(texAlias.c_str(), selectedTexture == texAlias))
                        selectedTexture = texAlias;
                }
                ImGui::EndCombo();
            }

            ImGui::Separator();

            if (!createError.empty())
                ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "%s", createError.c_str());

            const bool canCreate = std::strlen(aliasBuffer) > 0
                                && !selectedTexture.empty()
                                && !Assets::EmitterExists(aliasBuffer);

            ImGui::BeginDisabled(!canCreate);
            if (ImGui::Button("Create", ImVec2(230.0f, 0.0f))) {
                Particles::Emitter emitter;
                emitter.TextureId = Assets::GetTextureId(selectedTexture);

                if (Assets::SaveEmitter(emitter, game.ProjectDirectory, aliasBuffer)) {
                    if (Assets::LoadEmitter(aliasBuffer, emitter.Path.string()) != Config::INVALID_ID) {
                        _log->info("Emitter '{}' created.", aliasBuffer);
                        aliasBuffer[0]  = '\0';
                        selectedTexture = "";
                        createError.clear();
                        isOpen = false;
                    } else {
                        createError = "Failed to load emitter after saving.";
                    }
                } else {
                    createError = "Failed to save emitter to disk.";
                }
            }
            ImGui::EndDisabled();

            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(230.0f, 0.0f))) {
                aliasBuffer[0]  = '\0';
                selectedTexture = "";
                createError.clear();
                isOpen = false;
            }

            ImGui::End();
        }
    }

    // -------------------------------------------------------------------------
    // Emitter Editor
    // -------------------------------------------------------------------------

    static void ShowEmitterEditor(Game& game, const std::string& alias, bool& isOpen) {
        static sf::RenderTexture previewRT;
        static ECS::ParticleComponent* previewParticle = nullptr;
        static std::string lastAlias;

        const ImVec2 editorSize      { 1200.0f, 800.0f };
        const ImVec2 previewRTSize   {  512.0f, 512.0f };
        const ImVec2 previewAreaSize {  760.0f, 760.0f };
        const float  propertiesWidth {  400.0f };

        // Re-init preview when alias changes
        if (alias != lastAlias) {
            lastAlias = alias;
            if (!previewRT.resize({ static_cast<unsigned>(previewRTSize.x),
                                    static_cast<unsigned>(previewRTSize.y) }))
                _log->error("EmitterEditor: failed to resize preview render texture.");
            delete previewParticle;
            previewParticle = new ECS::ParticleComponent(nullptr);
            previewParticle->EmitterId = Assets::GetEmitterId(alias);
            previewParticle->SetPositionOverride({ previewRTSize.x * 0.5f, previewRTSize.y * 0.5f });
            previewParticle->Play();
        }

        // Tick preview — respects emitter.Loop; use "Restart Preview" to replay one-shot effects
        if (previewParticle) {
            previewParticle->Update(game.DeltaTime.asSeconds());
            previewRT.clear(sf::Color(30, 30, 30, 255));
            previewParticle->DrawDirect(previewRT);
            previewRT.display();
        }

        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos({ center.x - editorSize.x * 0.5f, center.y - editorSize.y * 0.5f },
                                ImGuiCond_Appearing);
        ImGui::SetNextWindowSize(editorSize, ImGuiCond_Appearing);

        if (!ImGui::Begin("Emitter Editor", &isOpen, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize)) {
            ImGui::End();
            return;
        }

        auto& emitter = Assets::GetEmitter(alias);

        // --- Left: preview ---
        ImGui::BeginChild("EmitterPreviewArea", previewAreaSize);

        // Centre the 512x512 preview inside the 760x760 area
        const float padX = (previewAreaSize.x - previewRTSize.x) * 0.5f;
        const float padY = (previewAreaSize.y - previewRTSize.y) * 0.5f - 30.0f; // leave room for buttons
        ImGui::Dummy(ImVec2(0.0f, padY));
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + padX);
        ImGui::Image(previewRT.getTexture().getNativeHandle(), previewRTSize,
                     ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));

        ImGui::Dummy(ImVec2(0.0f, 8.0f));
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + padX);
        if (ImGui::Button("Play",  ImVec2(118.0f, 0.0f))) {
            if (previewParticle) previewParticle->Play();
        }
        ImGui::SameLine();
        if (ImGui::Button("Stop",  ImVec2(118.0f, 0.0f))) {
            if (previewParticle) previewParticle->Stop();
        }
        ImGui::SameLine();
        if (ImGui::Button("Burst", ImVec2(118.0f, 0.0f))) {
            if (previewParticle) previewParticle->Burst();
        }
        ImGui::SameLine();
        if (ImGui::Button("Clear", ImVec2(118.0f, 0.0f))) {
            if (previewParticle) previewParticle->Clear();
        }

        ImGui::Dummy(ImVec2(0.0f, 4.0f));
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + padX);
        if (ImGui::Button("Make Thumbnail", ImVec2(484.0f, 0.0f))) {
            std::filesystem::path thumbPath = emitter.Path;
            thumbPath.replace_extension(".thumb.png");
            if (previewRT.getTexture().copyToImage().saveToFile(thumbPath.string())) {
                InvalidateEmitterThumbnail(alias);
                _log->info("Thumbnail saved to '{}'", thumbPath.string());
            } else {
                _log->error("Failed to save thumbnail to '{}'", thumbPath.string());
            }
        }

        ImGui::EndChild();

        ImGui::SameLine();

        // --- Right: properties ---
        ImGui::BeginChild("EmitterProperties", ImVec2(propertiesWidth, 0.0f),
                          ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar);

        ImGui::Text("Emitter: %s", alias.c_str());
        ImGui::Separator();

        // Sprite
        ImGui::SeparatorText("Sprite");

        {
            const std::string currentTexAlias = Assets::GetTextureAlias(emitter.TextureId);
            if (ImGui::BeginCombo("Texture", currentTexAlias.c_str())) {
                auto texAliases = Assets::GetTextureAliases();
                std::ranges::sort(texAliases);
                for (const auto& t : texAliases) {
                    if (t == Config::DEFAULT_TEXTURE_ALIAS) continue;
                    if (ImGui::Selectable(t.c_str(), t == currentTexAlias)) {
                        emitter.TextureId = Assets::GetTextureId(t);
                        if (previewParticle) previewParticle->Play();
                    }
                }
                ImGui::EndCombo();
            }
        }

        {
            const char* modeNames[] = { "Full Texture", "Sprite Sheet Frame", "Animation Clip" };
            int modeInt = static_cast<int>(emitter.Mode);
            if (ImGui::Combo("Mode", &modeInt, modeNames, 3)) {
                emitter.Mode = static_cast<Particles::Emitter::SpriteMode>(modeInt);
                if (previewParticle) previewParticle->Play();
            }
        }

        if (emitter.Mode == Particles::Emitter::SpriteMode::SpriteSheetFrame) {
            int frameIdx = static_cast<int>(emitter.FrameIndex);
            if (ImGui::DragInt("Frame Index", &frameIdx, 1.0f, 0, 9999))
                emitter.FrameIndex = static_cast<std::size_t>(std::max(0, frameIdx));
        }

        if (emitter.Mode == Particles::Emitter::SpriteMode::AnimationClip) {
            if (Assets::HasSpriteSheet(emitter.TextureId)) {
                auto& sheet = Assets::GetSpriteSheet(emitter.TextureId);
                auto clipNames = sheet.GetAnimationClipNames();
                std::ranges::sort(clipNames);
                const char* preview = emitter.AnimClipName.empty() ? "Select clip..." : emitter.AnimClipName.c_str();
                if (ImGui::BeginCombo("Anim Clip", preview)) {
                    for (int i = 0; i < static_cast<int>(clipNames.size()); i++) {
                        if (ImGui::Selectable(clipNames[i].c_str(), clipNames[i] == emitter.AnimClipName)) {
                            emitter.AnimClipName = clipNames[i];
                            if (previewParticle) previewParticle->Play();
                        }
                    }
                    ImGui::EndCombo();
                }
            } else {
                ImGui::TextDisabled("No sprite sheet on selected texture.");
            }
        }

        // Spawn
        ImGui::SeparatorText("Spawn");
        ImGui::DragFloat("Emission Rate",  &emitter.EmissionRate,  0.5f,  0.0f, 10000.0f, "%.1f /s");
        {
            int burst = static_cast<int>(emitter.BurstCount);
            if (ImGui::DragInt("Burst Count", &burst, 1.0f, 0, 10000))
                emitter.BurstCount = static_cast<std::size_t>(std::max(0, burst));
        }
        {
            int maxP = static_cast<int>(emitter.MaxParticles);
            if (ImGui::DragInt("Max Particles", &maxP, 1.0f, 1, 100000)) {
                emitter.MaxParticles = static_cast<std::size_t>(std::max(1, maxP));
                if (previewParticle) previewParticle->Play(); // resize vertex buffer
            }
        }
        ImGui::Checkbox("Loop", &emitter.Loop);
        ImGui::DragFloat("Spawn Radius", &emitter.SpawnRadius, 1.0f, 0.0f, 10000.0f, "%.1f px");

        // Lifetime
        ImGui::SeparatorText("Lifetime");
        ImGui::DragFloat("Min##Lifetime", &emitter.LifetimeMin, 0.05f, 0.0f, 3600.0f, "%.2f s");
        ImGui::DragFloat("Max##Lifetime", &emitter.LifetimeMax, 0.05f, 0.0f, 3600.0f, "%.2f s");

        // Velocity
        ImGui::SeparatorText("Velocity");
        ImGui::DragFloat("Speed Min",    &emitter.SpeedMin,    1.0f,    0.0f, 100000.0f, "%.1f px/s");
        ImGui::DragFloat("Speed Max",    &emitter.SpeedMax,    1.0f,    0.0f, 100000.0f, "%.1f px/s");
        ImGui::DragFloat("Spread Angle", &emitter.SpreadAngle, 1.0f,    0.0f, 360.0f,    "%.1f deg");
        ImGui::DragFloat2("Direction",   &emitter.Direction.x, 0.01f, -1.0f, 1.0f);

        // Rotation
        ImGui::SeparatorText("Rotation");
        ImGui::DragFloat("Rot Speed Min", &emitter.RotationSpeedMin, 1.0f, -36000.0f, 36000.0f, "%.1f deg/s");
        ImGui::DragFloat("Rot Speed Max", &emitter.RotationSpeedMax, 1.0f, -36000.0f, 36000.0f, "%.1f deg/s");

        // Visuals
        ImGui::SeparatorText("Visuals");
        {
            float cs[4] = { emitter.ColorStart.r / 255.0f, emitter.ColorStart.g / 255.0f,
                            emitter.ColorStart.b / 255.0f, emitter.ColorStart.a / 255.0f };
            if (ImGui::ColorEdit4("Color Start", cs)) {
                emitter.ColorStart.r = static_cast<uint8_t>(cs[0] * 255);
                emitter.ColorStart.g = static_cast<uint8_t>(cs[1] * 255);
                emitter.ColorStart.b = static_cast<uint8_t>(cs[2] * 255);
                emitter.ColorStart.a = static_cast<uint8_t>(cs[3] * 255);
            }
        }
        {
            float ce[4] = { emitter.ColorEnd.r / 255.0f, emitter.ColorEnd.g / 255.0f,
                            emitter.ColorEnd.b / 255.0f, emitter.ColorEnd.a / 255.0f };
            if (ImGui::ColorEdit4("Color End", ce)) {
                emitter.ColorEnd.r = static_cast<uint8_t>(ce[0] * 255);
                emitter.ColorEnd.g = static_cast<uint8_t>(ce[1] * 255);
                emitter.ColorEnd.b = static_cast<uint8_t>(ce[2] * 255);
                emitter.ColorEnd.a = static_cast<uint8_t>(ce[3] * 255);
            }
        }
        ImGui::DragFloat("Scale Start", &emitter.ScaleStart, 0.01f, 0.0f, 100.0f);
        ImGui::DragFloat("Scale End",   &emitter.ScaleEnd,   0.01f, 0.0f, 100.0f);

        // Physics
        ImGui::SeparatorText("Physics");
        ImGui::DragFloat2("Gravity", &emitter.Gravity.x, 1.0f, -100000.0f, 100000.0f, "%.1f px/s²");

        // Rendering
        ImGui::SeparatorText("Rendering");
        ImGui::DragInt("Draw Order", &emitter.DrawOrder, 1.0f);

        ImGui::Separator();
        ImGui::Dummy(ImVec2(0.0f, 8.0f));

        if (ImGui::Button("Save", ImVec2(propertiesWidth * 0.5f - 4.0f, 0.0f))) {
            const std::string fileName = emitter.Path.stem().string();
            if (Assets::SaveEmitter(emitter, game.ProjectDirectory, fileName))
                _log->info("Emitter '{}' saved.", alias);
        }
        ImGui::SameLine();
        if (ImGui::Button("Delete", ImVec2(propertiesWidth * 0.5f - 4.0f, 0.0f)))
            ImGui::OpenPopup("Delete Emitter?");

        if (ImGui::BeginPopupModal("Delete Emitter?", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Delete emitter '%s'?", alias.c_str());
            ImGui::Text("The .emitter file and its thumbnail will be removed from disk.");
            ImGui::Separator();

            if (ImGui::Button("Delete", ImVec2(180.0f, 0.0f))) {
                const auto emitterPath = emitter.Path;
                std::filesystem::path thumbPath = emitterPath;
                thumbPath.replace_extension(".thumb.png");

                InvalidateEmitterThumbnail(alias);

                std::error_code ec;
                std::filesystem::remove(emitterPath, ec);
                std::filesystem::remove(thumbPath, ec);

                Assets::UnloadEmitter(alias);

                ImGui::CloseCurrentPopup();
                isOpen = false;
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(180.0f, 0.0f)))
                ImGui::CloseCurrentPopup();

            ImGui::EndPopup();
        }

        ImGui::EndChild();
        ImGui::End();
    }

    // -------------------------------------------------------------------------
    // Emitter Browser
    // -------------------------------------------------------------------------

    void DisplayEmitterBrowser(Game& game, ImVec2 size) {
        static std::string selectedEmitterAlias;
        static bool showEmitterEditor        = false;
        static bool showCreateEmitterDialog  = false;

        const int columnsCount  = 8;
        const int thumbnailSize = 124;

        ImGui::BeginChild("EmitterBrowser", size);

        if (ImGui::Button("Create Emitter"))
            showCreateEmitterDialog = true;

        auto aliases = Assets::GetEmitterAliases();
        std::ranges::sort(aliases);

        ImGui::Columns(columnsCount, "EmitterBrowserColumnsLayout", false);

        for (const auto& alias : aliases) {
            sf::Texture* thumb = GetThumbnail(alias);
            unsigned int nativeHandle = thumb
                ? thumb->getNativeHandle()
                : EditorAssets::ParticleIconTexture()->getNativeHandle();

            if (ImGui::ImageButton(alias.c_str(), nativeHandle,
                                   ImVec2(thumbnailSize, thumbnailSize),
                                   ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f))) {
                selectedEmitterAlias = alias;
                showEmitterEditor = true;
            }

            ImGui::TextWrapped("%s", alias.c_str());
            ImGui::NextColumn();
        }

        ImGui::Columns(1);
        ImGui::EndChild();

        if (showCreateEmitterDialog)
            ShowCreateEmitterDialog(game, showCreateEmitterDialog);

        if (showEmitterEditor && !selectedEmitterAlias.empty())
            ShowEmitterEditor(game, selectedEmitterAlias, showEmitterEditor);
    }

}