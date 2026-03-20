#pragma once

#include <SFML/Audio/Sound.hpp>

#include "ecs/IComponent.h"

namespace LowEngine::ECS
{
	class SoundCueComponent : public IComponent<SoundCueComponent>
	{
	public:
		explicit SoundCueComponent(Memory::Memory* memory)
			: IComponent(memory) {
		}
		SoundCueComponent(Memory::Memory* memory, SoundCueComponent const* other)
			: IComponent(memory, other) {
		}
		~SoundCueComponent() override = default;

		void Initialize() override{}
		void Update(float deltaTime) override {}
		Sprite* Draw() override { return nullptr; }

		nlohmann::ordered_json SerializeToJSON() override;
		bool DeserializeFromJSON(const nlohmann::ordered_json& jsonData) override;

		void Play(size_t soundId);
		void Play(const std::string& soundAlias);

		void AddSound(size_t soundId);
		void AddSound(const std::string& soundAlias);

		void RemoveSound(size_t soundId);
		void RemoveSound(const std::string& soundAlias);

		const std::unordered_map<std::size_t, std::unique_ptr<sf::Sound>>& GetSoundCues() const { return _soundCues; }

	protected:
		std::unordered_map<std::size_t, std::unique_ptr<sf::Sound>> _soundCues;
	};
}
