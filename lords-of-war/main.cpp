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

    engine.Assets.LoadTexture("assets/textures/units/unit1.png", "unit1");

    LowEngine::Scene& mainScene = engine.Scenes.CreateScene("main scene");

    uint32_t playerId = mainScene.AddEntity("player");

    mainScene.AddComponent<LowEngine::ECS::TransformComponent>(playerId);

    auto& sprite = mainScene.AddComponent<LowEngine::ECS::SpriteComponent>(playerId);
    sprite.SetSprite(engine.Assets.GetTexture("unit1"));

    while (engine.IsWindowOpen()) {
        engine.Update();
        engine.Draw();
    }

    return 0;
}
