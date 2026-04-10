#pragma once
#include <cstddef>
#include <string>
#include <filesystem>

#include "nlohmann/json_fwd.hpp"
#include "SFML/Graphics/Color.hpp"
#include "SFML/System/Vector2.hpp"

namespace LowEngine::Particles {
    /**
     * @brief Data asset that defines the behaviour and appearance of a particle system.
     *
     * Emitter is a reusable, serializable definition — it holds no runtime simulation state.
     * It is loaded once via the Assets manager and referenced by any number of ParticleComponents,
     * which act as the runtime executors that own and simulate live Particle instances.
     */
    class Emitter {
    public:
        /**
         * @brief Determines how each particle's visual is sourced from the texture.
         */
        enum class SpriteMode {
            FullTexture,      /**< Entire texture is used as the particle sprite. */
            SpriteSheetFrame, /**< A single static frame from a sprite sheet. */
            AnimationClip     /**< An animation clip plays on each particle. */
        };

        /**
         * @brief Path to the emitter file.
         *
         * This is used to store the path from which the emitter was loaded.
         */
        std::filesystem::path Path;

        /**
         * @brief ID of the texture used to render particles.
         */
        std::size_t TextureId = 0;

        /**
         * @brief Determines how the texture is sampled for each particle.
         *
         * @see SpriteMode
         */
        SpriteMode Mode = SpriteMode::FullTexture;

        /**
         * @brief Index of the sprite sheet frame to display on each particle.
         *
         * Only used when Mode == SpriteMode::SpriteSheetFrame.
         */
        std::size_t FrameIndex = 0;

        /**
         * @brief Name of the animation clip to play on each particle.
         *
         * Only used when Mode == SpriteMode::AnimationClip.
         * The clip must be defined on the SpriteSheet associated with TextureId.
         */
        std::string AnimClipName;

        // --- Spawn ---

        /**
         * @brief Number of particles spawned per second during continuous emission.
         *
         * Set to 0 to disable continuous emission and rely on BurstCount only.
         */
        float EmissionRate = 10.0f;

        /**
         * @brief Number of particles spawned immediately when Play() is called.
         *
         * Applied once per Play() call regardless of EmissionRate.
         * Set to 0 to disable burst.
         */
        std::size_t BurstCount = 0;

        /**
         * @brief Maximum number of particles that can be alive at one time.
         *
         * New particles will not spawn if this limit is reached.
         */
        std::size_t MaxParticles = 100;

        /**
         * @brief Whether emission restarts automatically after all particles have died.
         *
         * When false, the system emits once and stops (one-shot).
         */
        bool Loop = false;

        // --- Lifetime ---

        /**
         * @brief Minimum lifespan of a spawned particle, in seconds.
         *
         * Each particle's MaxLifetime is chosen randomly between LifetimeMin and LifetimeMax.
         */
        float LifetimeMin = 1.0f;

        /**
         * @brief Maximum lifespan of a spawned particle, in seconds.
         *
         * Each particle's MaxLifetime is chosen randomly between LifetimeMin and LifetimeMax.
         */
        float LifetimeMax = 2.0f;

        // --- Initial velocity ---

        /**
         * @brief Minimum initial speed of a spawned particle, in pixels per second.
         */
        float SpeedMin = 50.0f;

        /**
         * @brief Maximum initial speed of a spawned particle, in pixels per second.
         */
        float SpeedMax = 100.0f;

        /**
         * @brief Total angular spread of the emission cone, in degrees.
         *
         * The initial velocity direction for each particle is chosen randomly within
         * a cone of this width centred on Direction. 360 produces omnidirectional emission.
         */
        float SpreadAngle = 360.0f;

        /**
         * @brief Base direction for particle emission, as a unit vector.
         *
         * Acts as the centre of the spread cone defined by SpreadAngle.
         * Ignored when SpreadAngle == 360 (all directions equally probable).
         */
        sf::Vector2f Direction = sf::Vector2f(0.0f, -1.0f);

        /**
         * @brief Radius of the circular spawn area around the entity's origin, in pixels.
         *
         * Each particle spawns at a random point within this radius.
         * Set to 0 to spawn all particles exactly at the entity's origin.
         */
        float SpawnRadius = 0.0f;

        /**
         * @brief Minimum initial rotation speed of a spawned particle, in degrees per second.
         *
         * Each particle's RotationSpeed is chosen randomly between RotationSpeedMin and RotationSpeedMax.
         * Negative values spin clockwise, positive counter-clockwise. Set both to 0 to disable rotation.
         */
        float RotationSpeedMin = 0.0f;

        /**
         * @brief Maximum initial rotation speed of a spawned particle, in degrees per second.
         *
         * Each particle's RotationSpeed is chosen randomly between RotationSpeedMin and RotationSpeedMax.
         * Negative values spin clockwise, positive counter-clockwise. Set both to 0 to disable rotation.
         */
        float RotationSpeedMax = 0.0f;

        // --- Visuals over lifetime ---

        /**
         * @brief Colour of a particle at the moment it spawns (normalized life = 0).
         */
        sf::Color ColorStart = sf::Color::White;

        /**
         * @brief Colour of a particle just before it dies (normalized life = 1).
         *
         * Linearly interpolated from ColorStart over the particle's lifetime.
         */
        sf::Color ColorEnd = sf::Color::Transparent;

        /**
         * @brief Uniform scale of a particle at the moment it spawns (normalized life = 0).
         */
        float ScaleStart = 1.0f;

        /**
         * @brief Uniform scale of a particle just before it dies (normalized life = 1).
         *
         * Linearly interpolated from ScaleStart over the particle's lifetime.
         */
        float ScaleEnd = 0.0f;

        // --- Physics ---

        /**
         * @brief Constant acceleration applied to every particle each frame, in pixels per second squared.
         *
         * Use to simulate gravity, wind, or other uniform forces.
         * Set to {0, 0} to disable.
         */
        sf::Vector2f Gravity = sf::Vector2f(0.0f, 0.0f);

        /**
         * @brief Render order relative to other sprites in the scene.
         *
         * Lower values are drawn first (appear behind). Follows the same convention as SpriteComponent::DrawOrder.
         */
        int DrawOrder = 0;

        /**
         * @brief Serialize this Emitter's configuration to JSON.
         * @return JSON object containing all Emitter fields.
         */
        [[nodiscard]] nlohmann::ordered_json SerializeToJSON() const;

        /**
         * @brief Deserialize this Emitter's configuration from JSON.
         *
         * TextureAlias is the only required field — the referenced texture must already
         * be loaded in the Assets manager before calling this. All other fields fall back
         * to their default values if absent.
         * @param json JSON object to read from.
         * @return True on success. False if a required field is missing or invalid.
         */
        bool DeserializeFromJSON(const nlohmann::ordered_json& json);
    };
}