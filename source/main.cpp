#include <SFML/Window.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

int main() {
    // spdlog setup
    auto logger = spdlog::basic_logger_mt("basic_logger", "lifesim.log", true);
    spdlog::set_default_logger(logger);
    spdlog::set_level(spdlog::level::debug);
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S] [%l] %v");

    spdlog::info("Starting LifeSim!");

    sf::Window window(sf::VideoMode({800,600}), "Hello SFML");

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }
    }

    return 0;
}