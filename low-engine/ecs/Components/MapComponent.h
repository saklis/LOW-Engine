#pragma once

#include "ecs/IComponent.h"
#include "TransformComponent.h"
#include "graphics/Sprite.h"

namespace LowEngine::ECS {
    class MapComponent : public IComponent {
    public:
        int Layer = 0;

        explicit MapComponent(Memory::Memory* memory)
            : IComponent(memory), _sprite(Assets::GetDefaultTexture()) {
        }

        MapComponent(Memory::Memory* memory, MapComponent const* other)
            : IComponent(memory, other), _sprite(other->_sprite), _mapId(other->_mapId), Layer(other->Layer) {
        }

        ~MapComponent() override = default;

        void CloneInto(Memory::Memory* newMemory, void* rawStorage) const override {
            new(rawStorage) MapComponent(newMemory, this);
        }

        static const std::vector<std::type_index>& Dependencies() {
            static std::vector dependencies = {
                std::type_index(typeid(TransformComponent))
            };
            return dependencies;
        }

        void Initialize() override {
        }

        void Update(float deltaTime) override;

        Sprite* Draw() override;

        void SetMapId(size_t mapId);

    protected:
        size_t _mapId = -1;

        Sprite _sprite;
        sf::RenderTexture _texture;
    };
}
