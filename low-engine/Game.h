#pragma once

#include "LowHeaders.h"
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

        void Update();

        void Draw();

    protected:
        sf::Clock _clock;

        void StartLog();

        void StopLog();
    };
}
