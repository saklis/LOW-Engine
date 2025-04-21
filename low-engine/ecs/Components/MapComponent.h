#pragma once

#include "ecs/IComponent.h"
#include "TransformComponent.h"
#include "graphics/Sprite.h"

namespace LowEngine::ECS {
    class MapComponent : public IComponent {
    public:
        int Layer = 0;

        explicit MapComponent(Memory::Memory* memory)
            : IComponent(memory), m_Sprite(Assets::GetDefaultTexture()) {
        }

        ~MapComponent() override = default;

        static const std::vector<std::type_index>& Dependencies() {
            static std::vector dependencies = {
                std::type_index(typeid(TransformComponent))
            };
            return dependencies;
        }

        void Initialize() override {
        }

        void Update(float deltaTime) override;

        LowEngine::Sprite* Draw() override;

        void SetMapId(size_t mapId);

    protected:
        size_t m_MapId = -1;

        Sprite m_Sprite;
        sf::RenderTexture m_Texture;
    };
}
