#pragma once

#include <filesystem>

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
         * @brief The title of the game.
         *
         * This is used to set the title of the window when it is created.
		 */
        std::string Title;

        /**
         * @brief The path to the project directory.
         *
         * This is used to store and load project files, assets and other resources.
         * It should point to a valid directory where the project can be saved or loaded from.
         */
        std::filesystem::path ProjectDirectory;

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
            Scenes.CreateScene("default");
        }

        explicit Game(const std::string& gameTitle) : Title(gameTitle), DeltaTime(sf::Time::Zero) {
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
         * @param width The width of the window.
         * @param height The height of the window.
         * @param framerateLimit Limit the framerate to a maximum fixed frequency. 0 means no limit.
         * @return true if the game's window was initialized successfully, false otherwise.
         */
        bool OpenWindow(unsigned int width, unsigned int height, unsigned int framerateLimit = 0);

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

        /**
         * @brief Saves the current project to a file.
         *
         * This function serializes the current project properties and assets to a JSON file.
         * The path should point to a valid location where the project can be saved.
         *
         * @param filePath The path to save the project file.
         * @return true if the project was saved successfully, false otherwise.
		 */
		bool SaveProject(const std::string& filePath);

        /**
         * @brief Loads a project from a file.
         *
         * This function loads the project properties and assets from a JSON file.
         * The path should point to a valid project file.
         *
         * @param filePath The path to the project file.
         * @return true if the project was loaded successfully, false otherwise.
		 */
        bool LoadProject(const std::string& filePath);

        /**
         * @brief Closes the current project.
         *
         * This function closes the current project, clearing all scenes and assets.
         *
		 * @return true if the project was closed successfully, false otherwise.
		 */
        void CloseProject();

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
