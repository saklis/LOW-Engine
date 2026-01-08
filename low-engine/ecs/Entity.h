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
    /**
     * @brief Base Entity for the engine.
     */
    class Entity : public IEntity {
    public:
        /**
         * @brief Constructs an Entity object.
         *
         * Initializes an entity using the provided memory manager.
         *
         * @param memory A pointer to the memory manager instance responsible for managing components of the entity.
         */
        explicit Entity(Memory::Memory* memory);

        /**
         * @brief Constructs an Entity object with a copy of another entity.
         *
         * Creates an entity based on the given memory manager and a copy of an existing entity.
         *
         * @param memory A pointer to the memory manager instance responsible for managing components of the entity.
         * @param other The source Entity object to copy during construction.
         */
        Entity(Memory::Memory* memory, const Entity& other);

        /**
         * @brief Activate this instance and assigning it a name.
         * @param name Name that should be assigned to Entity.
         */
        void Activate(const std::string& name);

        /**
         * @brief Add new component to this entity.
         * @tparam T Type of the component to add.
         * @tparam Args Arguments for Component's Initialize() function.
         * @param args Arguments for Component's Initialize() function.
         * @return Pointer to newly created Componet.
         */
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

        /**
         * @brief Destroy Component of given type.
         * @tparam T Type of the Component that should be destroyed.
         *
         * This method will remove Component from Entity and destroy it.
         */
        template<typename T>
        void DestroyComponent() {
            _memory->DestroyComponent<T>(Id);

            _log->debug("Component of type {} destroyed for entity with id {}", DemangledTypeName(typeid(T)), Id);
        }

        /**
         * @brief Check if this Entity has a Component of given type.
         * @param typeIndex Type of the Component that should be queried.
         * @return True if this Entity has Component of queried type. False otherwise
         */
        bool HasComponent(const std::type_index& typeIndex) override;

        /**
         * @brief Serialize Entity to JSON representation.
		 * @return JSON object representing this Entity.
         */
        nlohmann::ordered_json SerializeToJSON() override;

        /**
         * @brief Deserialize Entity from JSON representation.
		 * @param jsonData JSON object representing this Entity.
         */
		void DeserializeFromJSON(const nlohmann::ordered_json& jsonData) override;

        /**
         * @brief Create a deep-copy of the Entity.
         *
         * Entity's clone will be created in newMomery.
         * @param newMemory Pointer to new Memory instance in which clone should be created.
         * @return Pointer to cloned Entity.
         */
        IEntity* Clone(Memory::Memory* newMemory) const override;

        /**
         * @brief Retrieve Component of given type.
         * @tparam T Type of the component to retrieve.
         * @return Pointer to Component. Nullptr if component doesn't exists.
         */
        template<typename T>
        T* GetComponent() {
            return _memory->GetComponent<T>(Id);
        }

    protected:
        /**
         * @brief Pointer to Memory manager that is responsible for this instance of Entity. Will also contian all of its components.
         */
        Memory::Memory* _memory;
    };
}
