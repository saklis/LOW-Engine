#pragma once
#include <optional>
#include "EngineConfig.h"
#include "TransformComponent.h"
#include "assets/particles/Particle.h"
#include "ecs/IComponent.h"

namespace LowEngine::Animation { struct AnimationClip; }

namespace LowEngine::ECS {
    /**
     * @brief Component that drives a particle system at runtime.
     *
     * Reads configuration from an Emitter asset and simulates live Particle instances.
     * Renders using sf::VertexArray via DrawDirect, bypassing the sprite pipeline.
     *
     * Depends on TransformComponent.
     */
    class ParticleComponent : public IComponent<ParticleComponent, TransformComponent> {
    public:
        /**
         * @brief ID of the Emitter asset that defines this system's behaviour.
         *
         * Must be set before calling Play(). Defaults to Config::INVALID_ID.
         */
        std::size_t EmitterId = Config::INVALID_ID;

        /**
         * @brief Render order relative to other direct-draw components.
         *
         * Lower values are drawn first (appear behind).
         */
        int DrawOrder = 0;

        /**
         * @brief Offset added to the spawn origin every time a particle is spawned.
         *
         * Applied on top of the entity's transform position (or the position override
         * when one is active). Useful for emitters that should not originate exactly
         * at the entity's pivot point.
         */
        sf::Vector2f PositionOffset = sf::Vector2f(0.0f, 0.0f);

        explicit ParticleComponent(Memory::Memory* memory)
            : IComponent(memory) {
        }

        /**
         * @brief Copy constructor. Only copies configuration fields (EmitterId, DrawOrder).
         *
         * Runtime state (_particles, _vertices, _playing, _emissionAccumulator) starts fresh.
         */
        ParticleComponent(Memory::Memory* memory, ParticleComponent const* other)
            : IComponent(memory, other), EmitterId(other->EmitterId), DrawOrder(other->DrawOrder) {
        }

        ~ParticleComponent() override = default;

        void Initialize() override {}

        void Update(float deltaTime) override;

        void DrawDirect(sf::RenderTarget& target) override;

        nlohmann::ordered_json SerializeToJSON() override;

        bool DeserializeFromJSON(const nlohmann::ordered_json& jsonData) override;

        /**
         * @brief Starts emission. Clears any existing particles and fires an initial burst.
         *
         * Safe to call while already playing — restarts the system from scratch.
         */
        void Play();

        /**
         * @brief Stops spawning new particles. Existing particles live out their lifetime.
         */
        void Stop();

        /**
         * @brief Fires BurstCount particles immediately without affecting continuous emission.
         *
         * Requires the system to be playing (Play() must have been called first).
         * Respects MaxParticles — no particles are spawned beyond the cap.
         */
        void Burst();

        /**
         * @brief Immediately stops emission and removes all live particles.
         */
        void Clear();

        /**
         * @brief Overrides the spawn origin with a fixed world-space position.
         *
         * When set, the entity's TransformComponent is ignored for spawn origin
         * resolution. PositionOffset is still applied on top of the override.
         * Intended for editor previews and other contexts where no entity exists.
         * @param position World-space position to use as the spawn origin.
         */
        void SetPositionOverride(sf::Vector2f position) { _positionOverride = position; }

        /**
         * @brief Removes the position override, restoring normal TransformComponent lookup.
         */
        void ClearPositionOverride() { _positionOverride = std::nullopt; }

        /**
         * @brief Returns whether the system is currently active (playing or draining).
         */
        [[nodiscard]] bool IsPlaying() const { return _playing; }

    protected:
        /**
         * @brief Whether the system is currently emitting particles.
         */
        bool _playing = false;

        /**
         * @brief Pool of currently live particles.
         */
        std::vector<Particles::Particle> _particles;

        /**
         * @brief Accumulated fractional particle count for continuous emission.
         *
         * Carries the sub-integer remainder between frames so emission rate
         * stays accurate regardless of framerate.
         */
        float _emissionAccumulator = 0.0f;

        /**
         * @brief Total particles spawned since the last Play() call.
         *
         * Used by non-looping emitters to enforce the MaxParticles lifetime cap:
         * once this reaches MaxParticles, no further particles are spawned regardless
         * of how many live particles remain. Resets to zero on Play() and Clear().
         */
        uint32_t _totalSpawned = 0;

        /**
         * @brief Set to true the first time a particle is spawned after Play().
         *
         * Prevents the one-shot stop condition from firing on the very first
         * frame before the emission accumulator has had time to reach 1.0.
         */
        bool _hasEmitted = false;

        /**
         * @brief Vertex buffer used to render all live particles in a single draw call.
         *
         * Uses Triangles primitive — 6 vertices per particle (2 triangles per quad).
         * Sized to Emitter::MaxParticles * 6 on Play().
         */
        sf::VertexArray _vertices;

        /**
         * @brief Fixed spawn origin used instead of the entity's TransformComponent.
         *
         * Empty by default (normal ECS lookup). Set via SetPositionOverride().
         */
        std::optional<sf::Vector2f> _positionOverride;

        /**
         * @brief Resolves the world-space spawn origin for this frame.
         *
         * Returns the position override (if set) or the entity's TransformComponent position,
         * with PositionOffset added in either case.
         */
        [[nodiscard]] sf::Vector2f ResolveSpawnOrigin() const;

        /**
         * @brief Spawns a single new particle at the given origin using the emitter's configuration.
         * @param emitter Emitter asset to read spawn parameters from.
         * @param origin World-space spawn position (typically the entity's Transform position).
         */
        void SpawnParticle(const Particles::Emitter& emitter, sf::Vector2f origin);

        /**
         * @brief Advances a single particle's simulation by one timestep.
         *
         * Updates position, velocity, rotation, scale, and animation frame.
         * Does not remove the particle — lifetime check is handled by Update().
         * @param particle Particle to update.
         * @param emitter Emitter asset to read physics and visual parameters from.
         * @param clip Pointer to the active AnimationClip. Null if Mode is not AnimationClip.
         * @param deltaTime Time elapsed since last update, in seconds.
         */
        void UpdateParticle(Particles::Particle& particle, const Particles::Emitter& emitter,
                            const Animation::AnimationClip* clip, float deltaTime);
    };
}