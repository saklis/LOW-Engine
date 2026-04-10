#pragma once
#include <cstddef>

namespace LowEngine::ECS {
	class AnimatedSpriteComponent;
	class SpriteComponent;
}

namespace LowEngine::Controls {
	bool TexturePickerPopup(const char* popup_str_id, LowEngine::ECS::SpriteComponent* sc);
	bool TexturePickerPopup(const char* popup_str_id, LowEngine::ECS::AnimatedSpriteComponent* asc);
	bool TexturePickerPopup(const char* popup_str_id, std::size_t& out_textureId);
}
