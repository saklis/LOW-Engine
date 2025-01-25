#pragma once

#include <cstdint>
#include <string>
#include <array>

#include "ecs/Entity.h"
#include "ecs/Components/TransformComponent.h"

class Memory {
public:
    static constexpr uint32_t MAX_ENTITIES = 1000;

    Memory();

    /**
     * Initializes new Entity object.
     * If there's no space for new Entity, it'll return Id of default entity.
     * @param name Name of the created entity
     * @return Id of new entity.
     */
    uint32_t CreateEntity(const std::string& name);

    /**
     * Initializes new Transform Component and assigns it to provided Entity.
     * If there's no space for new Component, it'll return Id of default Component.
     * @param entityId Id of the Entity that will own new Transform Component
     * @return Id of the new Component
     */
    uint32_t AddTransformComponent(uint32_t entityId);

protected:
    std::array<Entity, MAX_ENTITIES> _entities;
    std::array<TransformComponent, MAX_ENTITIES> _transformComponents;
};
