#include "SpriteComponent.h"

LowEngine::ECS::SpriteComponent::SpriteComponent(uint32_t ownerEntityId) : Component(ownerEntityId) {
    this->EntityId = ownerEntityId;
}

void LowEngine::ECS::SpriteComponent::InitAsDefault() {
    this->Active = true;
}

void LowEngine::ECS::SpriteComponent::Activate(uint32_t ownerEntityId) {
    this->EntityId = ownerEntityId;
    this->Active = true;
}

void LowEngine::ECS::SpriteComponent::Update() {
}
