#include <iostream>

#include "../low-engine/Game.h"

int main() {
    // assets
    LowEngine::Assets::LoadTextureWithAnimationSheet("assets/textures/terrain/green_terrain.png", "green_terrain", 16, 16, 3, 2);
    LowEngine::Assets::AddAnimationClip("green_terrain", "water", 3, 3, 0.50);

    LowEngine::Assets::LoadTextureWithAnimationSheet("assets/textures/terrain/green_features.png", "green_features", 16, 16, 4, 2);
    LowEngine::Assets::AddAnimationClip("green_features", "forest1", 0, 2, 0.1);
    LowEngine::Assets::AddAnimationClip("green_features", "forest2", 2, 2, 0.1);

    auto mapId = LowEngine::Assets::LoadMap("assets/maps/terrain_map_01/BasicMap.ldtkl", "BasicMap", {
                                                {
                                                    LowEngine::Terrain::LayerType::Terrain,
                                                    LowEngine::Assets::GetTextureId("green_terrain"),
                                                    {
                                                        /*0:*/ {}, // no animation clip - just a static tile
                                                        /*1:*/ {"water"}
                                                    }
                                                },
                                                {
                                                    LowEngine::Terrain::LayerType::Features,
                                                    LowEngine::Assets::GetTextureId("green_features"),
                                                    {
                                                        /*0:*/ {"forest1", "forest2"},
                                                        /*1:*/{}
                                                    }
                                                }
                                            });

    // engine instance
    LowEngine::Game engine;
    bool success = engine.OpenWindow("Lords of war", 800, 600);
    if (!success) return 1;

    // create scene
    LowEngine::Scene& mainScene = engine.Scenes.CreateScene("main scene");
    mainScene.SetSpriteSorting(LowEngine::Scene::SpriteSortingMethod::Layers);
    engine.Scenes.SelectScene(mainScene);

    // create map
    auto mapEntity = mainScene.AddEntity("Map");
    if (mapEntity) {
        auto mapTransform = mapEntity->AddComponent<LowEngine::ECS::TransformComponent>();
        if (mapTransform) {
            mapTransform->Position = {-350, -250};
        }

        auto mapComponent = mapEntity->AddComponent<LowEngine::ECS::MapComponent>();
        if (mapComponent) {
            mapComponent->SetMapId(mapId);
            mapComponent->Layer = -5;
        }
    }

    // create camera
    auto cameraEntity = mainScene.AddEntity("Main Camera");
    if (cameraEntity) {
        cameraEntity->AddComponent<LowEngine::ECS::TransformComponent>();
        cameraEntity->AddComponent<LowEngine::ECS::CameraComponent>();
        mainScene.SetCurrentCamera(cameraEntity->Id);
    }

    while (engine.IsWindowOpen()) {
        // game logic
    }

    return 0;
}
