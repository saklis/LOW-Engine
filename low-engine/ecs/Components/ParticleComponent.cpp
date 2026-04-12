#include "ParticleComponent.h"

#include "assets/Assets.h"
#include "assets/particles/Emitter.h"
#include "assets/animation/SpriteSheet.h"
#include "memory/Memory.h"
#include "log/Log.h"
#include "utils/ColorUtils.h"

#include <algorithm>
#include <cmath>
#include <numbers>
#include <random>

namespace LowEngine::ECS {
    void ParticleComponent::Update(float deltaTime) {
        if (!_playing || EmitterId == Config::INVALID_ID)
            return;

        const auto& emitter = Assets::GetEmitter(EmitterId);
        auto* transform = _memory->GetComponent<TransformComponent>(EntityId);

        // Fetch animation clip once for all particles this frame
        const Animation::AnimationClip* clip = nullptr;
        if (emitter.Mode == Particles::Emitter::SpriteMode::AnimationClip)
            clip = &Assets::GetSpriteSheet(emitter.TextureId).GetAnimationClip(emitter.AnimClipName);

        // Update and kill dead particles
        for (int i = static_cast<int>(_particles.size()) - 1; i >= 0; i--) {
            UpdateParticle(_particles[i], emitter, clip, deltaTime);
            if (_particles[i].Lifetime >= _particles[i].MaxLifetime) {
                _particles[i] = _particles.back();
                _particles.pop_back();
            }
        }

        // Spawn new particles
        if (_particles.size() < emitter.MaxParticles) {
            _emissionAccumulator += emitter.EmissionRate * deltaTime;
            while (_emissionAccumulator >= 1.0f && _particles.size() < emitter.MaxParticles) {
                SpawnParticle(emitter, transform->Position);
                _emissionAccumulator -= 1.0f;
            }
        }

        // Stop if one-shot and all particles dead
        if (!emitter.Loop && _particles.empty() && _emissionAccumulator < 1.0f)
            _playing = false;
    }

    void ParticleComponent::DrawDirect(sf::RenderTarget& target) {
        if (_particles.empty() || EmitterId == Config::INVALID_ID)
            return;

        const auto& emitter  = Assets::GetEmitter(EmitterId);
        const auto& texture  = Assets::GetTexture(emitter.TextureId);
        _vertices.resize(_particles.size() * 6); // since Play() pre-allocates MaxParticles capacity, this should never cause any memory move/copy

        // Pre-compute static rect and clip pointer for modes that don't vary per-particle
        sf::IntRect staticRect;
        const Animation::AnimationClip* drawClip = nullptr;
        switch (emitter.Mode) {
            case Particles::Emitter::SpriteMode::SpriteSheetFrame: {
                auto& sheet = Assets::GetSpriteSheet(emitter.TextureId);
                staticRect = sheet.GetTile(emitter.FrameIndex % sheet.FrameCount.x,
                                           emitter.FrameIndex / sheet.FrameCount.x);
                break;
            }
            case Particles::Emitter::SpriteMode::AnimationClip:
                drawClip = &Assets::GetSpriteSheet(emitter.TextureId).GetAnimationClip(emitter.AnimClipName);
                break;
            case Particles::Emitter::SpriteMode::FullTexture:
            default:
                staticRect = sf::IntRect({0, 0}, static_cast<sf::Vector2i>(texture.getSize()));
                break;
        }

        for (std::size_t i = 0; i < _particles.size(); i++) {
            const auto& p = _particles[i];

            // Resolve per-particle rect
            sf::IntRect rect = staticRect;
            if (drawClip != nullptr) {
                if (p.currentFrame >= drawClip->Frames.size()) {
                    _log->error("ParticleComponent: currentFrame {} out of bounds for clip '{}' ({} frames).",
                                p.currentFrame, emitter.AnimClipName, drawClip->Frames.size());
                    rect = drawClip->Frames.empty() ? staticRect : drawClip->Frames.back();
                } else {
                    rect = drawClip->Frames[p.currentFrame];
                }
            }

            // Quad half-size in world space
            const float hw = rect.size.x * 0.5f * p.Scale;
            const float hh = rect.size.y * 0.5f * p.Scale;

            // Rotation
            const float rad  = p.Rotation * (std::numbers::pi_v<float> / 180.0f);
            const float cosR = std::cos(rad);
            const float sinR = std::sin(rad);

            auto rotate = [&](float lx, float ly) -> sf::Vector2f {
                return { p.Position.x + lx * cosR - ly * sinR,
                         p.Position.y + lx * sinR + ly * cosR };
            };

            const sf::Vector2f tl = rotate(-hw, -hh);
            const sf::Vector2f tr = rotate( hw, -hh);
            const sf::Vector2f bl = rotate(-hw,  hh);
            const sf::Vector2f br = rotate( hw,  hh);

            // Texture coords
            const float tx0 = static_cast<float>(rect.position.x);
            const float ty0 = static_cast<float>(rect.position.y);
            const float tx1 = static_cast<float>(rect.position.x + rect.size.x);
            const float ty1 = static_cast<float>(rect.position.y + rect.size.y);

            // Color
            const float t = std::clamp(p.Lifetime / p.MaxLifetime, 0.0f, 1.0f);
            const sf::Color color = Utils::LerpColor(emitter.ColorStart, emitter.ColorEnd, t);

            // Write 6 vertices (2 triangles: TL-TR-BL, TR-BR-BL)
            const std::size_t v = i * 6;
            _vertices[v + 0] = { tl, color, { tx0, ty0 } };
            _vertices[v + 1] = { tr, color, { tx1, ty0 } };
            _vertices[v + 2] = { bl, color, { tx0, ty1 } };
            _vertices[v + 3] = { tr, color, { tx1, ty0 } };
            _vertices[v + 4] = { br, color, { tx1, ty1 } };
            _vertices[v + 5] = { bl, color, { tx0, ty1 } };
        }

        sf::RenderStates states;
        states.texture = &texture;
        target.draw(_vertices, states);
    }

