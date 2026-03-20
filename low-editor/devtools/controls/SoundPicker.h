#pragma once
#include <cstddef>

namespace LowEngine::ECS {
	class SoundComponent;
}

namespace LowEngine::Controls {
	void SoundPickerPopup(const char* popup_str_id, LowEngine::ECS::SoundComponent* sc);
	void SoundPickerPopup(const char* popup_str_id, /*out*/ std::size_t& selectedSoundId);
}