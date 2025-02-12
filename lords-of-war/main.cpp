#include "../low-engine/LowEngine.h"

class PlayerCustomComponent : public LowEngine::ECS::IComponent {
public:
    int Health = 100;

    PlayerCustomComponent() = default;

    ~PlayerCustomComponent() override {

    }

    void Initialize() override {

    }

    void Update(float deltaTime) override {

    }
};

int main() {
    LowEngine::LowEngine engine;

    bool success = engine.OpenWindow("Lords of war", 800, 600);
    if (!success) return 1;

    engine.Assets.LoadAllFromPath("/assets");

    LowEngine::Scene& mainScene = engine.Scenes.CreateScene("main scene");

    uint32_t playerId = mainScene.AddEntity("player");
    mainScene.AddComponent<LowEngine::ECS::TransformComponent>(playerId);
    mainScene.AddComponent<PlayerCustomComponent>(playerId);

    auto& playerComponent = mainScene.GetComponent<PlayerCustomComponent>(playerId);
    playerComponent.Health = 200;

    mainScene.GetComponent<LowEngine::ECS::TransformComponent>(playerId).Position = sf::Vector2f(100.0f, 100.0f);

    auto& playerComponent2 = mainScene.GetComponent<PlayerCustomComponent>(playerId);

    while (engine.IsWindowOpen()) {
        engine.Update();
        engine.Draw();
    }

    return 0;
}
