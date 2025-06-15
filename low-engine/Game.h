#pragma once

#include "Config.h"
#include "Log.h"
#include "assets/Assets.h"

#include "ecs/ECSHeaders.h"
#include "scene/SceneManager.h"
#include "input/InputManager.h"

namespace LowEngine {
    /**
     * @brief The main class of the game engine.
     *
     * This class is responsible for creating the window, managing the game loop, scenes and handling input.
     */
    class Game {
    public:
        /**
         * @brief The main window of the game.
         *
         * This is the window where the game will be rendered.
         */
        sf::RenderWindow Window;

        /**
         * @brief List of all Window's sf::Events for current frame.
         *
         * Useful to pull events outside from game's instance.
         */
        std::vector<std::optional<sf::Event> > WindowEvents;

        /**
         * @brief The time elapsed since the last frame.
         */
        sf::Time DeltaTime;

        /**
         * @brief All scenes managed by the game.
         *
         * Scenes are imagined as a stack of scenes, where the top scene is the one currently being rendered.
         * This is useful for managing multiple scenes, such as a main menu, pause menu, etc.
         * Note, that this is not a stack in technical scene, so active/current index can be moved.
         */
        SceneManager Scenes;

        /**
         * @brief Game's Action-based input system.
         *
         * This is a wrapper around SFML's input system, which allows for more flexible and reusable input handling.
         * Interaction is done through defining new Actions and binding them to specific keys or buttons.
         * Then Action can be retrieved and checked for state (started, ended, active).
         */
        Input::InputManager Input;

        Game() : DeltaTime(sf::Time::Zero) {
            StartLog();
        }

        ~Game() {
            StopLog();
        }

        /**
         * @brief Initializes the game.
         *
         * This function creates and opens the window.
         *
         * @param title The title of the window.
         * @param width The width of the window.
         * @param height The height of the window.
         * @param framerateLimit Limit the framerate to a maximum fixed frequency. 0 means no limit.
         * @return true if the game's window was initialized successfully, false otherwise.
         */
        bool OpenWindow(const sf::String& title, unsigned int width, unsigned int height, unsigned int framerateLimit = 0);

        /**
         * @brief Main game loop.
         *
         * This is the main loop of the game, which runs until the window is closed.
         * It polls window events (including input) and updates the game state though Components in current scene.
         *
         * @return true if the window is still open and the game loop is running, false otherwise.
         */
        bool IsWindowOpen();

        /**
         * @brief Draws the game.
         *
         * This function clears the window, draws the current scene on the Window.
         * It can also take additional callback functions to be executed after drawing the scene.
         *
         * @tparam Extras The types of the additional callback functions.
         * @param callbackChain The additional callback functions to be executed after scene is prepared but before displaying.
         */
        template<typename... Extras>
            requires (std::is_invocable_r_v<void, Extras, sf::RenderWindow&> && ...)
        void Draw(Extras&&... callbackChain) {
            Window.clear();
            Scenes.GetCurrentScene()->Draw(Window);
            (std::forward<Extras>(callbackChain)(Window), ...);
            Window.display();
        }

    protected:
        sf::Clock _clock;

        /**
         * @brief Updates the game state.
         *
         * This function updates the game state by calling the Update function of the current scene.
         * It's called every frame by IsWindowOpen().
         *
         * @param deltaTime The time elapsed since the last frame.
         */
        void Update(float deltaTime);

        void StartLog();

        void StopLog();

        void OnWindowClosed();
    };
}
