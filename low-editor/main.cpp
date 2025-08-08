#include "Game.h"
#include "devtools/DevTools.h"

//sf::Vector2f StartPosition(4 * 16.f + 4.f, 7 * 16.f + 4.f);
//sf::Vector2f EndPosition(8 * 16.f + 4.f, 4 * 16.f + 4.f);
//std::vector<sf::Vector2f> CurrentPath;

//void DebugDraw(sf::RenderWindow& window) {
//    sf::RectangleShape startShape(sf::Vector2f(8.f, 8.f));
//    startShape.setFillColor(sf::Color::Green);
//    startShape.setPosition({StartPosition.x - 4.f, StartPosition.y - 4.f});
//
//    sf::RectangleShape endShape(sf::Vector2f(8.f, 8.f));
//    endShape.setFillColor(sf::Color::Red);
//    endShape.setPosition({EndPosition.x - 4.f, EndPosition.y - 4.f});
//
//    window.draw(startShape);
//    window.draw(endShape);
//
//    if (!CurrentPath.empty()) {
//        for (auto& point: CurrentPath) {
//            sf::RectangleShape pathPoint(sf::Vector2f(4.f, 4.f));
//            pathPoint.setFillColor(sf::Color::Magenta);
//            pathPoint.setPosition({point.x + 6.f, point.y + 6.f});
//            window.draw(pathPoint);
//        }
//    }
//}
//
//void FindPathBetweenPositions(LowEngine::Game& game) {
//    auto currentMapEntity = game.Scenes.GetCurrentScene()->FindEntity("map entity");
//    if (currentMapEntity) {
//        auto tileMap = currentMapEntity->GetComponent<LowEngine::ECS::TileMapComponent>();
//        if (tileMap) {
//            CurrentPath.clear();
//
//            auto path = tileMap->FindPath(StartPosition, EndPosition, LowEngine::Terrain::Navigation::MovementType::Walk);
//            if (!path.empty()) {
//                for (const auto& point: path) {
//                    CurrentPath.emplace_back(point.x, point.y);
//                }
//            }
//        }
//    }
//}

int main() {
    // initialize the game engine
    LowEngine::Game game;
	game.Title = "LOWEditor";

    auto mainScene = game.Scenes.CreateScene("new scene");
    mainScene->IsPaused = true;
    game.Scenes.SelectScene(mainScene);

    //game.LoadProject("projects\\LOWEditor\\LOWEditor.lowproj");

    // create scene
    //auto mainScene = game.Scenes.CreateScene("new scene");
    //mainScene->IsPaused = true;
    //game.Scenes.SelectScene(mainScene);

    //// camera entity
    //if (const auto cameraEntity = mainScene->GetCurrentCamera()) {
    //    if (const auto tc = cameraEntity->GetComponent<LowEngine::ECS::TransformComponent>()) {
    //        tc->Position = {124.0f, 175.0f};
    //    }
    //    if (const auto camera = cameraEntity->GetComponent<LowEngine::ECS::CameraComponent>()) {
    //        camera->ZoomFactor = 0.5f;
    //    }
    //}

    //// map entity
    //if (auto mapEntity = mainScene->AddEntity("map entity")) {
    //    auto tc = mapEntity->AddComponent<LowEngine::ECS::TransformComponent>();
    //    if (auto map = mapEntity->AddComponent<LowEngine::ECS::TileMapComponent>()) {
    //        map->SetMapId(LowEngine::Assets::GetTileMapId("BasicMap"));
    //    }
    //}

    //// init scene by doing a single update to propagate values between components
    //mainScene->Update(0.0f);

    // open window
    bool success = game.OpenWindow(1664, 936);
    if (!success)return 1;

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
        /*auto setStartPosition = game.Input.GetAction("SetStartPosition");
        if (setStartPosition && setStartPosition->Started) {
            auto mouseWorldPosition = game.Window.mapPixelToCoords(game.Input.GetMousePosition());

            int cellX = static_cast<int>(std::floor(mouseWorldPosition.x / 16));
            int cellY = static_cast<int>(std::floor(mouseWorldPosition.y / 16));

            StartPosition.x = cellX * 16 + 8;
            StartPosition.y = cellY * 16 + 8;

            FindPathBetweenPositions(game);
        }

		auto setEndPosition = game.Input.GetAction("SetEndPosition");
        if (setEndPosition && setEndPosition->Started) {
            auto mouseWorldPosition = game.Window.mapPixelToCoords(game.Input.GetMousePosition());

            int cellX = static_cast<int>(std::floor(mouseWorldPosition.x / 16));
            int cellY = static_cast<int>(std::floor(mouseWorldPosition.y / 16));

            EndPosition.x = cellX * 16 + 8;
            EndPosition.y = cellY * 16 + 8;

            FindPathBetweenPositions(game);
        }

		auto findPath = game.Input.GetAction("FindPath");
        if (findPath && findPath->Started) {
            FindPathBetweenPositions(game);
        }*/

        // draw
        game.Draw(/*DebugDraw,*/ LowEngine::DevTools::Render);
    }

    LowEngine::DevTools::Free();

    return 0;
}
