#pragma once
#include <cstdint>

namespace LowEngine::ECS {
    class Component {
    public:
        uint32_t EntityId = 0;
        bool Active = false;

        Component() = default;
        explicit Component(uint32_t ownerEntityId);


        virtual void InitAsDefault() = 0;
        virtual void Activate(uint32_t ownerEntityId) = 0;
        virtual void Update() = 0;
    };
}
