#include "ImageTextButton.h"

#include <imgui_internal.h>

namespace LowEngine::Controls {
	bool ImageTextButton(const char* id, ImTextureID textureNativeHandle, const ImVec2& imageSize, const char* text, float imageTextSpacing)
	{
        const ImGuiStyle& style = ImGui::GetStyle();
        const ImVec2 textSize = ImGui::CalcTextSize(text);
        const float contentHeight = ImMax(imageSize.y, textSize.y);

        const ImVec2 buttonSize(
            imageSize.x + imageTextSpacing + textSize.x + style.FramePadding.x * 2.0f,
            contentHeight + style.FramePadding.y * 2.0f
        );

        const bool pressed = ImGui::Button(id, buttonSize);

        ImDrawList* drawList = ImGui::GetWindowDrawList();
        const ImVec2 rectMin = ImGui::GetItemRectMin();
        const ImVec2 rectMax = ImGui::GetItemRectMax();

        const ImVec2 contentMin(rectMin.x + style.FramePadding.x, rectMin.y + style.FramePadding.y);
        const float contentH = rectMax.y - rectMin.y - style.FramePadding.y * 2.0f;

        const ImVec2 imageMin(
            contentMin.x,
            contentMin.y + (contentH - imageSize.y) * 0.5f
        );
        const ImVec2 imageMax(imageMin.x + imageSize.x, imageMin.y + imageSize.y);

        drawList->AddImage(textureNativeHandle, imageMin, imageMax);

        const ImVec2 textPos(
            imageMax.x + imageTextSpacing,
            contentMin.y + (contentH - textSize.y) * 0.5f
        );
        drawList->AddText(textPos, ImGui::GetColorU32(ImGuiCol_Text), text);

        return pressed;
	}
}
