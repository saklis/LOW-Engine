#include "TransformComponent.h"

LowEngine::ECS::TransformComponent::TransformComponent(uint32_t ownerEntityId): Component(ownerEntityId) {
}

void LowEngine::ECS::TransformComponent::InitAsDefault() {
    this->Position = sf::Vector2f(0.0f, 0.0f);
    this->Rotation = sf::degrees(0.0f);
    this->Scale = sf::Vector2f(1.0f, 1.0f);

    this->Active = true;
}

void LowEngine::ECS::TransformComponent::Activate(uint32_t ownerEntityId) {
    this->Position = sf::Vector2f(0.0f, 0.0f);
    this->Rotation = sf::degrees(0.0f);
    this->Scale = sf::Vector2f(1.0f, 1.0f);

    this->EntityId = ownerEntityId;
    this->Active = true;
}

void LowEngine::ECS::TransformComponent::Update() {
    // do nothing
}
