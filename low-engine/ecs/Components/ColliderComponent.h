#pragma once

#include "box2d/box2d.h"

#include "ecs/IComponent.h"
#include "ecs/Components/TransformComponent.h"
#include "graphics/Sprite.h"

namespace LowEngine::ECS {
	/**
	 * Types of colliders.
	 */
	enum class ColliderType {
		Static,
		Dynamic,
		Kinematic
	};

	/**
	 * Represents a component that provides collision capabilities.
	 *
	 * Depends on Transform Component for collider's position.
	 */
	class ColliderComponent : public IComponent<ColliderComponent, TransformComponent> {
	public:

		/**
		 * @brief Should collision shapes be drawn as overlay in the scene?
		 */
		bool DrawCollisionOverlay = false;

		explicit ColliderComponent(Memory::Memory* memory)
			: IComponent(memory), _sprite(Assets::GetDefaultTexture()) {
			_sprite.Layer = Config::DRAW_OVERLAY_LAYER_ID;
			if (!_renderTexture.resize({256, 256})) {
				_log->error("Failed to resize render texture for collision overlay.");
			}
		}

		ColliderComponent(Memory::Memory* memory, const ColliderComponent* other)
			: IComponent(memory, other), _sprite(Assets::GetDefaultTexture()) {
			_sprite.Layer = Config::DRAW_OVERLAY_LAYER_ID;
			if (!_renderTexture.resize({256, 256})) {
				_log->error("Failed to resize render texture for collision overlay.");
			}
		}

		~ColliderComponent() override;

		void Initialize() override {}

		void Update(float deltaTime) override;

		LowEngine::Sprite* Draw() override;

		nlohmann::ordered_json SerializeToJSON() override;

		bool DeserializeFromJSON(const nlohmann::ordered_json& jsonData) override;

		bool CreateBoxCollider(float halfWidth, float halfHeight);

	protected:
		/**
		 * @brief Type of this collider.
		 *
		 * Static colliders do not move.
		 * Dynamic colliders are affected by physics.
		 * Kinematic colliders can be moved manually, but are not affected by physics.
		 */
		ColliderType _type = ColliderType::Kinematic;

		/**
		 * @brief Box2D Body ID associated with this collider.
		 */
		b2BodyId _bodyId = b2_nullBodyId;

		/**
		 * @brief Component's Sprite used for collision overlay.
		 */
		LowEngine::Sprite _sprite;

		/**
		 * @brief texture used to generate an overlay image.
		 */
		sf::RenderTexture _renderTexture;
	};
}
