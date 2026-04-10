#include "ecs/Components/ColliderComponent.h"

#include "memory/Memory.h"

namespace LowEngine::ECS {
	ColliderComponent::~ColliderComponent() {
		if (B2_IS_NON_NULL(_bodyId) && b2Body_IsValid(_bodyId)) {
			b2DestroyBody(_bodyId);
			_bodyId = b2_nullBodyId;
			_shapeId = b2_nullShapeId;
		}
	}

	void ColliderComponent::Update(float deltaTime) {
		
	}

	void ColliderComponent::FixedUpdate(float fixedDeltaTime)
	{
		if (Type == ColliderType::Kinematic && B2_IS_NON_NULL(_bodyId)) {
			auto transform = _memory->GetComponent<TransformComponent>(EntityId);
			
			b2Vec2 currentPos = b2Body_GetPosition(_bodyId);
			b2Rot currentRot = b2Body_GetRotation(_bodyId);
			float currentAng = b2Rot_GetAngle(currentRot);
			
			b2Vec2 targetPos = { transform->Position.x, transform->Position.y };
			b2Rot targetRot = b2MakeRot(transform->Rotation.asRadians());
			float targetAng = b2Rot_GetAngle(targetRot);
			
			float dx = targetPos.x - currentPos.x;
			float dy = targetPos.y - currentPos.y;
			float distance2 = dx*dx + dy*dy;
			
			float dAng = fabsf(shortestAngleDiff(currentAng, targetAng));
			
			if (distance2 <= _snapLinear2 && dAng <= _snapAngular) {
				// close enough - snap transforms to avoid jiggle
				b2Body_SetTransform(_bodyId, targetPos, targetRot);
				b2Body_SetLinearVelocity(_bodyId, b2Vec2_zero);
				b2Body_SetAngularVelocity(_bodyId, 0.0f);
			}
			else if (distance2 >= _teleportLinear2) {
				// large jump - ignore collision on the way
				b2Body_SetTransform(_bodyId, targetPos, targetRot);
				b2Body_SetLinearVelocity(_bodyId, b2Vec2_zero);
				b2Body_SetAngularVelocity(_bodyId, 0.0f);
			}
			else {
				// normal case - set velocity to detect collision on the way
				b2Body_SetTargetTransform(_bodyId, {targetPos, targetRot}, fixedDeltaTime, true);
			}
		}

		if (DrawCollisionOverlay && B2_IS_NON_NULL(_bodyId)) {
			auto position = b2Body_GetPosition(_bodyId);
			auto rotation = b2Body_GetRotation(_bodyId);
			auto angleRad = b2Rot_GetAngle(rotation);

			_renderTexture.clear(sf::Color::Transparent);

			sf::Vector2f center(
				_renderTexture.getSize().x * 0.5f,
				_renderTexture.getSize().y * 0.5f
			);

			auto shapeCount = b2Body_GetShapeCount(_bodyId);
			std::vector<b2ShapeId> shapes(shapeCount);
			b2Body_GetShapes(_bodyId, shapes.data(), shapeCount);

			for (auto shapeId : shapes) {
				auto polygon = b2Shape_GetPolygon(shapeId);

				sf::ConvexShape debugShape;
				debugShape.setPointCount(polygon.count);
				debugShape.setOutlineThickness(0.0f); // disable outline for now, as it can cause calculation errors while shape is being constructed.

				for (int i=0; i<polygon.count; i++) {
					// center shape around texture's center
					float x = polygon.vertices[i].x + center.x;
					float y = polygon.vertices[i].y + center.y;

					debugShape.setPoint(i, sf::Vector2f(x, y));
				}

				debugShape.setOutlineThickness(1.0f);
				debugShape.setOutlineColor(sf::Color::Green);
				debugShape.setFillColor(sf::Color(0, 255, 0, 128)); // semi-transparent green

				_renderTexture.draw(debugShape);
			}

			_renderTexture.display();
			
			_sprite.setTexture(_renderTexture.getTexture(), true);
			_sprite.setOrigin({center.x, center.y});

			_sprite.setPosition({position.x, position.y});
			_sprite.setRotation(sf::radians(angleRad));
		}
	}

	void ColliderComponent::Draw(/* out */std::vector<LowEngine::Sprite>& sprites) {
		if (DrawCollisionOverlay && B2_IS_NON_NULL(_bodyId)) {
			sprites.emplace_back(_sprite);
		}
	}

