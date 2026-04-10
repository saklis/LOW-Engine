#pragma once
#include "EngineConfig.h"
#include "TransformComponent.h"
#include "assets/particles/Particle.h"
#include "ecs/IComponent.h"

namespace LowEngine::ECS {
    class ParticleComponent : public IComponent<ParticleComponent, TransformComponent> {
    public:
        std::size_t EmitterId = Config::INVALID_ID;
        int DrawOrder = 0;

        explicit ParticleComponent(Memory::Memory* memory)
            : IComponent(memory) {
        }

        ParticleComponent(Memory::Memory* memory, ParticleComponent const* other)
            : IComponent(memory, other), EmitterId(other->EmitterId), DrawOrder(other->DrawOrder) {
        }

        ~ParticleComponent() override = default;

        void Initialize() override {}

        void Update(float deltaTime) override;

        void DrawDirect(sf::RenderTarget& target) override;

        nlohmann::ordered_json SerializeToJSON() override;

        bool DeserializeFromJSON(const nlohmann::ordered_json& jsonData) override;

        void Play();

        void Stop();

        void Burst();

    protected:
        bool _playing = false;
        std::vector<Particles::Particle> _particles;
        float _emissionAccumulator = 0.0f;
        sf::VertexArray _vertices;

        void SpawnParticle(const Particles::Emitter& emitter, sf::Vector2f origin);

        void UpdateParticle(Particles::Particle& particle, const Particles::Emitter& emitter, float deltaTime);
    };
}
