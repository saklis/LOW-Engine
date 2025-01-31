
#include "Scene.h"

LowEngine::Scene::Scene::Scene(std::string name): Name(std::move(name)) {
}

void LowEngine::Scene::Scene::Update() {
}

uint32_t LowEngine::Scene::Scene::AddEntity(const std::string& name) {
    return _memory.CreateEntity(name);
}

template<typename T, typename ... Args>
uint32_t LowEngine::Scene::Scene::AddComponent(uint32_t entityId, Args&&... args) {
    return _memory.CreateComponent<T>(entityId, std::forward<Args>(args)...);
}

template uint32_t LowEngine::Scene::Scene::AddComponent<LowEngine::ECS::TransformComponent>(uint32_t entityId);
