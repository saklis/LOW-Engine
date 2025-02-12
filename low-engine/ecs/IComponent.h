#pragma once

#include <cstdint>

namespace LowEngine::ECS {
    class IComponent {
    public:
        uint32_t EntityId = 0;
        bool Active = false;

        IComponent() = default;

        virtual ~IComponent() = default;

        virtual void Initialize() = 0;

        virtual void Update(float deltaTime) = 0;

    protected:
    };
}
