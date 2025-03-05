#include "Game.h"

void LowEngine::Game::StartLog() {
    _log = spdlog::basic_logger_mt(Config::LOGGER_NAME, "engine.log", true);
    _log->set_level(spdlog::level::debug);
    _log->set_pattern("[%Y-%m-%d %H:%M:%S] [%l] %v");
    _log->info("LowEngine started");
}

void LowEngine::Game::StopLog() {
    _log->info("LowEngine stopped");
    spdlog::drop(Config::LOGGER_NAME);
}

bool LowEngine::Game::OpenWindow(const sf::String& title, uint32_t width, uint32_t height) {
    Window.create(sf::VideoMode({width, height}), title);
    Window.setFramerateLimit(60);
    Window.setKeyRepeatEnabled(false);

    return Window.isOpen();
}

bool LowEngine::Game::IsWindowOpen() {
    DeltaTime = _clock.restart(); // time elapsed since last loop iteration

    while (const std::optional event = Window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            Scenes.DestroyAll();
            Assets::UnloadAll();
            Window.close();
            return false;
        }

        // INPUT HANDLING
        if (const auto* mouseMoved = event->getIf<sf::Event::MouseMoved>()) {
            // mouse moved
        }
        if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>()) {
            // mouse button pressed
        }
        if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            // key pressed
        }
        if (const auto* keyReleased = event->getIf<sf::Event::KeyReleased>()) {
            // key released
        }
    }

    Update();
    Draw();

    return Window.isOpen();
}

void LowEngine::Game::Update() {
    Scenes.GetCurrentScene().Update();
}

void LowEngine::Game::Draw() {
    // clear frame
    Window.clear();

    // draw commands here
    Scenes.GetCurrentScene().Draw(Window);

    // draw current frame
    Window.display();
}
