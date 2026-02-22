#pragma once

#include "box2d/box2d.h"

#include "EngineConfig.h"
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
		
		void FixedUpdate(float fixedDeltaTime) override;

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
		 * Kinematic colliders can be moved manualy, but are not affected by physics.
		 */
		ColliderType _type = ColliderType::Kinematic;

		/**
		 * @brief Precision for collision detection, in units.
		 * 
		 * This is a threshold for precise collision detection for each frame.
		 * If a body moved further than this during a single frame, it'll be teleported instead of simulated along the path.
		 */
		float _collisionPrecision = 0.001f;

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
		
		// Tunable thresholds (safe for Box2D contacts)
		const float _snapLinear = 0.015f; // 1.5 cm
		const float _snapLinear2 = _snapLinear * _snapLinear;
		const float _snapAngular = 0.035f; // ~2 degrees (0.0349 rad)
		const float _teleportLinear = 0.25f; // 25 cm (choose to taste)
		const float _teleportLinear2 = _teleportLinear * _teleportLinear;
		
		float shortestAngleDiff(float a, float b);
	};
}
