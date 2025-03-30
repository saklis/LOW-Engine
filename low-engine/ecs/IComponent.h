#pragma once

#include <cstdint>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "memory/Memory.h"

namespace LowEngine::ECS {
    class IComponent {
    public:
        unsigned int EntityId = 0;
        bool Active = false;
        Memory::Memory* Memory;

        IComponent() = default;

        virtual ~IComponent() = default;

        virtual void Initialize() = 0;

        virtual void Update(float deltaTime) = 0;

        template<typename T>
        static std::vector<std::type_index> GetDependencies() {
            return T::Dependencies();
        }
    };
}
