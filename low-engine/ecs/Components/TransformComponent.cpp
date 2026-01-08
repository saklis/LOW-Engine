#include "TransformComponent.h"

namespace LowEngine::ECS {
	nlohmann::ordered_json TransformComponent::SerializeToJSON() {
		nlohmann::ordered_json json = IComponent::SerializeToJSON();
		json["Position"] = {{"x", Position.x}, {"y", Position.y}};
		json["Rotation"] = Rotation.asDegrees();
		json["Scale"] = {{"x", Scale.x}, {"y", Scale.y}};
		return json;
	}

	bool TransformComponent::DeserializeFromJSON(const nlohmann::ordered_json& jsonData) {
		if (!IComponent::DeserializeFromJSON(jsonData)) {
			_log->error("TransformComponent deserialization failed: base component data not set.");
			return false;
		}

		if (jsonData.contains("Position")) {
			auto posJson = jsonData["Position"];
			if (posJson.contains("x")) {
				Position.x = posJson["x"].get<float>();
			} else {
				_log->error("TransformComponent deserialization failed: 'Position.x' field is missing.");
				return false;
			}
			if (posJson.contains("y")) {
				Position.y = posJson["y"].get<float>();
			} else {
				_log->error("TransformComponent deserialization failed: 'Position.y' field is missing.");
				return false;
			}
		} else {
			_log->error("TransformComponent deserialization failed: 'Position' field is missing.");
			return false;
		}
		if (jsonData.contains("Rotation")) {
			Rotation = sf::degrees(jsonData["Rotation"].get<float>());
		} else {
			_log->error("TransformComponent deserialization failed: 'Rotation' field is missing.");
			return false;
		}
		if (jsonData.contains("Scale")) {
			auto scaleJson = jsonData["Scale"];
			if (scaleJson.contains("x")) {
				Scale.x = scaleJson["x"].get<float>();
			} else {
				_log->error("TransformComponent deserialization failed: 'Scale.x' field is missing.");
				return false;
			}
			if (scaleJson.contains("y")) {
				Scale.y = scaleJson["y"].get<float>();
			} else {
				_log->error("TransformComponent deserialization failed: 'Scale.y' field is missing.");
				return false;
			}
		} else {
			_log->error("TransformComponent deserialization failed: 'Scale' field is missing.");
			return false;
		}

		return true;
	}
}
