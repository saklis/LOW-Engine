#pragma once

#include "ecs/Component.h"

namespace LowEngine::ECS {
    class ScriptComponent : public Component {
    public:
        ScriptComponent() = default;
        explicit ScriptComponent(uint32_t ownerEntityId);
        virtual ~ScriptComponent() = default;

        void InitAsDefault() override;
        void Activate(uint32_t ownerEntityId) override;
        void Update() override;
    };
}
