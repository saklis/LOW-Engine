#include "Memory.h"

namespace LowEngine::Memory {
    Memory::Memory() {
        // do nothing
    }

    void Memory::Update(float deltaTime) {
        for (auto component: _components) {

        }
    }

    unsigned int Memory::CreateEntity(const std::string& name) {
        ECS::Entity entity;
        entity.Activate(name);
        _entities.push_back(std::move(entity));
        return _entities.size() - 1;
    }

    std::vector<ECS::Entity>& Memory::GetEntities() {
        return _entities;
    }

    void Memory::Destroy() {
        _entities.clear();
        for (auto& component: _components) {
            component.second.clear();
        }
        _components.clear();
    }
}
