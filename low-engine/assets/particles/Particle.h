#pragma once
#include "SFML/Graphics/Color.hpp"
#include "SFML/System/Vector2.hpp"

namespace LowEngine::Particles {
    /**
     * @brief Represents a single live particle managed by ParticleComponent.
     *
     * Particle is a pure data struct — it holds only runtime simulation state.
     * Spawn parameters and visual configuration come from the Emitter asset.
     */
    struct Particle {
        /**
         * @brief World-space position of the particle.
         */
        sf::Vector2f Position;

        /**
         * @brief Current movement direction and speed, in pixels per second.
         */
        sf::Vector2f Velocity;

        /**
         * @brief Time this particle has been alive, in seconds.
         *
         * Counts up from 0 on spawn. When it reaches MaxLifetime the particle is destroyed.
         */
        float Lifetime = 0.0f;

        /**
         * @brief Total lifespan of this particle, in seconds.
         *
         * Assigned at spawn from the Emitter's LifetimeMin/LifetimeMax range.
         * Used as the denominator when computing normalized life progress (Lifetime / MaxLifetime → 0..1)
         * for interpolating color, scale, and other over-lifetime properties.
         */
        float MaxLifetime = 0.0f;

        /**
         * @brief Current rotation of the particle sprite, in degrees.
         */
        float Rotation = 0.0f;

        /**
         * @brief Rate at which Rotation changes, in degrees per second.
         */
        float RotationSpeed = 0.0f;

        /**
         * @brief Uniform scale of the particle sprite.
         *
         * Interpolated each frame between Emitter's ScaleStart and ScaleEnd
         * based on normalized life progress.
         */
        float Scale = 0.0f;

        /**
         * @brief Index of the currently displayed animation frame.
         *
         * Only used when the Emitter's Mode is SpriteMode::AnimationClip.
         */
        std::size_t currentFrame = 0;

        /**
         * @brief Accumulated time for the current animation frame, in seconds.
         *
         * Compared against the clip's FrameDuration to decide when to advance currentFrame.
         * Only used when the Emitter's Mode is SpriteMode::AnimationClip.
         */
        float frameTimer = 0.0f;
    };
}