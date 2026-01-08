#include "SoundComponent.h"

namespace LowEngine::ECS {
	void SoundComponent::SetSound(const std::string& soundAlias) {
		SoundId = Assets::GetSoundId(soundAlias);
		auto& newSound = Assets::GetSound(soundAlias);
		Sound.setBuffer(newSound);
	}

	void SoundComponent::Play() {
		Sound.play();
	}

	nlohmann::ordered_json SoundComponent::SerializeToJSON() {
		nlohmann::ordered_json json = IComponent::SerializeToJSON();
		json["sound_id"] = SoundId;
		return json;
	}

	bool SoundComponent::DeserializeFromJSON(const nlohmann::ordered_json& jsonData) {
		if (!IComponent::DeserializeFromJSON(jsonData)) {
			_log->error("SoundComponent: Failed to deserialize base IComponent data.");
			return false;
		}
		if (jsonData.contains("sound_id")) {
			SoundId = jsonData["sound_id"].get<size_t>();
			auto& newSound = Assets::GetSound(SoundId);
			Sound.setBuffer(newSound);
		} else {
			_log->error("SoundComponent deserialization failed: missing 'sound_id' field.");
			return false;
		}
		return true;
	}
}
