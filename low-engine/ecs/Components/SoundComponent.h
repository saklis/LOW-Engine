#pragma once

#include <SFML/Audio.hpp>
#include "assets/Assets.h"
#include "ecs/IComponent.h"

namespace LowEngine::ECS {
    /**
     * @brief Represents a component that play a sound.
     */
    class SoundComponent : public IComponent<SoundComponent> {
    public:
		size_t SoundId = 0;

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

        /**
         * @brief Set sound to be played.
         * @param soundAlias Alias of the sound to play.
         */
        void SetSound(const std::string& soundAlias);

        /**
         * @brief Play currently set sound.
         */
        void Play();

        void Initialize() override {
        }

        void Update(float deltaTime) override {
        }

        Sprite* Draw() override { return nullptr; }

        nlohmann::ordered_json SerializeToJSON() override;
		bool DeserializeFromJSON(const nlohmann::ordered_json& jsonData) override;

    protected:
    };
}
