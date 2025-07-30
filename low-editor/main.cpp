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
    LowEngine::Game game("LOWEditor");

    // load assets
    LowEngine::Assets::LoadTextureWithSpriteSheet("green_terrain", "assets/textures/terrain/green_terrain.png", 16, 16, 3, 2);
    LowEngine::Assets::AddAnimationClip("water", "green_terrain", 3, 3, 0.5f);

    LowEngine::Assets::LoadTextureWithSpriteSheet("green_features", "assets/textures/terrain/green_features.png", 16, 16, 4, 2);
    LowEngine::Assets::AddAnimationClip("forest1", "green_features", 0, 2, 0.20f);
    LowEngine::Assets::AddAnimationClip("forest2", "green_features", 2, 2, 0.20f);

    LowEngine::Assets::LoadTileMap("BasicMap", "assets/maps/terrain_map_01/BasicMap.ldtkl", std::vector<LowEngine::Terrain::LayerDefinition>{
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

    LowEngine::Assets::LoadTextureWithSpriteSheet("rogue", "assets/textures/units/rogue.png", 32, 32, 10, 10);
    LowEngine::Assets::AddAnimationClip("idle", "rogue", 0, 10, 0.2f);
    LowEngine::Assets::AddAnimationClip("walk", "rogue", 20, 10, 0.15f);
    LowEngine::Assets::AddAnimationClip("attack", "rogue", 30, 10, 0.05f);
    LowEngine::Assets::AddAnimationClip("die", "rogue", 40, 10, 0.15f);

    LowEngine::Assets::LoadSound("positive", "assets/sounds/positive.wav");

    // create scene
    auto mainScene = game.Scenes.CreateScene("new scene");
    mainScene->IsPaused = true;
    game.Scenes.SelectScene(*mainScene);

    // camera entity
    if (const auto cameraEntity = mainScene->GetCurrentCamera()) {
        if (const auto tc = cameraEntity->GetComponent<LowEngine::ECS::TransformComponent>()) {
            tc->Position = {124.0f, 175.0f};
        }
        if (const auto camera = cameraEntity->GetComponent<LowEngine::ECS::CameraComponent>()) {
            camera->ZoomFactor = 0.5f;
        }
    }

    // map entity
    if (auto mapEntity = mainScene->AddEntity("map entity")) {
        auto tc = mapEntity->AddComponent<LowEngine::ECS::TransformComponent>();
        if (auto map = mapEntity->AddComponent<LowEngine::ECS::TileMapComponent>()) {
            map->SetMapId(LowEngine::Assets::GetTileMapId("BasicMap"));
        }
    }

    // init scene by doing a single update to propagate values between components
    mainScene->Update(0.0f);

    // open window
    bool success = game.OpenWindow(1664, 936);
    if (!success)return 1;

    game.Input.AddAction("FindPath", sf::Keyboard::Key::Space);
    game.Input.AddAction("SetStartPosition", sf::Mouse::Button::Left);
    game.Input.AddAction("SetEndPosition", sf::Mouse::Button::Right);

    LowEngine::DevTools::Initialize(game.Window);

    // main loop
    while (game.IsWindowOpen()) {
        // build dev tools
        for (auto windowEvent: game.WindowEvents) {
            LowEngine::DevTools::ReadInput(game.Window, windowEvent);
        }

        LowEngine::DevTools::Update(game.Window, game.DeltaTime);
        LowEngine::DevTools::Build(game);

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
        game.Draw(DebugDraw, LowEngine::DevTools::Render);
    }

    LowEngine::DevTools::Free();

    return 0;
}
