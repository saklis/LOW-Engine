#include "SpriteComponent.h"

namespace LowEngine::ECS {
	void SpriteComponent::SetTexture(const sf::Texture& texture) {
		Sprite.setTexture(texture);

		auto size = static_cast<sf::Vector2<int>>(texture.getSize());
		Sprite.setTextureRect(sf::IntRect({0, 0}, size));
		Sprite.setOrigin({static_cast<float>(size.x) / 2, static_cast<float>(size.y) / 2});
	}

	void SpriteComponent::Update(float deltaTime) {
		auto transformComponent = _memory->GetComponent<TransformComponent>(EntityId);
		Sprite.setPosition(transformComponent->Position);
		Sprite.setRotation(transformComponent->Rotation);
		Sprite.setScale(transformComponent->Scale);
		Sprite.Layer = Layer;
	}

	nlohmann::ordered_json SpriteComponent::SerializeToJSON() {
		nlohmann::ordered_json json = IComponent::SerializeToJSON();
		json["TextureId"] = TextureId;
		json["Layer"] = Layer;
		return json;
	}

	bool SpriteComponent::DeserializeFromJSON(const nlohmann::ordered_json& jsonData) {
		if (!IComponent::DeserializeFromJSON(jsonData)) {
			_log->error("SpriteComponent deserialization failed: base component data not set.");
			return false;
		}
		if (jsonData.contains("TextureId")) {
			TextureId = jsonData["TextureId"].get<size_t>();
			SetTexture(TextureId);
		} else {
			_log->error("SpriteComponent deserialization failed: missing 'TextureId' field.");
			return false;
		}
		if (jsonData.contains("Layer")) {
			Layer = jsonData["Layer"].get<int>();
		} else {
			_log->error("SpriteComponent deserialization failed: missing 'Layer' field.");
			return false;
		}
		
		return true;
	}

	void SpriteComponent::SetTexture(const std::string& textureAlias) {
		SetTexture(Assets::GetTextureId(textureAlias));
	}

	void SpriteComponent::SetTexture(size_t textureId) {
		TextureId = textureId;
		SetTexture(Assets::GetTexture(textureId));
	}
}
