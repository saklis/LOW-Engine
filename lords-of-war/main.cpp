#include "../low-engine/LowEngine.h"

class PlayerCustomComponent : public LowEngine::ECS::Component {
public:
    int32_t Health = 100;

    PlayerCustomComponent() {

    }
    explicit PlayerCustomComponent(uint32_t ownerEntityId) {
        this->EntityId = ownerEntityId;
    }
    virtual ~PlayerCustomComponent() = default;

    void InitAsDefault() override {
        this->Active = true;
    }

    void Activate(uint32_t ownerEntityId) override {
        this->EntityId = ownerEntityId;
        this->Active = true;
    }

    void Update() override {
        // do nothing
    }
};

int main() {
    LowEngine::LowEngine engine;

    bool success = engine.OpenWindow("Lords of war", 800, 600);
    if (!success) return 1;

    engine.Assets.LoadAllFromPath("/assets");

    LowEngine::Scene& mainScene = engine.Scenes.CreateScene("main scene");

    uint32_t playerId = mainScene.AddEntity("player");
    //mainScene.AddComponent<LowEngine::ECS::TransformComponent>(playerId);
    mainScene.AddComponent<PlayerCustomComponent>(playerId);

    // auto& playerComponent = mainScene.GetComponent<PlayerCustomComponent>(playerId);
    // playerComponent.Health = 200;

    while (engine.IsWindowOpen()) {
        engine.Update();
        engine.Draw();
    }

    return 0;
}
