#include "Memory.h"

namespace LowEngine::Memory {
    Memory::Memory() {
        // do nothing
    }

    // ECS::Entity* Memory::GetEntity(unsigned int entityId) {
    //     if (entityId >= _entities.size()) return nullptr;
    //     return &_entities[entityId];
    // }

    // std::vector<ECS::Entity>& Memory::GetEntities() {
    //     return _entities;
    // }

    void Memory::UpdateAllComponents(float deltaTime) {
        for (auto& component: _components) {
            component.second->Update(deltaTime);
        }
    }

    void Memory::CollectSprites(std::vector<Sprite>& sprites) {
        for (auto& [type, pool] : _components) {
            pool->CollectSprites(sprites);
        }
    }

    void Memory::Destroy() {
        _entities.clear();
        for (auto& component: _components) {
            component.second.reset();
        }
        _components.clear();
    }
}
