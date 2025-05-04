#pragma once

#include <cstdint>
#include <string>
#include <array>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "IEntity.h"
#include "memory/Memory.h"

namespace LowEngine::Memory {
    class Memory;
}

namespace LowEngine::ECS {
    class Entity : public IEntity {
    public:
        explicit Entity(Memory::Memory* memory);

        Entity(Memory::Memory* memory, const Entity& other);

        void InitAsDefault();

        void Activate(const std::string& name);

        //void AddComponent(const std::type_index& typeIndex, unsigned int componentId);

        template<typename T, typename... Args>
        T* AddComponent(Args&&... args) {
            T* component = _memory->CreateComponent<T>(Id, std::forward<Args>(args)...);
            if (component == nullptr) {
                _log->error("Failed to create component of type {} for entity with id {}", DemangledTypeName(typeid(T)), Id);
                return nullptr;
            }

            _log->debug("Component of type {} created for entity with id {}", DemangledTypeName(typeid(T)), Id);
            return component;
        }

        // int GetComponent(const std::type_index& typeIndex);

        bool HasComponent(const std::type_index& typeIndex);

        IEntity* Clone(Memory::Memory* newMemory) const override;

        template<typename T>
        T* GetComponent() {
            return _memory->GetComponent<T>(Id);
        }

        // std::vector<std::type_index> GetComponentTypes();

    protected:
        Memory::Memory* _memory;
        //std::vector<std::type_index> _components;
    };
}
