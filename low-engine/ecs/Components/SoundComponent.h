#pragma once

#include <SFML/Audio.hpp>
#include "assets/Assets.h"
#include "ecs/IComponent.h"

namespace LowEngine::ECS {
    /**
     * @brief Represents a component that play a sound.
     */
    class SoundComponent : public IComponent {
    public:
        /**
         * @brief Instance of the Sound that manages the playback.
         */
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

        /**
         * @brief Set sound to be played.
         * @param soundAlias Alias of the sound to play.
         */
        void SetSound(const std::string& soundAlias);

        /**
         * @brief Play currently set sound.
         */
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
