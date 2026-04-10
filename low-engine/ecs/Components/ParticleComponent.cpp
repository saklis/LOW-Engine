#include "ParticleComponent.h"

#include "assets/Assets.h"
#include "assets/particles/Emitter.h"
#include "memory/Memory.h"
#include "log/Log.h"

#include <cmath>
#include <numbers>
#include <random>

namespace LowEngine::ECS {
    void ParticleComponent::Update(float deltaTime) {
    }

    void ParticleComponent::DrawDirect(sf::RenderTarget& target) {
    }

    nlohmann::ordered_json ParticleComponent::SerializeToJSON() {
        return IComponent<ParticleComponent, TransformComponent>::SerializeToJSON();
    }

    bool ParticleComponent::DeserializeFromJSON(const nlohmann::ordered_json& jsonData) {
        return IComponent<ParticleComponent, TransformComponent>::DeserializeFromJSON(jsonData);
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
    }

    void ParticleComponent::Burst() {
    }

    void ParticleComponent::SpawnParticle(const Particles::Emitter& emitter, sf::Vector2f origin) {
        static std::mt19937 rng(std::random_device{}());

        Particles::Particle particle;

        // Lifetime
        particle.MaxLifetime = std::uniform_real_distribution<float>(emitter.LifetimeMin, emitter.LifetimeMax)(rng);
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
        const float finalAngle = baseAngle + std::uniform_real_distribution<float>(-halfSpread, halfSpread)(rng);
        const float speed = std::uniform_real_distribution<float>(emitter.SpeedMin, emitter.SpeedMax)(rng);
        particle.Velocity = sf::Vector2f(std::cos(finalAngle) * speed, std::sin(finalAngle) * speed);

        // Visuals
        particle.Scale = emitter.ScaleStart;
        particle.Rotation = 0.0f;
        particle.RotationSpeed = std::uniform_real_distribution<float>(emitter.RotationSpeedMin,
                                                                       emitter.RotationSpeedMax)(rng);

        // Animation
        particle.currentFrame = 0;
        particle.frameTimer = 0.0f;

        _particles.push_back(particle);
    }

    void ParticleComponent::UpdateParticle(Particles::Particle& particle, const Particles::Emitter& emitter,
                                           float deltaTime) {
    }
}
