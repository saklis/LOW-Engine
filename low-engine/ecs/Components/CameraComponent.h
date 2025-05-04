#pragma once
#include "TransformComponent.h"
#include "ecs/IComponent.h"
#include "SFML/Graphics/View.hpp"

namespace LowEngine::ECS {
    class CameraComponent : public IComponent {
    public:
        sf::Vector2f ViewSize = sf::Vector2f(0.0f, 0.0f);
        float ZoomFactor = 1.0f;

        explicit CameraComponent(Memory::Memory* memory)
            : IComponent(memory) {
        }

        CameraComponent(Memory::Memory* memory, CameraComponent const* other)
            : IComponent(memory, other), _view(other->_view), ViewSize(other->ViewSize), ZoomFactor(other->ZoomFactor) {
        }

        ~CameraComponent() override = default;

        void CloneInto(Memory::Memory* newMemory, void* rawStorage) const override {
            new(rawStorage) CameraComponent(newMemory, this);
        }

        static const std::vector<std::type_index>& Dependencies() {
            static std::vector<std::type_index> dependencies = {
                std::type_index(typeid(TransformComponent))
            };
            return dependencies;
        }

        void Initialize() override{};

        void Update(float deltaTime) override;

        void SetWindowSize(sf::Vector2f windowSize);

        void SetView(sf::RenderWindow& window);

    protected:
        sf::View _view;
    };
}
