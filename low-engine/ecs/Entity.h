#pragma once

#include <cstdint>
#include <string>

#include "Components/SpriteComponent.h"
#include "Components/TransformComponent.h"
#include "scene/Scene.h"

namespace LowEngine::ECS {
    class Entity {
    public:
        bool Active = false;
        uint32_t Id = 0;
        std::string Name;

        Scene& Scene;
        TransformComponent* Transform = nullptr;
        SpriteComponent* Sprite = nullptr;

        Entity(LowEngine::Scene& scene);

        void InitAsDefault();
        void Activate(const std::string& name);

        template <typename T, typename... Args>
        T& AddComponent(Args&&... args);

    protected:
        static uint32_t _nextId;
    };
}
