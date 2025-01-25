#include "TransformComponent.h"

void TransformComponent::ActivateAsDefault() {
    this->Position = sf::Vector2f(0.0f, 0.0f);
    this->Rotation = sf::degrees(0.0f);
    this->Scale = sf::Vector2f(1.0f, 1.0f);

    this->OwnerId = 0;
    this->Active = true;
}

void TransformComponent::Activate(uint32_t ownerId) {
    this->Position = sf::Vector2f(0.0f, 0.0f);
    this->Rotation = sf::degrees(0.0f);
    this->Scale = sf::Vector2f(1.0f, 1.0f);

    this->OwnerId = ownerId;
    this->Active = true;
}

void TransformComponent::Update() {
    // do nothing
}
