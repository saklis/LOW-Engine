#include "SoundCueComponent.h"

#include "SFML/Audio/Sound.hpp"

namespace LowEngine::ECS
{
	nlohmann::ordered_json SoundCueComponent::SerializeToJSON()
	{
		auto json = IComponent::SerializeToJSON();
		json["sound_cues"] = nlohmann::ordered_json::array();
		for (const auto& soundId : _soundCues | std::views::keys) {
			json["sound_cues"].push_back(soundId);
		}
		return json;
	}

	bool SoundCueComponent::DeserializeFromJSON(const nlohmann::ordered_json& jsonData)
	{
		if (!IComponent::DeserializeFromJSON(jsonData))
		{
			_log->error("SoundCueComponent: Failed to deserialize base IComponent data.");
			return false;
		}

		if (jsonData.contains("sound_cues"))
		{
			for (const auto& soundId : jsonData["sound_cues"]) {
				size_t id = soundId.get<size_t>();
				AddSound(id);
			}
		} else {
			_log->error("SoundCueComponent deserialization failed: missing 'sound_cues' field.");
			return false;
		}

		return true;
	}

	void SoundCueComponent::Play(size_t soundId)
	{
		auto it = _soundCues.find(soundId);
		if (it != _soundCues.end()) {
			it->second->play();
		}
		else {
			_log->error("SoundCueComponent: Sound ID {} not found in sound cues.", soundId);
		}
	}

	void SoundCueComponent::Play(const std::string& soundAlias)
	{
		size_t soundId = Assets::GetSoundId(soundAlias);
		Play(soundId);
	}

	void SoundCueComponent::AddSound(size_t soundId)
	{
		auto& sound = Assets::GetSound(soundId);
		_soundCues[soundId] = std::make_unique<sf::Sound>(sound);
	}

	void SoundCueComponent::AddSound(const std::string& soundAlias)
	{
		size_t soundId = Assets::GetSoundId(soundAlias);
		AddSound(soundId);
	}

	void SoundCueComponent::RemoveSound(size_t soundId)
	{
		auto it = _soundCues.find(soundId);
		if (it != _soundCues.end()) {
			_soundCues.erase(it);
		}
		else {
			_log->error("SoundCueComponent: Sound ID {} not found in sound cues.", soundId);
		}
	}

	void SoundCueComponent::RemoveSound(const std::string& soundAlias)
	{
		size_t soundId = Assets::GetSoundId(soundAlias);
		RemoveSound(soundId);
	}
}
