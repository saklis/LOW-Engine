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
        /**
         * @brief Id of the Entity that owns this component.
         */
        size_t EntityId = 0;

        /**
         * @brief Is this component Active?
         *
         * Component that are not Active skip their Update and Draw calls.
         */
        bool Active = false;

        explicit IComponent(Memory::Memory* memory) : _memory(memory) {
        };

        IComponent(Memory::Memory* memory, IComponent const* other) : _memory(memory) {
            EntityId = other->EntityId;
            Active = other->Active;
        };

        /**
         * @brief Creates a copy of the Component in provided Memory manager.
         *
         * Called by Component Pool.
         * @param newMemory Pointer to Memory manager that should hold copy of the component.
         * @param rawStorage Pointer to raw memory that new instance should be placed in.
         */
        virtual void CloneInto(Memory::Memory* newMemory, void*  rawStorage) const = 0;

        virtual ~IComponent() = default;

        /**
         * @brief Initialize default values for component.
         *
         * Called automatically after constructor
         */
        virtual void Initialize() = 0;

        /**
         * @brief Update Component's state.
         * @param deltaTime Time that is passed since last update, in seconds.
         */
        virtual void Update(float deltaTime) {
        };

        /**
         * @brief Retrieve a pointer to Sprite that should be drawn in current frame.
         * @return Pointer to Sprite. Returns nullptr if there's nothing to be drawn.
         */
        virtual Sprite* Draw() {
            return nullptr;
        };

        /**
         * @brief Retrieve a list of types of components that this component depends on.
         * @return List of types of components that this component depends on.
         */
        template<typename T>
        static std::vector<std::type_index> GetDependencies() {
            return T::Dependencies();
        }

    protected:
        /**
         * @brief Pointer to Memory manager that is responsible for this instance of Component. Will also contain Entity.
         */
        Memory::Memory* _memory;
    };
}
