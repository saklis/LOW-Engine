#include "SpriteComponent.h"

#include "memory/Memory.h"

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
		Sprite.DrawOrder = DrawOrder;
	}

	nlohmann::ordered_json SpriteComponent::SerializeToJSON() {
		nlohmann::ordered_json json = IComponent::SerializeToJSON();
		json["TextureAlias"] = Assets::GetTextureAlias(TextureId);
		json["DrawOrder"] = DrawOrder;
		return json;
	}

	bool SpriteComponent::DeserializeFromJSON(const nlohmann::ordered_json& jsonData) {
		if (!IComponent::DeserializeFromJSON(jsonData)) {
			_log->error("SpriteComponent deserialization failed: base component data not set.");
			return false;
		}
		if (jsonData.contains("TextureAlias")) {
			SetTexture(jsonData["TextureAlias"].get<std::string>());
		} else if (jsonData.contains("TextureId")) {
			// legacy: fallback to raw numeric ID (may assign wrong texture if load order changed)
			SetTexture(jsonData["TextureId"].get<size_t>());
		} else {
			_log->error("SpriteComponent deserialization failed: missing 'TextureAlias' field.");
			return false;
		}
		if (jsonData.contains("DrawOrder")) {
			DrawOrder = jsonData["DrawOrder"].get<int>();
		} else {
			_log->error("SpriteComponent deserialization failed: missing 'DrawOrder' field.");
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
