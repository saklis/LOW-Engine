
#include "Scene.h"

LowEngine::Scene::Scene(const std::string& name): Name(name) {
}

void LowEngine::Scene::InitAsDefault() {
    Active = true;
    Name = "Default scene";
}

void LowEngine::Scene::Update() {
}

LowEngine::ECS::Entity& LowEngine::Scene::AddEntity(const std::string& name) {
    return _memory.CreateEntity(name).Scene = this;
}

// uint32_t LowEngine::Scene::AddEntity(const std::string& name) {
//     return _memory.CreateEntity(name);
// }

template<typename T, typename ... Args>
T& LowEngine::Scene::AddComponent(uint32_t entityId, Args&&... args) {
    return _memory.CreateComponent<T>(entityId, std::forward<Args>(args)...);
}

template LowEngine::ECS::TransformComponent& LowEngine::Scene::AddComponent<LowEngine::ECS::TransformComponent>(uint32_t entityId);
