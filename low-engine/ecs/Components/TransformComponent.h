#pragma once

#include "ecs/Component.h"
#include <SFML/Graphics.hpp>

namespace LowEngine::ECS {
  class TransformComponent : public Component {
  public:
    sf::Vector2f Position = sf::Vector2f(0.0f, 0.0f);
    sf::Angle Rotation = sf::degrees(0.0f);
    sf::Vector2f Scale = sf::Vector2f(1.0f, 1.0f);

    TransformComponent() = default;
    explicit TransformComponent(uint32_t ownerEntityId);
    virtual ~TransformComponent() = default;

    void InitAsDefault() override;

    void Activate(uint32_t ownerEntityId) override;

    void Update() override;
  };
}
