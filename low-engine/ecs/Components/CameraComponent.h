#pragma once
#include "TransformComponent.h"
#include "ecs/IComponent.h"
#include "SFML/Graphics/View.hpp"

namespace LowEngine::ECS {
    /**
     * @brief Represents a component that acts as a camera in the scene.
     *
     * Depends on Transform Component
     */
    class CameraComponent : public IComponent<CameraComponent, TransformComponent> {
    public:
        /**
         * @brief A factor by which the viewport will be zoomed-in or -out.
         *
         * - <0.0 mirrors viewport;
         * - 0.0 - 1.0 zoom in;
         * - >1.0 zoom out;
         */
        float ZoomFactor = 1.0f;

        explicit CameraComponent(Memory::Memory* memory)
            : IComponent(memory) {
        }

        CameraComponent(Memory::Memory* memory, CameraComponent const* other)
            : IComponent(memory, other), _view(other->_view), ZoomFactor(other->ZoomFactor) {
        }

        ~CameraComponent() override = default;

        void Initialize() override{};

        void Update(float deltaTime) override;

        /**
         * @brief Set the size of the view, in Units.
         *
         * In default setting, Units are Pixel equivalent, but are scaled by ZoomFactor.
         * @param windowSize
         */
        void SetWindowSize(sf::Vector2f windowSize);

        /**
         * @brief Set thi Component's View to provided window.
         * @param window Reference to the Window that will have Component's view assigned to.
         */
        void SetView(sf::RenderWindow& window);

    protected:
        sf::View _view;
    };
}
