#include "Component.h"

LowEngine::ECS::Component::Component(uint32_t ownerEntityId) {
    this->EntityId = ownerEntityId;
}
