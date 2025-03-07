#include <iostream>

#include "../low-engine/Game.h"

int main() {
    // assets
    LowEngine::Assets::LoadTexture("assets/textures/units/unit1.png", "unit1");

    // engine instance
    LowEngine::Game engine;
    bool success = engine.OpenWindow("Lords of war", 800, 600);
    if (!success) return 1;

    // create scene
    LowEngine::Scene& mainScene = engine.Scenes.CreateScene("main scene");
    engine.Scenes.SelectScene(mainScene);

    uint32_t playerId = mainScene.AddEntity("player");

    mainScene.AddComponent<LowEngine::ECS::TransformComponent>(playerId);
    mainScene.AddComponent<LowEngine::ECS::SpriteComponent>(playerId).SetSprite("unit1");

    auto& transforms = mainScene.GetComponent<LowEngine::ECS::TransformComponent>(playerId);
    transforms.Position = { 350, 200 };
    transforms.Scale = { 7, 7 };

    // declare input actions
    engine.Input.AddAction("MoveUp", sf::Keyboard::Key::W);
    engine.Input.AddAction("MoveDown", sf::Keyboard::Key::W, sf::Keyboard::Key::LShift);
    engine.Input.AddAction("MoveLeft", sf::Keyboard::Key::A);
    engine.Input.AddAction("MoveRight", sf::Keyboard::Key::D);

    engine.Input.AddAction("PrimaryMouseClick", sf::Mouse::Button::Left);
    engine.Input.AddAction("SecondaryMouseClick", sf::Mouse::Button::Left, sf::Keyboard::Key::RControl);

    while (engine.IsWindowOpen()) {
        if (engine.Input.GetAction("MoveUp").Active) {
            transforms.Position.y -= 3;
        }
        if (engine.Input.GetAction("MoveDown").Active) {
            transforms.Position.y += 3;
        }
        if (engine.Input.GetAction("MoveLeft").Started) {
            transforms.Position.x -= 3;
        }
        if (engine.Input.GetAction("MoveRight").Ended) {
            transforms.Position.x += 3;
        }

        if (engine.Input.GetAction("SecondaryMouseClick").Started) {
            sf::Vector2i mousePos = engine.Input.GetMousePosition();
        }
    }

    return 0;
}
