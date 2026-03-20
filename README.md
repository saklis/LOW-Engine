# LOW Engine

A C++20 2D game engine built for *Lords of War* — a turn-based strategy game inspired by Warlords 3.

## Features

- **Entity Component System (ECS)** — CRTP-based components with explicit dependency declarations and memory-pooled allocation
- **Scene management** — scene stack with pause/resume support; each scene owns its own physics world
- **2D physics** — Box2D integration with static, dynamic, and kinematic bodies; fixed-timestep `FixedUpdate` loop
- **Sprite rendering** — static sprites, frame-based animation, and tile map rendering via SFML
- **Tile maps** — LDtk format support with multiple layers, navigation grid, and A\* pathfinding
- **Action-based input** — named actions bound to keys or mouse buttons with optional modifier keys (Shift, Ctrl, Alt)
- **Audio** — sound effects, music playback with queue and crossfade support
- **JSON serialization** — full project/scene/entity/asset save & load (`.lowproj` / `.lowscene`)
- **ImGui editor** — in-engine editor with world outliner, per-component property panels, asset browser, and file dialogs

## Building

**Prerequisites:** CMake 3.26+, C++20 compiler, OpenGL

**Windows (Visual Studio):** Run one of the generator scripts to produce a solution, then build inside the IDE:
```
generate_vs2022-win32-sln.bat
generate_vs2026-win32-slnx.bat
```

**Manual:**
```bash
cmake -B build -S .
cmake --build build --config Debug
```

Outputs go to `bin/Debug/` or `bin/Release/`.

**CMake options:**

| Option | Default | Description |
|---|---|---|
| `BUILD_LOW_EDITOR` | `ON` | Build the LOWEditor executable |
| `BUILD_LOW_ENGINE_SHARED` | `ON` | Build LOWEngine as a shared library |

## Usage

```cpp
#include "Game.h"
#include "ecs/ECSHeaders.h"
#include "assets/Assets.h"

using namespace LowEngine;

int main() {
    Game game("My Game");
    game.OpenWindow(1280, 720, 60);

    // Load assets
    Assets::LoadTexture("player", "assets/textures/player.png");

    // Set up a scene
    Scene* scene = game.Scenes.GetCurrentScene();

    Entity* player = scene->AddEntity("Player");
    scene->AddComponent<TransformComponent>(player->Id);

    auto* sprite = scene->AddComponent<SpriteComponent>(player->Id);
    sprite->SetTexture("player");

    // Game loop
    while (game.IsWindowOpen()) {
        game.Draw();
    }
}
```

### Creating a custom component

```cpp
// Declare dependencies as template parameters
class HealthComponent : public IComponent<HealthComponent, TransformComponent> {
public:
    int HP = 100;

    explicit HealthComponent(Memory::Memory* memory) : IComponent(memory) {}

    void Initialize() override { HP = 100; }
    void Update(float deltaTime) override { /* ... */ }
};
```

### Project structure (at runtime)

```
my_project/
  my_project.lowproj
  assets/
    textures/
    sounds/
    music/
    fonts/
  scenes/
    main.lowscene
```

## Dependencies

All dependencies are in `3rd_party/` as git submodules.

| Library | Purpose |
|---|---|
| [SFML](https://github.com/SFML/SFML) | Graphics, window, input, audio |
| [Box2D](https://github.com/erincatto/box2d) | 2D physics |
| [TGUI](https://github.com/texus/TGUI) | SFML-based GUI widgets |
| [spdlog](https://github.com/gabime/spdlog) | Logging |
| [nlohmann/json](https://github.com/nlohmann/json) | JSON serialization |
| [Dear ImGui (docking)](https://github.com/ocornut/imgui) | Editor UI |
| [imgui-sfml](https://github.com/SFML/imgui-sfml) | ImGui ↔ SFML backend |
| [ImGuiFileDialog](https://github.com/aiekick/ImGuiFileDialog) | File browser widget |