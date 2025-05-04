#include "Game.h"
#include "devtools/DevTools.h"

int main() {
    // initialize the game engine
    LowEngine::Game engine;

    // load assets
    LowEngine::Assets::LoadSound("assets/sounds/positive.wav", "positive_sound");
    LowEngine::Assets::LoadTexture("assets/textures/units/unit1.png", "unit1");
    LowEngine::Assets::LoadTextureWithAnimationSheet("assets/textures/units/player-spritemap.png", "player", 46, 50, 8, 4);
    LowEngine::Assets::AddAnimationClip("player", "run", 24, 8, 0.2f);

    // create scene
    LowEngine::Scene& mainScene = engine.Scenes.CreateScene("new scene");
    mainScene.IsPaused = true;
    engine.Scenes.SelectScene(mainScene);

    // camera entity
    auto cameraEntity = mainScene.AddEntity("camera entity");
    if (cameraEntity) {
        auto tc = cameraEntity->AddComponent<LowEngine::ECS::TransformComponent>();
        auto camera = cameraEntity->AddComponent<LowEngine::ECS::CameraComponent>();
        if (camera) {
            mainScene.SetCurrentCamera(cameraEntity->Id);
        }
    }

    //player entity
    auto playerEntity = mainScene.AddEntity("player entity");
    if (playerEntity) {
        auto tc = playerEntity->AddComponent<LowEngine::ECS::TransformComponent>();
        auto sprite = playerEntity->AddComponent<LowEngine::ECS::AnimatedSpriteComponent>();
        if (sprite) {
            sprite->SetSprite("player");
            sprite->Play("run");
        }
    }

    // sound-playing entity
    auto soundEntity = mainScene.AddEntity("sound entity");
    if (soundEntity) {
        auto tc = soundEntity->AddComponent<LowEngine::ECS::TransformComponent>();
        auto sound = soundEntity->AddComponent<LowEngine::ECS::SoundComponent>();
        if (sound) {
            sound->SetSound("positive_sound");
        }
    }

    // init scene by doing a single update
    mainScene.Update(0.0f);

    // add input action
    engine.Input.AddAction("move_left", sf::Keyboard::Key::A);
    engine.Input.AddAction("move_right", sf::Keyboard::Key::D);

    // open window
    bool success = engine.OpenWindow("LOW Editor", 1664, 936);
    if (!success) return 1;
    LowEngine::DevTools::Initialize(engine.Window);

    // main loop
    while (engine.IsWindowOpen()) {
        // build dev tools
        for (auto windowEvent : engine.WindowEvents) {
            LowEngine::DevTools::ReadInput(engine.Window, windowEvent);
        }
        LowEngine::DevTools::Update(engine.Window, engine.DeltaTime);
        LowEngine::DevTools::Build(engine);

        // update
        if (engine.Input.GetAction("move_left").Active) {
            engine.Scenes.GetCurrentScene().FindEntity("player entity")->GetComponent<LowEngine::ECS::TransformComponent>()->Position.x -= 1;
        }
        if (engine.Input.GetAction("move_right").Active) {
            engine.Scenes.GetCurrentScene().FindEntity("player entity")->GetComponent<LowEngine::ECS::TransformComponent>()->Position.x += 1;
        }

        // draw
        engine.Draw(LowEngine::DevTools::Render);
    }

    LowEngine::DevTools::Free();

    return 0;
}
