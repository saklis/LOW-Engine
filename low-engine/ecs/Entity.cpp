#include "Entity.h"
#include "Components/TransformComponent.h"
#include "Components/SpriteComponent.h"
#include "memory/Memory.h"

// namespace LowEngine::Memory {
//     class Memory;
// }

namespace LowEngine::ECS {
    // initialize default value to static field
    unsigned int Entity::_nextId = 0;

    Entity::Entity(LowEngine::Memory::Memory* memory) {
        Id = _nextId++; // assign current value to this instance and then increment

        _memory = memory;
    }

    void Entity::InitAsDefault() {
        Name = "Default";
        Active = true;
    }

    void Entity::Activate(const std::string& name) {
        Name = name;// + "_" + std::to_string(this->Id);
        Active = true;
    }

    bool Entity::HasComponent(const std::type_index& typeIndex) {
        void* componentPtr = _memory->GetComponent(Id, typeIndex);
        return componentPtr != nullptr;
    }

    // void Entity::AddComponent(const std::type_index& typeIndex, unsigned int componentId) {
    //     //_components[typeIndex].push_back(componentId);
    //     _memory->CreateComponent(Id, typeIndex);
    // }

    // int Entity::GetComponent(const std::type_index& typeIndex) {
    //     auto it = _components.find(typeIndex);
    //     if (it != _components.end() && !it->second.empty()) {
    //         return it->second[0];
    //     }
    //
    //     return -1;
    // }

    // std::vector<std::type_index> Entity::GetComponentTypes() {
    //     std::vector<std::type_index> types;
    //
    //     for (auto& component : _components) {
    //         types.emplace_back(component.first);
    //     }
    //
    //     return types;
    // }
}
