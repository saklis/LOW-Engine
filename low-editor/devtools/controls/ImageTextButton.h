#pragma once

#include <imgui.h>

namespace LowEngine::Controls {
	bool ImageTextButton(const char* id, ImTextureID textureNativeHandle, const ImVec2& imageSize, const char* text, float imageTextSpacing = 6.0f);
}
