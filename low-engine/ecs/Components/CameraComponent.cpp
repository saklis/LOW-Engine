
#include "CameraComponent.h"

namespace LowEngine::ECS {

    void CameraComponent::Update(float deltaTime) {
        auto transformComponent = Memory->GetComponent<TransformComponent>(EntityId);
        if (transformComponent) {
            _view.setCenter(transformComponent->Position);
            _view.setRotation(transformComponent->Rotation);
        }
    }

    void CameraComponent::SetWindowSize(sf::Vector2f windowSize) {
        _view.setSize(windowSize);
    }

    void CameraComponent::SetView(sf::RenderWindow& window) {
        auto size = window.getSize();
        _view.setSize({size.x * ZoomFactor, size.y * ZoomFactor});
        window.setView(_view);
    }
}
