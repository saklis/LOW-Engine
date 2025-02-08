#pragma once

#include <cstdint>
#include <string>
#include <array>

#include "ecs/Entity.h"
#include "ecs/Components/TransformComponent.h"
#include "ecs/Components/SpriteComponent.h"

namespace LowEngine::Memory {
    class Memory {
    public:
        Memory(Scene& scene);

        //uint32_t CreateEntity(const std::string& name);
        ECS::Entity& CreateEntity(const std::string& name);

        // template <typename T, typename... Args>
        // uint32_t CreateComponent(uint32_t entityId, Args&&... args);
        template <typename T, typename... Args>
        T& CreateComponent(uint32_t entityId, Args&&... args);

    protected:
        std::array<ECS::Entity, 1000> _entities;
        std::array<ECS::TransformComponent, 1000> _transformComponents;
    };
}
