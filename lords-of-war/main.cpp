#include "../low-engine/LowEngine.h"

int main() {
    LowEngine::LowEngine engine;

    engine.Assets.LoadAllFromPath("assets");
    engine.OpenWindow("Lords of war", 800, 600);

    // engine.Scenes.CreateScene("main scene");
    //
    // auto mainScene = new LowEngine::Scene::SceneManager();
    // engine.Scenes = *mainScene;

    // game loop
    while (engine.IsWindowOpen()) {
        //engine.GetScenes().GetCurrent().Update();
        engine.Draw();
    }
    return 0;
}
