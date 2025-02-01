#include "../low-engine/LowEngine.h"

int main() {
    LowEngine::LowEngine engine;

    engine.OpenWindow("Lords of war", 800, 600);

    // initialize scene
    engine.CreateScene("MyScene");

    const uint32_t playerId = engine.GetScene().AddEntity("Player");
    engine.GetScene().AddComponent<LowEngine::ECS::TransformComponent>(playerId);

    const uint32_t enemyId = engine.GetScene().AddEntity("Enemy");
    engine.GetScene().AddComponent<LowEngine::ECS::TransformComponent>(enemyId);

    // game loop
    while (engine.IsWindowOpen()) {
        engine.Update();
        engine.Draw();
    }
    return 0;
}
