#pragma once

#include <cstdint>
#include <string>
#include <array>
#include <vector>

#include "ecs/Entity.h"
#include "ComponentRegistry.h"

namespace LowEngine::Memory {
    class Memory {
    public:
        Memory();

        uint32_t CreateEntity(const std::string& name);

        template<typename T, typename... Args>
        uint32_t CreateComponent(uint32_t entityId, Args&&... args) {
            const std::string typeName = typeid(T).name();

            // register component type if not already registered
            if (!_componentRegistry.IsComponentRegistered(typeName)) {
                _componentRegistry.RegisterComponent<T>(typeName);
            }

            auto component = _componentRegistry.CreateComponent(typeName);

            //_components[typeName] -  Automatically initializes an empty std::vector
            _components[typeName].emplace_back(std::move(component));
            uint32_t componentId = _components[typeName].size() - 1;

            _entities[entityId].AddComponent(typeName, componentId);
            component->Activate(entityId, std::forward<Args>(args)...);

            return componentId;
        }

        template<typename T>
        T& GetComponent(uint32_t entityId) {

        }

    protected:
        ComponentRegistry _componentRegistry;

        std::array<ECS::Entity, 1000> _entities;
        std::unordered_map<std::string, std::vector<std::unique_ptr<ECS::Component> > > _components;
    };
}