	nlohmann::ordered_json ColliderComponent::SerializeToJSON() {
		nlohmann::ordered_json json = IComponent::SerializeToJSON();
		json["DrawCollisionOverlay"] = DrawCollisionOverlay;
		return json;
	}

	bool ColliderComponent::DeserializeFromJSON(const nlohmann::ordered_json& jsonData) {
		if (!IComponent::DeserializeFromJSON(jsonData)) {
			_log->error("ColliderComponent deserialization failed: base component data not set.");
			return false;
		}

		if (jsonData.contains("DrawCollisionOverlay")) {
			DrawCollisionOverlay = jsonData["DrawCollisionOverlay"].get<bool>();
		}
		else {
			_log->error("ColliderComponent deserialization failed: 'DrawCollisionOverlay' field missing.");
			return false;
		}

		return true;
	}

	bool ColliderComponent::HasBody() {
		return B2_IS_NON_NULL(_bodyId);
	}

	b2BodyId ColliderComponent::GetBodyId() {
		return _bodyId;
	}

	b2ShapeId ColliderComponent::GetShapeId() {
		return _shapeId;
	}

	bool ColliderComponent::CreateBoxCollider(float halfWidth, float halfHeight) {
		if (B2_IS_NON_NULL(_bodyId)) {
			_log->warn("ColliderComponent::CreateBoxCollider called, but body already exists.");
			return false;
		}

		// create body
		auto bodyDef = b2DefaultBodyDef();
		bodyDef.type = b2BodyType::b2_kinematicBody;
		Type = ColliderType::Kinematic;

		auto transform = _memory->GetComponent<TransformComponent>(EntityId);
		bodyDef.position = b2Vec2{transform->Position.x, transform->Position.y};

		_bodyId = b2CreateBody(_memory->Box2dWorldId, &bodyDef);

		// create shape
		auto shape = b2MakeBox(halfWidth, halfHeight);
		auto shapeDef = b2DefaultShapeDef();
		shapeDef.enableContactEvents = true;
		shapeDef.isSensor = true;
		shapeDef.enableSensorEvents = true;
		_shapeId = b2CreatePolygonShape(_bodyId, &shapeDef, &shape);

		_log->debug("ColliderComponent: Box collider created with size {0}x{1}.", halfWidth * 2, halfHeight * 2);

		return true;
	}

