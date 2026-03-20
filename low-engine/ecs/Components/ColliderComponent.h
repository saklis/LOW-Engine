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
	enum class ColliderType : std::uint8_t {
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
		 * @brief Type of this collider.
		 *
		 * Static colliders do not move.
		 * Dynamic colliders are affected by physics.
		 * Kinematic colliders can be moved manualy, but are not affected by physics.
		 */
		ColliderType Type = ColliderType::Kinematic;

		/**
		 * @brief Should collision shapes be drawn as overlay in the scene?
		 */
		bool DrawCollisionOverlay = false;

		explicit ColliderComponent(Memory::Memory* memory)
			: IComponent(memory), _sprite(Assets::GetDefaultTexture()) {
			_sprite.DrawOrder = Config::DRAW_OVERLAY_LAYER_ID;
			if (!_renderTexture.resize({256, 256})) {
				_log->error("Failed to resize render texture for collision overlay.");
			}
		}

		ColliderComponent(Memory::Memory* memory, const ColliderComponent* other)
			: IComponent(memory, other), _sprite(Assets::GetDefaultTexture()) {
			_sprite.DrawOrder = Config::DRAW_OVERLAY_LAYER_ID;
			if (!_renderTexture.resize({ 256, 256 })) {
				_log->error("Failed to resize render texture for collision overlay.");
			}

			 Type = other->Type;
			 DrawCollisionOverlay = other->DrawCollisionOverlay;

			 // Needed so CopyColliderToB2World() can clone from source world
			 _bodyId = other->_bodyId;
			 _shapeId = other->_shapeId;
		}

		~ColliderComponent() override;

		void Initialize() override {
		}

		void Update(float deltaTime) override;

		void FixedUpdate(float fixedDeltaTime) override;

		Sprite* Draw() override;

		nlohmann::ordered_json SerializeToJSON() override;

		bool DeserializeFromJSON(const nlohmann::ordered_json& jsonData) override;

		/**
		 * @brief Checks if this collider has an associated physics body.
		 *
		 * @return true if a physics body is associated, false otherwise.
		 */
		bool HasBody();

		/**
		 * @brief Retrieves the Box2D Body ID associated with this collider.
		 *
		 * @return The Box2D Body ID if it exists, or b2_nullBodyId if no body is associated.
		 */
		b2BodyId GetBodyId();

		/**
		 * @brief Retrieves the Box2D Shape ID associated with this collider.
		 *
		 * @return The Box2D Shape ID if it exists, or b2_nullShapeId if no shape is associated.
		 */
		b2ShapeId GetShapeId();

		/**
		 * @brief Creates a box collider with the specified half-width and half-height.
		 *
		 * This method creates a Box2D body and shape for this collider. The body is created as a kinematic body by default.
		 *
		 * @param halfWidth The half-width of the box collider in units.
		 * @param halfHeight The half-height of the box collider in units.
		 * @return true if the collider was successfully created, false if a body already exists or if creation failed.
		 */
		bool CreateBoxCollider(float halfWidth, float halfHeight);

		void CopyColliderToB2World(b2WorldId worldId);

	protected:
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
		 * @brief Box2D Shape ID associated with this collider.
		 */
		b2ShapeId _shapeId = b2_nullShapeId;

		/**
		 * @brief Component's Sprite used for collision overlay.
		 */
		Sprite _sprite;

		/**
		 * @brief texture used to generate an overlay image.
		 */
		sf::RenderTexture _renderTexture;

		// Tunable thresholds
		const float _snapLinear = 0.015f; // 1.5 cm
		const float _snapLinear2 = _snapLinear * _snapLinear;
		const float _snapAngular = 0.035f; // ~2 degrees (0.0349 rad)
		const float _teleportLinear = 0.25f; // 25 cm
		const float _teleportLinear2 = _teleportLinear * _teleportLinear;

		float shortestAngleDiff(float a, float b);
	};
}
