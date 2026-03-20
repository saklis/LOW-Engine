#pragma once
#include <cstddef>

namespace LowEngine::ECS {
	class AnimatedSpriteComponent;
	class SpriteComponent;
}

namespace LowEngine::Controls {
	void TexturePickerPopup(const char* popup_str_id, LowEngine::ECS::SpriteComponent* sc);
	void TexturePickerPopup(const char* popup_str_id, LowEngine::ECS::AnimatedSpriteComponent* asc);
	void TexturePickerPopup(const char* popup_str_id, std::size_t& out_textureId);
}
