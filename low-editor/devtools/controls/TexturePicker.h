#pragma once

namespace LowEngine::ECS {
	class SpriteComponent;
}

namespace LowEngine::Controls {
	void TexturePickerPopup(const char* popup_str_id, LowEngine::ECS::SpriteComponent* sc);
}
