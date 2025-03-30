#include <iostream>

#include "../low-engine/Game.h"

int main() {
    // assets
    LowEngine::Assets::LoadTextureWithAnimationSheet("assets/textures/units/player-spritemap.png", "player", 46, 50, 8, 4);
    LowEngine::Assets::AddAnimationClip("player", "run", 24, 8, 0.20);

    LowEngine::Assets::LoadTextureWithAnimationSheet("assets/textures/units/rogue.png", "rogue", 32, 32, 10, 10);
    LowEngine::Assets::AddAnimationClip("rogue", "idle", 1, 10, 0.20);
    LowEngine::Assets::AddAnimationClip("rogue", "special", 11, 10, 0.10);
    LowEngine::Assets::AddAnimationClip("rogue", "walk", 21, 10, 0.10);
    LowEngine::Assets::AddAnimationClip("rogue", "attack", 31, 10, 0.05);
    LowEngine::Assets::AddAnimationClip("rogue", "die", 41, 10, 0.10);

    // engine instance
    LowEngine::Game engine;
    bool success = engine.OpenWindow("Lords of war", 800, 600);
    if (!success) return 1;

    // create scene

    LowEngine::Scene& mainScene = engine.Scenes.CreateScene("main scene");
    engine.Scenes.SelectScene(mainScene);

    auto cameraEntity = mainScene.AddEntity("Main Camera");
    auto cameraTransform = cameraEntity->AddComponent<LowEngine::ECS::TransformComponent>();
    if (cameraTransform) {
        cameraTransform->Position = { 200, 200 };
    }
    auto camera = cameraEntity->AddComponent<LowEngine::ECS::CameraComponent>();
    mainScene.SetCurrentCamera(cameraEntity->Id);

    auto player = mainScene.AddEntity("player");
    auto playerTransform = player->AddComponent<LowEngine::ECS::TransformComponent>();
    if (playerTransform) {
        playerTransform->Position = { 0, 0 };
        playerTransform->Scale = { 4, 4 };
    }
    auto playerAnimation = player->AddComponent<LowEngine::ECS::AnimatedSpriteComponent>();
    if (playerAnimation) {
        playerAnimation->SetSprite("rogue");
        playerAnimation->Play("attack", true);
    }

    auto sceond = mainScene.AddEntity("sceond");
    auto sceondTransform = sceond->AddComponent<LowEngine::ECS::TransformComponent>();
    if (sceondTransform) {
        sceondTransform->Position = { 200, 200 };
        sceondTransform->Scale = { -4, 4 };
    }
    auto sceondAnimation = sceond->AddComponent<LowEngine::ECS::AnimatedSpriteComponent>();
    if (sceondAnimation) {
        sceondAnimation->SetSprite("rogue");
        sceondAnimation->Play("walk", true);
    }

    ///////////////// STUFF

    auto playerTransform2 = player->GetComponent<LowEngine::ECS::TransformComponent>();
    if (playerTransform2) {
        playerTransform2->Position = { 200, 200 };
    }

    while (engine.IsWindowOpen()) {
        // main game loop
    }

    return 0;
}
