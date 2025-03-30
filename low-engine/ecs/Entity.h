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

        void InitAsDefault();

        void Activate(const std::string& name);

        //void AddComponent(const std::type_index& typeIndex, unsigned int componentId);

        template<typename T, typename... Args>
        T* AddComponent(Args&&... args) {
            return _memory->CreateComponent<T>(Id, std::forward<Args>(args)...);
        }

        // int GetComponent(const std::type_index& typeIndex);

        bool HasComponent(const std::type_index& typeIndex);

        template<typename T>
        T* GetComponent() {
            return _memory->GetComponent<T>(Id);
        }

        // std::vector<std::type_index> GetComponentTypes();

    protected:
        static unsigned int _nextId;

        Memory::Memory* _memory;
        //std::vector<std::type_index> _components;
    };
}
