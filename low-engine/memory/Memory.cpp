#include "Memory.h"

namespace LowEngine::Memory {
    Memory::Memory() {
        // do nothing
    }

    Memory::Memory(Memory const& other) : _typeInfos(other._typeInfos) {
        _nextTypeId = other._nextTypeId;

        for (auto const& entPtr: other._entities) {
            _entities.emplace_back(
                std::unique_ptr<ECS::IEntity>(entPtr->Clone(this))
            );
            _entities.back()->Id = _entities.size() - 1;
        }

        for (auto const& [typeIdx, poolPtr]: other._components) {
            _components[typeIdx] = poolPtr->Clone(this);
        }
    }

    void Memory::UpdateAllComponents(float deltaTime) {
        for (auto& component: _components) {
            component.second->Update(deltaTime);
        }
    }

    void Memory::CollectSprites(std::vector<Sprite>& sprites) {
        for (auto& [type, pool]: _components) {
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
