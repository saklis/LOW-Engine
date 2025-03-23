#pragma once
#include "ecs/IComponent.h"
#include "SFML/Graphics/View.hpp"

namespace LowEngine::ECS {
    class CameraComponent : public IComponent {
    public:
        float ZoomFactor = 1.0f;

        CameraComponent() = default;

        ~CameraComponent() override;

        void Initialize() override;

        void Update(float deltaTime) override;

    protected:
        sf::View _view;
    };
}
