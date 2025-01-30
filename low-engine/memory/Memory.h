#pragma once

#include <cstdint>
#include <string>
#include <array>

#include "../../lords-of-war/config.h"
#include "ecs/Entity.h"
#include "ecs/Components/TransformComponent.h"

class Memory {
public:
    Memory();

    /**
     * Initializes new Entity object.
     * If there's no space for new Entity, it'll return Id of default entity.
     * @param name Name of the created entity
     * @return Id of new entity.
     */
    uint32_t CreateEntity(const std::string& name);

    /**
     * Initializes new Component object.
     * @tparam T Type of component
     * @tparam Args Arguments of component's initializer
     * @param entityId Id of an entity this component will be owned by
     * @param args Initializer's arguments
     * @return Id of the initialized component
     */
    template <typename T, typename... Args>
    uint32_t CreateComponent(uint32_t entityId, Args&&... args);

protected:
    std::array<Entity, config::MAX_ENTITIES> _entities;
    std::array<TransformComponent, config::MAX_ENTITIES> _transformComponents;
};
