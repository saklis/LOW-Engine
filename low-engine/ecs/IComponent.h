#pragma once

#include <cstdint>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "graphics/Sprite.h"
#include "memory/Memory.h"

namespace LowEngine::ECS {
    class IComponent {
    public:
        unsigned int EntityId = 0;
        bool Active = false;

        explicit IComponent(Memory::Memory* memory) : _memory(memory) {
        };

        IComponent(Memory::Memory* memory, IComponent const* other) : _memory(memory) {
            EntityId = other->EntityId;
            Active = other->Active;
        };

        virtual void CloneInto(Memory::Memory* newMemory, void*  rawStorage) const = 0;

        virtual ~IComponent() = default;

        virtual void Initialize() = 0;

        virtual void Update(float deltaTime) {
        };

        virtual LowEngine::Sprite* Draw() {
            return nullptr;
        };

        template<typename T>
        static std::vector<std::type_index> GetDependencies() {
            return T::Dependencies();
        }

    protected:
        Memory::Memory* _memory;
    };
}
