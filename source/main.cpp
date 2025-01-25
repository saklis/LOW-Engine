#include <SFML/Window.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

#include "memory/Memory.h"
#include "ecs/Entity.h"

void StartLog() {
    auto logger = spdlog::basic_logger_mt("basic_logger", "lords-of-war.log", true);
    spdlog::set_default_logger(logger);
    spdlog::set_level(spdlog::level::debug);
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S] [%l] %v");
}

void StopLog() {
    spdlog::shutdown();
}

int main() {
    StartLog();
    spdlog::info("Lords of War starting...");

    // INITIALIZE MEMORY
    Memory memory;

    sf::Window window(sf::VideoMode({800, 600}), "Lords of War");
    window.setFramerateLimit(60);
    window.setKeyRepeatEnabled(false);

    sf::Clock clock;
    while (window.isOpen()) {
        const sf::Time deltaTime = clock.restart(); // time elapsed since last loop iteration

        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
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
    }

    spdlog::info("Lords of War exits");
    StopLog();
    return 0;
}
