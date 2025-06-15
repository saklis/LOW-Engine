#pragma once

namespace LowEngine::Memory {
    class Memory;
}

namespace LowEngine::ECS {
    /**
     * @brief Interface for Entities, should be used as Entity abstraction Layer
     */
    class IEntity {
    public:
        virtual ~IEntity() = default;

        /**
         * @brief Create a deep-copy of the Entity.
         *
         * Entity's clone will be created in newMomery.
         * @param newMemory Pointer to new Memory instance in which clone should be created.
         * @return Pointer to cloned Entity.
         */
        virtual IEntity* Clone(Memory::Memory* newMemory) const = 0;

        /**
         * @brief Check if Entity is Active. Entity that is not active will be skipped during Update and Draw calls.
         */
        bool Active = false;

        /**
         * @brief Id that was assigned to this Entity during creation.
         */
        size_t Id = 0;

        /**
         * @brief Name of this Entity.
         */
        std::string Name;

        /**
         * @brief Check if this Entity has a Component of given type.
         * @param typeIndex Type of the Component that should be queried.
         * @return True if this Entity has Component of queried type. False otherwise
         */
        virtual bool HasComponent(const std::type_index& typeIndex) = 0;
    protected:
        IEntity() = default;
    };
}
