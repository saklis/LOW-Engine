#include "LowEngine.h"

void LowEngine::LowEngine::StartLog() {
    _log = spdlog::basic_logger_mt(LOGGER_NAME, "engine.log", true);
    _log->set_level(spdlog::level::debug);
    _log->set_pattern("[%Y-%m-%d %H:%M:%S] [%l] %v");
}

void LowEngine::LowEngine::StopLog() {
    spdlog::drop(LOGGER_NAME);
}

bool LowEngine::LowEngine::OpenWindow(const sf::String& title, uint32_t width, uint32_t height) {
    _window.create(sf::VideoMode({width, height}), title);
    _window.setFramerateLimit(60);
    _window.setKeyRepeatEnabled(false);

    return _window.isOpen();
}

bool LowEngine::LowEngine::IsWindowOpen() {
    DeltaTime = _clock.restart(); // time elapsed since last loop iteration

    while (const std::optional event = _window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            _window.close();
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
    }
    return _window.isOpen();
}

void LowEngine::LowEngine::Update() {
    Scenes.GetCurrent().Update();
}

void LowEngine::LowEngine::Draw() {
    // clear frame
    _window.clear();

    // draw commands here

    // draw current frame
    _window.display();
}

LowEngine::Scene& LowEngine::LowEngine::GetCurrentScene() {
    return Scenes.GetCurrent();
}
