#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "scene/SceneManager.h"

namespace LowEngine {
    inline constexpr const char* LOGGER_NAME = "low_engine_spdlog_logger_name";

    class LowEngine {
    public:
        const sf::Time& DeltaTime; // public, read-only reference to _deltaTime

        LowEngine() : DeltaTime(_deltaTime), _deltaTime(sf::Time::Zero) {
            StartLog();
        }

        ~LowEngine() {
            StopLog();
        };

        bool OpenWindow(const sf::String& title, uint32_t width, uint32_t height);
        bool IsWindowOpen();

        void Update();
        void Draw();

        void CreateScene(const std::string& name);
        Scene::Scene& GetScene();

    protected:
        sf::RenderWindow _window;
        sf::Clock _clock;
        sf::Time _deltaTime;

        Scene::SceneManager _scenes;

        std::shared_ptr<spdlog::logger> _log;
        void StartLog();
        void StopLog();
    };
}

