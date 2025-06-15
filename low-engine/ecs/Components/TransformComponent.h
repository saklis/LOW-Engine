#pragma once

#include "ecs/IComponent.h"
#include <SFML/Graphics.hpp>

namespace LowEngine::ECS {
    /**
     * Represents a component that manages the transformation data, including position, rotation, and scale.
     */
    class TransformComponent : public IComponent {
    public:
        /**
         * @brief Position in the world, in Units.
         *
         * Units are equal to SFML's positioning units.
         */
        sf::Vector2f Position = sf::Vector2f(0.0f, 0.0f);

        /**
         * @brief Current rotation.
         */
        sf::Angle Rotation = sf::degrees(0.0f);

        /**
         * @brief Current scale.
         */
        sf::Vector2f Scale = sf::Vector2f(1.0f, 1.0f);

        explicit TransformComponent(Memory::Memory* memory)
            : IComponent(memory) {
        }

        TransformComponent(Memory::Memory* memory, TransformComponent const* other)
            : IComponent(memory, other), Position(other->Position), Rotation(other->Rotation), Scale(other->Scale) {
        }

        ~TransformComponent() override = default;

        void CloneInto(Memory::Memory* newMemory, void* rawStorage) const override {
            new(rawStorage) TransformComponent(newMemory, this);
        }

        static const std::vector<std::type_index>& Dependencies() {
            static std::vector<std::type_index> dependencies = {};
            return dependencies;
        }

        void Initialize() override {
        }

        void Update(float deltaTime) override {
        }
    };
}
