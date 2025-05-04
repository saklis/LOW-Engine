#pragma once

#include <SFML/Audio.hpp>
#include "assets/Assets.h"
#include "ecs/IComponent.h"

namespace LowEngine::ECS {
    class SoundComponent : public IComponent {
    public:
        sf::Sound Sound;

        explicit SoundComponent(Memory::Memory* memory)
            : IComponent(memory), Sound(Assets::GetDefaultSound()) {
        }

        SoundComponent(Memory::Memory* memory, SoundComponent const* other)
            : IComponent(memory, other), Sound(other->Sound) {
        }

        ~SoundComponent() override = default;

        void CloneInto(Memory::Memory* newMemory, void* rawStorage) const override {
            new(rawStorage) SoundComponent(newMemory, this);
        }

        void SetSound(const std::string& soundAlias);

        void Play();

        static const std::vector<std::type_index>& Dependencies() {
            static std::vector<std::type_index> dependencies = {};
            return dependencies;
        }

        void Initialize() override {
        }

        void Update(float deltaTime) override {
        }

        Sprite* Draw() override { return nullptr; }

    protected:
    };
}
