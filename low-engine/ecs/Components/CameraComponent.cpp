#include "CameraComponent.h"

#include "memory/Memory.h"

namespace LowEngine::ECS {
	void CameraComponent::Update(float deltaTime) {
		auto transformComponent = _memory->GetComponent<TransformComponent>(EntityId);
		if (transformComponent) {
			_view.setCenter(transformComponent->Position);
			_view.setRotation(transformComponent->Rotation);
		}
	}

	nlohmann::ordered_json CameraComponent::SerializeToJSON() {
		nlohmann::ordered_json json = IComponent::SerializeToJSON();
		json["zoom_factor"] = ZoomFactor;
		return json;
	}

	bool CameraComponent::DeserializeFromJSON(const nlohmann::ordered_json& jsonData) {
		if (!IComponent::DeserializeFromJSON(jsonData)) {
			_log->error("CameraComponent deserialization failed: base component data not set.");
			return false;
		}
		if (jsonData.contains("zoom_factor")) {
			ZoomFactor = jsonData["zoom_factor"].get<float>();
		} else {
			_log->error("CameraComponent deserialization failed: missing 'zoom_factor' field.");
			return false;
		}
		return true;
	}

	void CameraComponent::SetWindowSize(sf::Vector2f windowSize) {
		_view.setSize(windowSize);
	}

	void CameraComponent::SetView(sf::RenderWindow& window) {
		auto size = window.getSize();
		_view.setSize({size.x * ZoomFactor, size.y * ZoomFactor});
		window.setView(_view);
	}
}
