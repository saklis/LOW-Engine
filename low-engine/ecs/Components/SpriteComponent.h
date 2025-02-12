#pragma once
#include "ecs/IComponent.h"

namespace LowEngine::ECS {
    class SpriteComponent : public IComponent {
    public:
        SpriteComponent() = default;

        virtual ~SpriteComponent() = default;

        void Initialize() override {
        }

        void Update(float deltaTime) override {
        }
    };
}
