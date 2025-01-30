
#include "LowEngine.h"

void LowEngine::StartLog() {
    auto logger = spdlog::basic_logger_mt("basic_logger", "low-engine.log", true);
    spdlog::set_default_logger(logger);
    spdlog::set_level(spdlog::level::debug);
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S] [%l] %v");
}

void LowEngine::EndLog() {
    spdlog::shutdown();
}

bool LowEngine::OpenWindow(const sf::String& title, uint32_t width, uint32_t height) {
    _window.create(sf::VideoMode({width, height}), title);
    _window.setFramerateLimit(60);
    _window.setKeyRepeatEnabled(false);

    return _window.isOpen();
}

bool LowEngine::WindowIsOpen() {
    const sf::Time deltaTime = _clock.restart(); // time elapsed since last loop iteration

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
