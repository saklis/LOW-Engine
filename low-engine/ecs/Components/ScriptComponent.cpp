
#include "ScriptComponent.h"

void LowEngine::ECS::ScriptComponent::InitAsDefault() {
    this->Active = true;
}

void LowEngine::ECS::ScriptComponent::Activate(uint32_t ownerEntityId) {
    this->EntityId = ownerEntityId;
    this->Active = true;
}

void LowEngine::ECS::ScriptComponent::Update() {
    // do nothing
}
