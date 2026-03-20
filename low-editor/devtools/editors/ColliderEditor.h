#pragma once
#include <cstddef>

namespace LowEngine {
	class Scene;

	namespace Editors {
		void AddColliderComponent(Scene* scene, std::size_t entityId);
		void DrawColliderEditor(Scene* scene, std::size_t entityId);
	}

}
