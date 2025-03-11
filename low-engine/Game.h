#pragma once

#include "Config.h"
#include "Log.h"
#include "assets/Assets.h"

#include "scene/SceneManager.h"
#include "input/InputManager.h"

namespace LowEngine {

    class Game {
    public:
        sf::RenderWindow Window;
        sf::Time DeltaTime;

        SceneManager Scenes;
        Input::InputManager Input;

        Game() : DeltaTime(sf::Time::Zero) {
            StartLog();
        }

        ~Game() {
            StopLog();
        }

        bool OpenWindow(const sf::String& title, uint32_t width, uint32_t height);

        bool IsWindowOpen();

        void Update(float deltaTime);

        void Draw();

    protected:
        sf::Clock _clock;

        void StartLog();

        void StopLog();
    };
}
