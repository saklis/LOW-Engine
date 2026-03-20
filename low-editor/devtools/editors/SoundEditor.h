#pragma once
#include <cstddef>

namespace LowEngine {
	class Scene;

	namespace Editors {
		void AddSoundComponent(Scene* scene, std::size_t entityId);
		void DrawSoundEditor(Scene* scene, std::size_t entityId);
	}
}