    nlohmann::ordered_json ParticleComponent::SerializeToJSON() {
        nlohmann::ordered_json json = IComponent::SerializeToJSON();
        json["EmitterAlias"] = Assets::GetEmitterAlias(EmitterId);
        json["DrawOrder"]    = DrawOrder;
        return json;
    }

    bool ParticleComponent::DeserializeFromJSON(const nlohmann::ordered_json& jsonData) {
        if (!IComponent::DeserializeFromJSON(jsonData))
            return false;

        if (jsonData.contains("EmitterAlias")) {
            const auto alias = jsonData["EmitterAlias"].get<std::string>();
            if (!Assets::EmitterExists(alias)) {
                _log->error("ParticleComponent deserialization failed: emitter '{}' not found.", alias);
                return false;
            }
            EmitterId = Assets::GetEmitterId(alias);
        } else {
            _log->error("ParticleComponent deserialization failed: missing 'EmitterAlias' field.");
            return false;
        }

        DrawOrder = jsonData.value("DrawOrder", 0);
        return true;
    }

    void ParticleComponent::Play() {
        if (EmitterId == Config::INVALID_ID) {
            _log->warn("ParticleComponent::Play called but no Emitter is assigned.");
            return;
        }

        const auto& emitter = Assets::GetEmitter(EmitterId);
        auto* transform = _memory->GetComponent<TransformComponent>(EntityId);

        _playing = true;
        _emissionAccumulator = 0.0f;
        _particles.clear();
        _particles.reserve(emitter.MaxParticles);
        _vertices = sf::VertexArray(sf::PrimitiveType::Triangles, emitter.MaxParticles * 6);

        for (std::size_t i = 0; i < emitter.BurstCount; i++) {
            if (_particles.size() < emitter.MaxParticles)
                SpawnParticle(emitter, transform->Position);
        }
    }

    void ParticleComponent::Stop() {
        _playing = false;
        _emissionAccumulator = 0.0f;
    }

