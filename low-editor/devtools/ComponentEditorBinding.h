#pragma once
#include <memory>
#include <string>
#include <functional>

#include "ecs/IComponent.h"

namespace LowEngine {
	class Scene;

	class ComponentEditorBinding {
	public:
		std::string Label;
		std::type_index ComponentType;
		std::function<void(Scene*, std::size_t)> AddComponent;
		std::function<void(Scene*, std::size_t)> DrawEditor;

		ComponentEditorBinding(std::string label, std::type_index componentType, std::function<void(Scene*, std::size_t)> addComponent, std::function<void(Scene*, std::size_t)> drawEditor) :
			Label(label), ComponentType(componentType), AddComponent(addComponent), DrawEditor(std::move(drawEditor)) {}

		// explicit move semantics, no copy semantics
		ComponentEditorBinding(ComponentEditorBinding&& other) = default;
		ComponentEditorBinding& operator=(ComponentEditorBinding&& other) = default;
		ComponentEditorBinding(const ComponentEditorBinding& other) = delete;
		ComponentEditorBinding& operator=(const ComponentEditorBinding& other) = delete;
	};
}
