#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "scene/SceneManager.h"
#include "assets/AssetManager.h"

namespace LowEngine {
    inline constexpr const char* LOGGER_NAME = "low_engine_spdlog_logger_name";

    class LowEngine {
    public:
        sf::Time DeltaTime;

        SceneManager Scenes;
        Assets::AssetManager Assets;

        LowEngine() : DeltaTime(sf::Time::Zero) {
            StartLog();
        }

        ~LowEngine() {
            StopLog();
        }

        bool OpenWindow(const sf::String& title, uint32_t width, uint32_t height);

        bool IsWindowOpen();

        void Update();

        void Draw();

    protected:
        sf::RenderWindow _window;
        sf::Clock _clock;

        std::shared_ptr<spdlog::logger> _log;

        void StartLog();

        void StopLog();
    };
}
