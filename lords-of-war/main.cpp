#include <iostream>

#include "../low-engine/Game.h"

int main() {
    // assets
    LowEngine::Assets::LoadTextureWithAnimationSheet("assets/textures/units/player-spritemap.png", "player", 46, 50, 8, 4);
    LowEngine::Assets::AddAnimationClip("player", "run", 24, 8, 0.20);

    // engine instance
    LowEngine::Game engine;
    bool success = engine.OpenWindow("Lords of war", 800, 600);
    if (!success) return 1;

    // create scene
    LowEngine::Scene& mainScene = engine.Scenes.CreateScene("main scene");
    engine.Scenes.SelectScene(mainScene);

    uint32_t playerId = mainScene.AddEntity("player");

    auto playerTransform = mainScene.AddComponent<LowEngine::ECS::TransformComponent>(playerId);
    if (playerTransform) {
        playerTransform->Position = { 400, 300 };
        playerTransform->Scale = { 3, 3 };
    }

    auto playerAnimation = mainScene.AddComponent<LowEngine::ECS::AnimatedSpriteComponent>(playerId);
    if (playerAnimation) {
        playerAnimation->SetSprite("player");
        playerAnimation->Play("run", true);
    }

    while (engine.IsWindowOpen()) {
        // main game loop
    }

    return 0;
}