	void ColliderComponent::CopyColliderToB2World(b2WorldId worldId) {
		if (!b2World_IsValid(worldId)) {
			_log->error("ColliderComponent::CopyColliderToB2World failed: target world is invalid.");
			return;
		}

		// Source ids from the copied component (pointing to original world's objects)
		const b2BodyId sourceBodyId = _bodyId;
		const b2ShapeId sourceShapeId = _shapeId;

		if (B2_IS_NULL(sourceBodyId) || !b2Body_IsValid(sourceBodyId)) {
			// Nothing to copy
			_bodyId = b2_nullBodyId;
			_shapeId = b2_nullShapeId;
			return;
		}

		// Build body definition from source body runtime state
		b2BodyDef bodyDef = b2DefaultBodyDef();
		bodyDef.type = b2Body_GetType(sourceBodyId);
		bodyDef.position = b2Body_GetPosition(sourceBodyId);
		bodyDef.rotation = b2Body_GetRotation(sourceBodyId);
		bodyDef.linearVelocity = b2Body_GetLinearVelocity(sourceBodyId);
		bodyDef.angularVelocity = b2Body_GetAngularVelocity(sourceBodyId);
		bodyDef.linearDamping = b2Body_GetLinearDamping(sourceBodyId);
		bodyDef.angularDamping = b2Body_GetAngularDamping(sourceBodyId);
		bodyDef.gravityScale = b2Body_GetGravityScale(sourceBodyId);
		bodyDef.sleepThreshold = b2Body_GetSleepThreshold(sourceBodyId);
		bodyDef.enableSleep = b2Body_IsSleepEnabled(sourceBodyId);
		bodyDef.isAwake = b2Body_IsAwake(sourceBodyId);
		bodyDef.isBullet = b2Body_IsBullet(sourceBodyId);
		bodyDef.isEnabled = b2Body_IsEnabled(sourceBodyId);
		bodyDef.motionLocks = b2Body_GetMotionLocks(sourceBodyId);
		bodyDef.userData = b2Body_GetUserData(sourceBodyId);

		const char* sourceName = b2Body_GetName(sourceBodyId);
		if (sourceName != nullptr && sourceName[0] != '\0') {
			bodyDef.name = sourceName;
		}

		_bodyId = b2CreateBody(worldId, &bodyDef);
		if (B2_IS_NULL(_bodyId) || !b2Body_IsValid(_bodyId)) {
			_log->error("ColliderComponent::CopyColliderToB2World failed: body creation failed.");
			_bodyId = b2_nullBodyId;
			_shapeId = b2_nullShapeId;
			return;
		}

		// Keep component enum aligned with copied Box2D type
		switch (bodyDef.type) {
		case b2BodyType::b2_staticBody:
			Type = ColliderType::Static;
			break;
		case b2BodyType::b2_dynamicBody:
			Type = ColliderType::Dynamic;
			break;
		case b2BodyType::b2_kinematicBody:
		default:
			Type = ColliderType::Kinematic;
			break;
		}

		// Resolve source shape id (fallback: first shape on body)
		b2ShapeId shapeToCopy = sourceShapeId;
		if (B2_IS_NULL(shapeToCopy) || !b2Shape_IsValid(shapeToCopy)) {
			const int shapeCount = b2Body_GetShapeCount(sourceBodyId);
			if (shapeCount > 0) {
				std::vector<b2ShapeId> shapeIds(shapeCount);
				const int copied = b2Body_GetShapes(sourceBodyId, shapeIds.data(), shapeCount);
				if (copied > 0) {
					shapeToCopy = shapeIds[0];
				}
			}
		}

		if (B2_IS_NULL(shapeToCopy) || !b2Shape_IsValid(shapeToCopy)) {
			_log->warn("ColliderComponent::CopyColliderToB2World: source body has no valid shape.");
			_shapeId = b2_nullShapeId;
			return;
		}

		// Build shape definition from source shape runtime state
		b2ShapeDef shapeDef = b2DefaultShapeDef();
		shapeDef.userData = b2Shape_GetUserData(shapeToCopy);
		shapeDef.material = b2Shape_GetSurfaceMaterial(shapeToCopy);
		shapeDef.density = b2Shape_GetDensity(shapeToCopy);
		shapeDef.filter = b2Shape_GetFilter(shapeToCopy);
		shapeDef.isSensor = b2Shape_IsSensor(shapeToCopy);
		shapeDef.enableSensorEvents = b2Shape_AreSensorEventsEnabled(shapeToCopy);
		shapeDef.enableContactEvents = b2Shape_AreContactEventsEnabled(shapeToCopy);
		shapeDef.enablePreSolveEvents = b2Shape_ArePreSolveEventsEnabled(shapeToCopy);
		shapeDef.enableHitEvents = b2Shape_AreHitEventsEnabled(shapeToCopy);

		switch (b2Shape_GetType(shapeToCopy)) {
		case b2ShapeType::b2_circleShape: {
			const b2Circle circle = b2Shape_GetCircle(shapeToCopy);
			_shapeId = b2CreateCircleShape(_bodyId, &shapeDef, &circle);
			break;
		}
		case b2ShapeType::b2_capsuleShape: {
			const b2Capsule capsule = b2Shape_GetCapsule(shapeToCopy);
			_shapeId = b2CreateCapsuleShape(_bodyId, &shapeDef, &capsule);
			break;
		}
		case b2ShapeType::b2_segmentShape: {
			const b2Segment segment = b2Shape_GetSegment(shapeToCopy);
			_shapeId = b2CreateSegmentShape(_bodyId, &shapeDef, &segment);
			break;
		}
		case b2ShapeType::b2_polygonShape: {
			const b2Polygon polygon = b2Shape_GetPolygon(shapeToCopy);
			_shapeId = b2CreatePolygonShape(_bodyId, &shapeDef, &polygon);
			break;
		}
		default:
			_log->error("ColliderComponent::CopyColliderToB2World failed: unsupported shape type {}.",
				static_cast<int>(b2Shape_GetType(shapeToCopy)));
			_shapeId = b2_nullShapeId;
			break;
		}
	}

	float ColliderComponent::shortestAngleDiff(float a, float b) {
		float d = b - a;
		while (d > B2_PI) d -= 2.0f * B2_PI;
		while (d < -B2_PI) d += 2.0f * B2_PI;
		return d;
	}
}
