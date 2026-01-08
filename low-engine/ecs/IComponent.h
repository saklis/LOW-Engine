#pragma once

#include <cstdint>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "nlohmann/json.hpp"

#include "graphics/Sprite.h"
#include "memory/Memory.h"
#include "utils/TypeName.h"

namespace LowEngine::ECS {
    class IComponentBase {
    public:
        /**
         * @brief Id of the Entity that owns this component.
         */
        size_t EntityId = 0;

        /**
         * @brief Is this component Active?
         *
         * Component that is not Active skips their Update and Draw calls.
         */
        bool Active = false;

        explicit IComponentBase(Memory::Memory* memory) : _memory(memory) {
        };

        IComponentBase(Memory::Memory* memory, IComponentBase const* other) : _memory(memory) {
            EntityId = other->EntityId;
            Active = other->Active;
        };

        /**
         * @brief Creates a copy of the Component in the provided Memory manager.
         *
         * Called by Component Pool.
         * @param newMemory Pointer to Memory manager that should hold a copy of the component.
         * @param rawStorage Pointer to raw memory that a new instance should be placed in.
         */
        virtual void CloneInto(Memory::Memory* newMemory, void*  rawStorage) const = 0;

        virtual ~IComponentBase() = default;

        /**
         * @brief Initialize default values for a component.
         *
         * Called automatically after constructor
         */
        virtual void Initialize() = 0;

        /**
         * @brief Update Component's state.
         * @param deltaTime Time that is passed since last update in seconds.
         */
        virtual void Update(float deltaTime) {
        };

        /**
         * @brief Retrieve a pointer to Sprite that should be drawn in the current frame.
         * @return Pointer to Sprite. Returns nullptr if there's nothing to be drawn.
         */
        virtual Sprite* Draw() {
            return nullptr;
        };



    protected:
        /**
         * @brief Pointer to Memory manager that is responsible for this instance of Component. Will also contain Entity.
         */
        Memory::Memory* _memory;
    };

    /**
     * @brief Base class for Components that depend on other Components.
     * 
     * This template class allows Components to specify their dependencies on other Components.
     * Dependencies are used during Entity configuration to ensure all required Components are present.
     *
     * @tparam Derived The actual component type (CRTP pattern)
     * @tparam Dependencies Variadic template parameter pack listing all Component types that this Component depends on.
     */
    template<typename Derived, typename... Dependencies>
    class IComponent : public IComponentBase {
    public:
        /**
         * @brief Construct a new Component instance.
         * @param memory Pointer to Memory manager that will be responsible for this Component.
         */
        explicit IComponent(Memory::Memory* memory) : IComponentBase(memory) {
        };

        /**
         * @brief Construct a new Component as a copy of an existing one.
         * @param memory Pointer to Memory manager that will be responsible for this Component.
         * @param other Pointer to Component that should be copied.
         */
        IComponent(Memory::Memory* memory, IComponent const* other) : IComponentBase(memory, other) {};

        /**
         * @brief Get a list of Component types that this Component depends on.
         * 
         * This method returns a vector of type indices representing all Component types
         * that were specified as dependencies in the template parameters.
         * The dependencies are used during Entity configuration to ensure all required
         * Components are present before this Component can be added to an Entity.
         * 
         * @return Reference to a static vector containing type indices of all dependencies.
         */
        static const std::vector<std::type_index>& GetDependencies() {
            static std::vector<std::type_index> dependencies = {
                std::type_index(typeid(Dependencies))...
            };
            return dependencies;
        };

        /**
		 * @brief Creates a copy of the Component in the provided Memory manager.
		 * @param newMemory Pointer to Memory manager that should hold a copy of the component.
		 * @param rawStorage Pointer to raw memory that a new instance should be placed in.
         */
        void CloneInto(Memory::Memory* newMemory, void* rawStorage) const override {
            new(rawStorage) Derived(newMemory, static_cast<Derived const*>(this));
        }

        /**
         * @brief Serialize Component's data to JSON format.
         * @return JSON object containing Component's data.
         */
        virtual nlohmann::ordered_json SerializeToJSON() {
            nlohmann::ordered_json compJson;
			compJson["Type"] = LowEngine::Utils::GetCleanTypeName<Derived>();
            compJson["EntityId"] = EntityId;
            compJson["Active"] = Active;
            return compJson;
        };

        /**
         * @brief Deserialize Component's data from JSON format.
         * @param jsonData JSON object containing Component's data.
         */
        virtual bool DeserializeFromJSON(const nlohmann::ordered_json& jsonData) {
            // EntityId should not be changed during deserialization
            if (jsonData.contains("Active")) {
                Active = jsonData["Active"].get<bool>();
            }
            else
            {
				return false;
            }

            return true;
        };
    };
}
