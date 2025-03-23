#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace LowEngine::ECS {
    class IComponent {
    public:
        unsigned int EntityId = 0;
        bool Active = false;

        IComponent() = default;

        virtual ~IComponent() = default;

        virtual void Initialize() = 0;

        virtual void Update(float deltaTime) = 0;

    protected:
    };
}
