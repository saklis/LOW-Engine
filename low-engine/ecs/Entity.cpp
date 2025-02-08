#include "Entity.h"

// initialize default value to static field
uint32_t LowEngine::ECS::Entity::_nextId = 0;

LowEngine::ECS::Entity::Entity(LowEngine::Scene& scene): Scene(scene) {
    this->Id = _nextId++; // assign current value to this instance and then increment
}

void LowEngine::ECS::Entity::InitAsDefault() {
    this->Name = "Default";
    this->Active = true;
}

void LowEngine::ECS::Entity::Activate(const std::string& name) {
    this->Name = name + "_" + std::to_string(this->Id);
    this->Active = true;
}

template<typename T, typename... Args>
T& LowEngine::ECS::Entity::AddComponent(Args&&... args) {
    Scene.AddComponent<T>(this->Id, std::forward<Args>(args)...);
}
