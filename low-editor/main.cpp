#include "Game.h"
#include "devtools/DevTools.h"

sf::Vector2f StartPosition(4 * 16.f + 4.f, 7 * 16.f + 4.f);
sf::Vector2f EndPosition(8 * 16.f + 4.f, 4 * 16.f + 4.f);
std::vector<sf::Vector2f> CurrentPath;

void DebugDraw(sf::RenderWindow& window) {
    sf::RectangleShape startShape(sf::Vector2f(8.f, 8.f));
    startShape.setFillColor(sf::Color::Green);
    startShape.setPosition({StartPosition.x - 4.f, StartPosition.y - 4.f});

    sf::RectangleShape endShape(sf::Vector2f(8.f, 8.f));
    endShape.setFillColor(sf::Color::Red);
    endShape.setPosition({EndPosition.x - 4.f, EndPosition.y - 4.f});

    window.draw(startShape);
    window.draw(endShape);

    if (!CurrentPath.empty()) {
        for (auto& point: CurrentPath) {
            sf::RectangleShape pathPoint(sf::Vector2f(4.f, 4.f));
            pathPoint.setFillColor(sf::Color::Magenta);
            pathPoint.setPosition({point.x + 6.f, point.y + 6.f});
            window.draw(pathPoint);
        }
    }
}

void FindPathBetweenPositions(LowEngine::Game& game) {
    auto currentMapEntity = game.Scenes.GetCurrentScene()->FindEntity("map entity");
    if (currentMapEntity) {
        auto tileMap = currentMapEntity->GetComponent<LowEngine::ECS::TileMapComponent>();
        if (tileMap) {
            CurrentPath.clear();

            auto path = tileMap->FindPath(StartPosition, EndPosition, LowEngine::Terrain::Navigation::MovementType::Walk);
            if (!path.empty()) {
                for (const auto& point: path) {
                    CurrentPath.emplace_back(point.x, point.y);
                }
            }
        }
    }
}

