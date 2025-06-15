#include "Entity.h"
#include "Components/TransformComponent.h"
#include "Components/SpriteComponent.h"
#include "memory/Memory.h"

namespace LowEngine::ECS {
    Entity::Entity(Memory::Memory* memory) {
        _memory = memory;
    }

    Entity::Entity(Memory::Memory* memory, const Entity& other) {
        _memory = memory;

        Name = other.Name;
        Active = other.Active;
    }

    void Entity::Activate(const std::string& name) {
        Name = name;
        Active = true;
    }

    bool Entity::HasComponent(const std::type_index& typeIndex) {
        void* componentPtr = _memory->GetComponent(Id, typeIndex);
        return componentPtr != nullptr;
    }

    IEntity* Entity::Clone(Memory::Memory* newMemory) const {
        return new Entity(newMemory, *this);
    }
}