    void ParticleComponent::Burst() {
        if (!_playing) {
            _log->warn("ParticleComponent::Burst called but the system is not playing.");
            return;
        }

        if (EmitterId == Config::INVALID_ID) {
            _log->warn("ParticleComponent::Burst called but no Emitter is assigned.");
            return;
        }

        const auto& emitter = Assets::GetEmitter(EmitterId);
        auto* transform = _memory->GetComponent<TransformComponent>(EntityId);

        for (std::size_t i = 0; i < emitter.BurstCount; i++) {
            if (_particles.size() < emitter.MaxParticles)
                SpawnParticle(emitter, transform->Position);
        }
    }

    void ParticleComponent::Clear() {
        _playing = false;
        _emissionAccumulator = 0.0f;
        _particles.clear();
    }

    void ParticleComponent::SpawnParticle(const Particles::Emitter& emitter, sf::Vector2f origin) {
        static std::mt19937 rng(std::random_device{}());

        Particles::Particle particle;

        // Lifetime
        particle.MaxLifetime = (emitter.LifetimeMin < emitter.LifetimeMax)
            ? std::uniform_real_distribution<float>(emitter.LifetimeMin, emitter.LifetimeMax)(rng)
            : emitter.LifetimeMin;
        particle.Lifetime = 0.0f;

        // Position — random point within SpawnRadius
        sf::Vector2f spawnPos = origin;
        if (emitter.SpawnRadius > 0.0f) {
            const float angle = std::uniform_real_distribution<float>(0.0f, 2.0f * std::numbers::pi_v<float>)(rng);
            const float radius = std::sqrt(std::uniform_real_distribution<float>(0.0f, 1.0f)(rng))
                                 * emitter.SpawnRadius;
            spawnPos.x += std::cos(angle) * radius;
            spawnPos.y += std::sin(angle) * radius;
        }
        particle.Position = spawnPos;

        // Velocity — random direction within SpreadAngle cone around Direction
        const float baseAngle = std::atan2(emitter.Direction.y, emitter.Direction.x);
        const float halfSpread = (emitter.SpreadAngle * 0.5f) * (std::numbers::pi_v<float> / 180.0f);
        const float finalAngle = (halfSpread > 0.0f)
            ? baseAngle + std::uniform_real_distribution<float>(-halfSpread, halfSpread)(rng)
            : baseAngle;
        const float speed = (emitter.SpeedMin < emitter.SpeedMax)
            ? std::uniform_real_distribution<float>(emitter.SpeedMin, emitter.SpeedMax)(rng)
            : emitter.SpeedMin;
        particle.Velocity = sf::Vector2f(std::cos(finalAngle) * speed, std::sin(finalAngle) * speed);

        // Visuals
        particle.Scale = emitter.ScaleStart;
        particle.Rotation = 0.0f;
        particle.RotationSpeed = (emitter.RotationSpeedMin < emitter.RotationSpeedMax)
            ? std::uniform_real_distribution<float>(emitter.RotationSpeedMin, emitter.RotationSpeedMax)(rng)
            : emitter.RotationSpeedMin;

        // Animation
        particle.currentFrame = 0;
        particle.frameTimer = 0.0f;

        _particles.push_back(particle);
    }

    void ParticleComponent::UpdateParticle(Particles::Particle& particle, const Particles::Emitter& emitter,
                                           const Animation::AnimationClip* clip, float deltaTime) {
        particle.Lifetime += deltaTime;

        // Physics
        particle.Velocity  += emitter.Gravity * deltaTime;
        particle.Position  += particle.Velocity * deltaTime;
        particle.Rotation  += particle.RotationSpeed * deltaTime;

        // Normalized life progress [0, 1]
        const float t = std::clamp(particle.Lifetime / particle.MaxLifetime, 0.0f, 1.0f);

        // Interpolate scale over lifetime
        particle.Scale = emitter.ScaleStart + (emitter.ScaleEnd - emitter.ScaleStart) * t;

        // Animation frame advance
        if (clip != nullptr) {
            particle.frameTimer += deltaTime;
            if (particle.frameTimer >= clip->FrameDuration) {
                particle.frameTimer -= clip->FrameDuration;
                if (clip->FrameCount > 0)
                    particle.currentFrame = (particle.currentFrame + 1) % clip->FrameCount;
            }
        }
    }
}
