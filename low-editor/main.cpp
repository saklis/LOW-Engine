#include "Game.h"

#include "devtools/DevTools.h"

int main() {
    // initialize the game engine
    LowEngine::Game game;
	game.Title = "LOWEditor";

    auto mainScene = game.Scenes.CreateScene("new scene");
    game.DefaultSceneName = mainScene->Name;
    mainScene->IsPaused = true;
    game.Scenes.SelectScene(mainScene);

    // open window
    if (bool success = game.OpenWindow(1664, 936); !success) return 1;

    LowEngine::DevTools::Initialize(game.Window);

    // main loop
    while (game.IsWindowOpen()) {
        // build dev tools
        for (auto windowEvent: game.WindowEvents) {
            LowEngine::DevTools::ReadInput(game.Window, windowEvent);
        }

        LowEngine::DevTools::Update(game.Window, game.DeltaTime);
        LowEngine::DevTools::Build(game);

        // draw
        game.Draw(LowEngine::DevTools::Render);
    }

    LowEngine::DevTools::Free();

    return 0;
}
