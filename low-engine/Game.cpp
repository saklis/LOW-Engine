#include "Game.h"

#include <imgui-SFML.h>
#include <imgui.h>

namespace LowEngine {
    void Game::StartLog() {
        _log = spdlog::basic_logger_mt(Config::LOGGER_NAME, "engine.log", true);
        _log->set_level(spdlog::level::debug);
        _log->set_pattern("[%Y-%m-%d %H:%M:%S] [%l] %v");
        _log->flush_on(spdlog::level::trace);
        _log->info("LowEngine started");
    }

    void Game::StopLog() {
        _log->info("LowEngine stopped");
        spdlog::drop(Config::LOGGER_NAME);
    }

    void Game::OnWindowClosed() {
        Scenes.DestroyAll();
        Assets::UnloadAll();
        Window.close();
    }

    bool Game::OpenWindow(const sf::String& title, unsigned int width, unsigned int height) {
        Window.create(sf::VideoMode({width, height}), title);
        Window.setFramerateLimit(60);
        Window.setKeyRepeatEnabled(false); // for compatibility with Input system

        return Window.isOpen();
    }

    bool Game::IsWindowOpen() {
        DeltaTime = _clock.restart(); // time elapsed since last loop iteration

        WindowEvents.clear();
        Input.ClearActionState();

        bool isScenePaused = Scenes.GetCurrentScene().IsPaused;

        while (const std::optional event = Window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                OnWindowClosed();
                return false;
            }

            if (event->is<sf::Event::Resized>()) {
                auto windowSize = static_cast<sf::Vector2f>(Window.getSize());
                Scenes.GetCurrentScene().SetWindowSize(windowSize);
            }

            if (!isScenePaused) Input.Read(event);

            WindowEvents.emplace_back(event);
        }

        if (!isScenePaused) Input.Update();

        Update(DeltaTime.asSeconds());

        return Window.isOpen();
    }

    void Game::Update(float deltaTime) {
        if (!Scenes.GetCurrentScene().IsPaused) {
            Scenes.GetCurrentScene().Update(deltaTime);
        }
    }
}
