#include "Game.h"

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

    bool Game::OpenWindow(const sf::String& title, uint32_t width, uint32_t height) {
        Window.create(sf::VideoMode({width, height}), title);
        Window.setFramerateLimit(60);
        Window.setKeyRepeatEnabled(false);

        return Window.isOpen();
    }

    bool Game::IsWindowOpen() {
        DeltaTime = _clock.restart(); // time elapsed since last loop iteration

        Input.ClearActionState();
        while (const std::optional event = Window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                Scenes.DestroyAll();
                Assets::UnloadAll();
                Window.close();
                return false;
            }

            Input.Read(event);
        }
        Input.Update();

        Update(DeltaTime.asSeconds());
        Draw();

        return Window.isOpen();
    }

    void Game::Update(float deltaTime) {
        Scenes.GetCurrentScene().Update(deltaTime);
    }

    void Game::Draw() {
        // clear frame
        Window.clear();

        // draw commands here
        Scenes.GetCurrentScene().Draw(Window);

        // draw current frame
        Window.display();
    }
}
