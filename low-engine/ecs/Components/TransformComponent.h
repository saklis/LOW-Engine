#pragma once

#include "ecs/IComponent.h"
#include <SFML/Graphics.hpp>

namespace LowEngine::ECS {
    class TransformComponent : public IComponent {
    public:
        sf::Vector2f Position = sf::Vector2f(0.0f, 0.0f);
        sf::Angle Rotation = sf::degrees(0.0f);
        sf::Vector2f Scale = sf::Vector2f(1.0f, 1.0f);

        TransformComponent() = default;

        virtual ~TransformComponent() = default;

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
