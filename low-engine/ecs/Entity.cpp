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

	nlohmann::ordered_json Entity::SerializeToJSON() {
		nlohmann::ordered_json entityJson;

		entityJson["id"] = Id;
		entityJson["name"] = Name;
		entityJson["active"] = Active;
		
        return entityJson;
	}

    void Entity::DeserializeFromJSON(const nlohmann::ordered_json& jsonData) {
        if (jsonData.contains("name")) {
            Name = jsonData["name"].get<std::string>();
        }
        if (jsonData.contains("active")) {
            Active = jsonData["active"].get<bool>();
		}
    }

    IEntity* Entity::Clone(Memory::Memory* newMemory) const {
        return new Entity(newMemory, *this);
    }
}
