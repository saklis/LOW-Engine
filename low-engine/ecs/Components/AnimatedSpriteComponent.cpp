#include "AnimatedSpriteComponent.h"

namespace LowEngine::ECS {
	void AnimatedSpriteComponent::SetTexture(const std::string& textureAlias) {
		if (!Assets::HasSpriteSheet(textureAlias)) {
			_log->error("Cannot set texture alias {}. No sprite sheet with this alias exists.", textureAlias);
			throw std::runtime_error("No sprite sheet with this alias exists");
		}

		SetTexture(Assets::GetTextureId(textureAlias));
		UpdateFrameSize();
	}

	void AnimatedSpriteComponent::SetTexture(size_t textureId) {
		if (!Assets::HasSpriteSheet(textureId)) {
			_log->error("Cannot set texture alias {}. No sprite sheet with this alias exists.", textureId);
			throw std::runtime_error("No sprite sheet with this alias exists");
		}

		TextureId = textureId;
		SetTexture(Assets::GetTexture(textureId));
		UpdateFrameSize();
	}

	void AnimatedSpriteComponent::Play(const std::string& animationName, bool loop) {
		if (Assets::HasSpriteSheet(TextureId) == false) {
			_log->error("Cannot play animation {}. No sprite sheet is not set.", animationName);
			return;
		}
		auto& Sheet = Assets::GetSpriteSheet(TextureId);

		if (Sheet.HasAnimationClip(animationName) == false) {
			_log->error("Cannot play animation {}. Animation clip does not exist.", animationName);
			return;
		}
		auto& Clip = Sheet.GetAnimationClip(animationName);

		CurrentClipName = animationName;
		CurrentFrame = 0;
		FrameTime = 0.0f;
		Loop = loop;

		Sprite.setTextureRect(Clip.Frames[CurrentFrame]);
	}

	void AnimatedSpriteComponent::Stop() {
		CurrentClipName.clear();
		CurrentFrame = 0;
		FrameTime = 0.0f;
	}

	void AnimatedSpriteComponent::Update(float deltaTime) {
		auto transformComponent = _memory->GetComponent<TransformComponent>(EntityId);
		Sprite.setPosition(transformComponent->Position);
		Sprite.setRotation(transformComponent->Rotation);
		Sprite.setScale(transformComponent->Scale);
		Sprite.Layer = Layer;

		if (CurrentClipName.empty()) return;

		auto& Sheet = Assets::GetSpriteSheet(TextureId);
		auto& Clip = Sheet.GetAnimationClip(CurrentClipName);

		FrameTime += deltaTime;
		if (FrameTime >= Clip.FrameDuration) {
			FrameTime = 0.0f;
			CurrentFrame++;
			if (CurrentFrame >= Clip.FrameCount) {
				if (Loop) {
					CurrentFrame = 0;
				} else {
					CurrentClipName.clear();
					return;
				}
			}
		}

		Sprite.setTextureRect(Clip.Frames[CurrentFrame]);
	}

	nlohmann::ordered_json AnimatedSpriteComponent::SerializeToJSON() {
		nlohmann::ordered_json json = IComponent::SerializeToJSON();
		json["TextureId"] = TextureId;
		json["Layer"] = Layer;
		json["CurrentClipName"] = CurrentClipName;
		json["CurrentFrame"] = CurrentFrame;
		json["FrameTime"] = FrameTime;
		json["Loop"] = Loop;
		return json;
	}

	bool AnimatedSpriteComponent::DeserializeFromJSON(const nlohmann::ordered_json& jsonData) {
		if (!IComponent::DeserializeFromJSON(jsonData)) {
			_log->error("AnimatedSpriteComponent deserialization failed: base component data not set.");
			return false;
		}
		if (jsonData.contains("TextureId")) {
			TextureId = jsonData["TextureId"].get<size_t>();
			SetTexture(TextureId);
		} else {
			_log->error("AnimatedSpriteComponent deserialization failed: 'TextureId' field is missing.");
			return false;
		}
		if (jsonData.contains("Layer")) {
			Layer = jsonData["Layer"].get<int>();
		} else {
			_log->error("AnimatedSpriteComponent deserialization failed: 'Layer' field is missing.");
			return false;
		}
		if (jsonData.contains("CurrentClipName")) {
			CurrentClipName = jsonData["CurrentClipName"].get<std::string>();
		} else {
			_log->error("AnimatedSpriteComponent deserialization failed: 'CurrentClipName' field is missing.");
			return false;
		}
		if (jsonData.contains("CurrentFrame")) {
			CurrentFrame = jsonData["CurrentFrame"].get<size_t>();
		} else {
			_log->error("AnimatedSpriteComponent deserialization failed: 'CurrentFrame' field is missing.");
			return false;
		}
		if (jsonData.contains("FrameTime")) {
			FrameTime = jsonData["FrameTime"].get<float>();
		} else {
			_log->error("AnimatedSpriteComponent deserialization failed: 'FrameTime' field is missing.");
			return false;
		}
		if (jsonData.contains("Loop")) {
			Loop = jsonData["Loop"].get<bool>();
		} else {
			_log->error("AnimatedSpriteComponent deserialization failed: 'Loop' field is missing.");
			return false;
		}
		return true;
	}

	void AnimatedSpriteComponent::SetTexture(const sf::Texture& texture) {
		Sprite.setTexture(texture);

		auto size = static_cast<sf::Vector2<int>>(texture.getSize());
		Sprite.setTextureRect(sf::IntRect({0, 0}, size));
		Sprite.setOrigin({static_cast<float>(size.x) / 2, static_cast<float>(size.y) / 2});
	}

	void AnimatedSpriteComponent::UpdateFrameSize() {
		auto& sheet = Assets::GetSpriteSheet(TextureId);
		auto size = sf::Vector2<int>(static_cast<int>(sheet.FrameSize.x), static_cast<int>(sheet.FrameSize.y));
		Sprite.setTextureRect(sf::IntRect({0, 0}, size));
		Sprite.setOrigin({static_cast<float>(size.x) / 2.0f, static_cast<float>(size.y) / 2.0f});
	}
}
