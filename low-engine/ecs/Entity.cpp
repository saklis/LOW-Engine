#include "Entity.h"
#include "Components/TransformComponent.h"
#include "Components/SpriteComponent.h"

namespace LowEngine::ECS {
    // initialize default value to static field
    uint32_t Entity::_nextId = 0;

    Entity::Entity() {
        this->Id = _nextId++; // assign current value to this instance and then increment
    }

    void Entity::InitAsDefault() {
        this->Name = "Default";
        this->Active = true;
    }

    void Entity::Activate(const std::string& name) {
        this->Name = name + "_" + std::to_string(this->Id);
        this->Active = true;
    }

    void Entity::AddComponent(const std::string& typeName, uint32_t componentId) {
        this->_components[typeName].push_back(componentId);
    }

    uint32_t Entity::GetComponent(const std::string& typeName) {
        auto it = this->_components.find(typeName);
        if (it != this->_components.end() && !it->second.empty()) {
            return it->second[0];
        }

        return -1;
    }
}
