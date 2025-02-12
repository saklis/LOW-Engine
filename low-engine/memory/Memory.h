#pragma once

#include <cstdint>
#include <string>
#include <array>
#include <vector>
#include <unordered_map>

#include "ecs/Entity.h"
#include "ecs/IComponent.h"
#include "ecs/Components/TransformComponent.h"
#include "ecs/Components/SpriteComponent.h"

namespace LowEngine::Memory {
    class Memory {
    public:
        Memory();

        uint32_t CreateEntity(const std::string& name);

        template<typename T>
        uint32_t CreateComponent(uint32_t entityId) {
            auto& byteVector =_components[typeid(T).name()];
            size_t currentSize = byteVector.size();
            byteVector.resize(currentSize + sizeof(T));

            void* memory = byteVector.data() + currentSize;
            T* component = new (memory) T(); // placement new to construct object in target memory

            uint32_t componentId = currentSize / sizeof(T);
            _entities[entityId].AddComponent(typeid(T).name(), componentId);

            component->EntityId = entityId;
            component->Active = true;
            component->Initialize();

            return componentId;
        }

        template<typename T>
        T& GetComponent(uint32_t entityId) {
            std::string typeName = typeid(T).name();

            uint32_t componentId = _entities[entityId].GetComponent(typeName);

            auto& byteVector = _components[typeName];
            size_t offset = componentId * sizeof(T);
            if (offset + sizeof(T) > byteVector.size()) {
                throw std::runtime_error("Component Id is out of range for type: " + typeName);
            }

            return *reinterpret_cast<T*>(byteVector.data() + offset);
        }

    protected:
        std::array<ECS::Entity, 1000> _entities;
        std::unordered_map<std::string, std::vector<std::byte> > _components;
    };
}