int main() {
    // initialize the game engine
    LowEngine::Game game;

    // load assets
    LowEngine::Assets::LoadTextureWithSpriteSheet("assets/textures/terrain/green_terrain.png", "green_terrain", 16, 16, 3, 2);
    LowEngine::Assets::AddAnimationClip("green_terrain", "water", 3, 3, 0.5f);

    LowEngine::Assets::LoadTextureWithSpriteSheet("assets/textures/terrain/green_features.png", "green_features", 16, 16, 4, 2);
    LowEngine::Assets::AddAnimationClip("green_features", "forest1", 0, 2, 0.20f);
    LowEngine::Assets::AddAnimationClip("green_features", "forest2", 2, 2, 0.20f);

    LowEngine::Assets::LoadTileMap("assets/maps/terrain_map_01/BasicMap.ldtkl", "BasicMap", std::vector<LowEngine::Terrain::LayerDefinition>{
                                       {
                                           LowEngine::Terrain::LayerType::Terrain,
                                           LowEngine::Assets::GetTextureId("green_terrain"),
                                           std::unordered_map<unsigned, LowEngine::Terrain::CellDefinition>{
                                               {0, {true, false, true, 1.0f, {}}},
                                               {1, {false, true, true, 1.0f, {"water"}}},
                                           }
                                       },
                                       {
                                           LowEngine::Terrain::LayerType::Features,
                                           LowEngine::Assets::GetTextureId("green_features"),
                                           std::unordered_map<unsigned, LowEngine::Terrain::CellDefinition>{
                                               {0, {true, false, true, 1.0f, {"forest1", "forest2"}}},
                                               {1, {false, false, true, 1.0f, {}}}
                                           }
                                       }
                                   });

    LowEngine::Assets::LoadTextureWithSpriteSheet("assets/textures/units/rogue.png", "rogue", 32, 32, 10, 10);
    LowEngine::Assets::AddAnimationClip("rogue", "idle", 0, 10, 0.2f);
    LowEngine::Assets::AddAnimationClip("rogue", "walk", 20, 10, 0.15f);
    LowEngine::Assets::AddAnimationClip("rogue", "attack", 30, 10, 0.05f);
    LowEngine::Assets::AddAnimationClip("rogue", "die", 40, 10, 0.15f);

    // create scene
    auto mainScene = game.Scenes.CreateScene("new scene");
    mainScene->IsPaused = true;
    game.Scenes.SelectScene(*mainScene);

    // camera entity
    auto cameraEntity = mainScene->AddEntity("camera entity");
    if (cameraEntity) {
        auto tc = cameraEntity->AddComponent<LowEngine::ECS::TransformComponent>();
        if (tc) {
            tc->Position = {124.0f, 124.0f};
        }
        auto camera = cameraEntity->AddComponent<LowEngine::ECS::CameraComponent>();
        if (camera) {
            mainScene->SetCurrentCamera(cameraEntity->Id);
            camera->ZoomFactor = 0.4f;
        }
    }

    // map entity
    auto mapEntity = mainScene->AddEntity("map entity");
    if (mapEntity) {
        auto tc = mapEntity->AddComponent<LowEngine::ECS::TransformComponent>();
        auto map = mapEntity->AddComponent<LowEngine::ECS::TileMapComponent>();
        if (map) {
            map->SetMapId(LowEngine::Assets::GetTileMapId("BasicMap"));
        }
    }

    // init scene by doing a single update to propagate values between components
    mainScene->Update(0.0f);

    // open window
    bool success = game.OpenWindow("LOW Editor", 1664, 936);
    if (!success)return 1;

    game.Input.AddAction("FindPath", sf::Keyboard::Key::Space);
    game.Input.AddAction("SetStartPosition", sf::Mouse::Button::Left);
    game.Input.AddAction("SetEndPosition", sf::Mouse::Button::Right);

    LowEngine::DevTools::Initialize(game.Window); // ImGUI
    // LowEngine::DevTools::Build(game); //TGUI

    // main loop
    while (game.IsWindowOpen()) {
        // build dev tools
        for (auto windowEvent: game.WindowEvents) {
            LowEngine::DevTools::ReadInput(game.Window, windowEvent); // ImGUI
            // LowEngine::DevTools::ReadInput(windowEvent); // TGUI
        }

        LowEngine::DevTools::Update(game.Window, game.DeltaTime); // ImGUI
        LowEngine::DevTools::Build(game); // ImGUI
        // LowEngine::DevTools::Update(game); // TGUI

        // update
        if (game.Input.GetAction("SetStartPosition")->Started) {
            auto mouseWorldPosition = game.Window.mapPixelToCoords(game.Input.GetMousePosition());

            int cellX = static_cast<int>(std::floor(mouseWorldPosition.x / 16));
            int cellY = static_cast<int>(std::floor(mouseWorldPosition.y / 16));

            StartPosition.x = cellX * 16 + 8;
            StartPosition.y = cellY * 16 + 8;

            FindPathBetweenPositions(game);
        }
        if (game.Input.GetAction("SetEndPosition")->Started) {
            auto mouseWorldPosition = game.Window.mapPixelToCoords(game.Input.GetMousePosition());

            int cellX = static_cast<int>(std::floor(mouseWorldPosition.x / 16));
            int cellY = static_cast<int>(std::floor(mouseWorldPosition.y / 16));

            EndPosition.x = cellX * 16 + 8;
            EndPosition.y = cellY * 16 + 8;

            FindPathBetweenPositions(game);
        }
        if (game.Input.GetAction("FindPath")->Started) {
            FindPathBetweenPositions(game);
        }

        // draw
        game.Draw(DebugDraw, LowEngine::DevTools::Render); // ImGUI
        // game.Draw(LowEngine::DevTools::Draw); // TGUI
    }

    LowEngine::DevTools::Free();

    return 0;
}
