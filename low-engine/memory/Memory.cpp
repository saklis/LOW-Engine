#include "Memory.h"

LowEngine::Memory::Memory::Memory() {
    // _entities.resize(1);
    // _entities[0].InitAsDefault();
}

uint32_t LowEngine::Memory::Memory::CreateEntity(const std::string& name) {
    ECS::Entity entity;
    entity.Activate(name);
    _entities.push_back(std::move(entity));
    return _entities.size() - 1;
}

std::vector<LowEngine::ECS::Entity>& LowEngine::Memory::Memory::GetEntities() {
    return _entities;
}

void LowEngine::Memory::Memory::Destroy() {
    _entities.clear();
    for (auto& component : _components) {
        component.second.clear();
    }
    _components.clear();
}
