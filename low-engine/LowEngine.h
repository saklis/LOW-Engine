#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <SFML/Window.hpp>

#include <SFML/Window.hpp>

class LowEngine {
public:
    static void StartLog();
    static void EndLog();

    bool OpenWindow(const sf::String& title, uint32_t width, uint32_t height);
    bool WindowIsOpen();

protected:
    sf::Window _window;
    sf::Clock _clock;
};

