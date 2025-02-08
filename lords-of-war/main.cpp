#include "../low-engine/LowEngine.h"

int main() {
    LowEngine::LowEngine engine;

    bool success = engine.OpenWindow("Lords of war", 800, 600);
    if (!success) return 1;

    engine.Assets.LoadAllFromPath("/assets");

    uint32_t sceneId = engine.Scenes.CreateScene("main scene");

    LowEngine::ECS::Entity& player = engine.Scenes[sceneId].AddEntity("player");
    auto& playerTransform = player.AddComponent<LowEngine::ECS::TransformComponent>();

    engine.Scenes.SetAsCurrent(sceneId);

    while (engine.IsWindowOpen()) {
        engine.Update();
        engine.Draw();
    }

    return 0;

    // le::OpenWindow("Lords of war", 800, 600);
    // le::Assets::LoadAllFromPath("/assets");
    //
    // le::Scene::CreateNew("main scene");
    // int entityId = le::CurrentScene::AddEntity("player");
    // le::CurrentScene::AddComponent<le::TransformComponent>(entityId);
    //
    // // game loop
    // while (le::IsWindowOpen()) {
    //     le::Update();
    //     le::Draw();
    // }
    // return 0;
}
