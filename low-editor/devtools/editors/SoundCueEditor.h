#pragma once
#include <cstddef>

namespace LowEngine {
	class Scene;

	namespace Editors {
		void AddSoundCueComponent(Scene* scene, std::size_t entityId);
		void DrawSoundCueEditor(Scene* scene, std::size_t entityId);
	}
}
