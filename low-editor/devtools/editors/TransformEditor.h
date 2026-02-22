#pragma once
#include <cstddef>

namespace LowEngine{
	class Scene;

	namespace Editors {
		void AddTransformComponent(Scene* scene, std::size_t entityId);
		void DrawTransformEditor(Scene* scene, std::size_t entityId);
	}
	
}
