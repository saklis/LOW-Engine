#include "ecs/Components/ColliderComponent.h"

#include "memory/Memory.h"

namespace LowEngine::ECS {
	ColliderComponent::~ColliderComponent() {
		if (B2_IS_NON_NULL(_bodyId) && b2Body_IsValid(_bodyId)) {
			b2DestroyBody(_bodyId);
			_bodyId = b2_nullBodyId;
		}
	}

	void ColliderComponent::Update(float deltaTime) {
		
	}

	void ColliderComponent::FixedUpdate(float fixedDeltaTime)
	{
		if (_type == ColliderType::Kinematic && B2_IS_NON_NULL(_bodyId)) {
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

				debugShape.setOutlineColor(sf::Color::Green);
				debugShape.setOutlineThickness(1.f);
				debugShape.setFillColor(sf::Color(0, 255, 0, 128)); // semi-transparent green

				for (int i=0; i<polygon.count; i++) {
					// center shape around texture's center
					float x = polygon.vertices[i].x + center.x;
					float y = polygon.vertices[i].y + center.y;

					debugShape.setPoint(i, sf::Vector2f(x, y));
				}

				_renderTexture.draw(debugShape);
			}

			_renderTexture.display();
			
			_sprite.setTexture(_renderTexture.getTexture(), true);
			_sprite.setOrigin({center.x, center.y});

			_sprite.setPosition({position.x, position.y});
			_sprite.setRotation(sf::radians(angleRad));
		}
	}

	LowEngine::Sprite* ColliderComponent::Draw() {
		if (DrawCollisionOverlay && B2_IS_NON_NULL(_bodyId)) {
			return &_sprite;
		}

		return nullptr;
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

	bool ColliderComponent::CreateBoxCollider(float halfWidth, float halfHeight) {
		if (B2_IS_NON_NULL(_bodyId)) {
			_log->warn("ColliderComponent::CreateBoxCollider called, but body already exists.");
			return false;
		}

		// create body
		auto bodyDef = b2DefaultBodyDef();
		bodyDef.type = b2BodyType::b2_kinematicBody;
		_type = ColliderType::Kinematic;

		auto transform = _memory->GetComponent<TransformComponent>(EntityId);
		bodyDef.position = b2Vec2{transform->Position.x, transform->Position.y};

		_bodyId = b2CreateBody(_memory->Box2dWorldId, &bodyDef);

		// create shape
		auto shape = b2MakeBox(halfWidth, halfHeight);
		auto shapeDef = b2DefaultShapeDef();
		shapeDef.enableContactEvents = true;
		shapeDef.isSensor = true;
		shapeDef.enableSensorEvents = true;
		b2CreatePolygonShape(_bodyId, &shapeDef, &shape);

		_log->debug("ColliderComponent: Box collider created with size {0}x{1}.", halfWidth * 2, halfHeight * 2);

		return true;
	}

	float ColliderComponent::shortestAngleDiff(float a, float b) {
		float d = b - a;
		while (d > B2_PI) d -= 2.0f * B2_PI;
		while (d < -B2_PI) d += 2.0f * B2_PI;
		return d;
	}
}
