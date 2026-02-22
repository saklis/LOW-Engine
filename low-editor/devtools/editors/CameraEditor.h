#pragma once
#include <cstddef>

namespace LowEngine {
	class Scene;

	namespace Editors {
		void AddCameraComponent(Scene* scene, std::size_t entityId);
		void DrawCameraEditor(Scene* scene, std::size_t entityId);
	}

}
