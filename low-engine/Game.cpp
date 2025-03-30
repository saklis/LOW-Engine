#include "Game.h"

#include <imgui-SFML.h>
#include <imgui.h>

#include "devtools/DevTools.h"

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

    bool Game::OpenWindow(const sf::String& title, unsigned int width, unsigned int height) {
        Window.create(sf::VideoMode({width, height}), title);
        Window.setFramerateLimit(60);
        Window.setKeyRepeatEnabled(false);

        if (AllowDevTools) {
            if (!DevTools::Initialize(Window)) return false;
        }

        return Window.isOpen();
    }

    bool Game::IsWindowOpen() {
        DeltaTime = _clock.restart(); // time elapsed since last loop iteration

        if (ShowDevTools) DevTools::BeginReadInput();
        Input.ClearActionState();
        while (const std::optional event = Window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                Scenes.DestroyAll();
                Assets::UnloadAll();
                if (AllowDevTools) {
                    DevTools::Free();
                }
                Window.close();
                return false;
            }

            if (event->is<sf::Event::Resized>()) {
                auto windowSize = static_cast<sf::Vector2f>(Window.getSize());
                Scenes.GetCurrentScene().SetWindowSize(windowSize);
                // Scenes.GetCurrentScene().GetCurrentCamera()->SetWindowSize(windowSize);
            }

            if (AllowDevTools) {
                if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                    if (keyPressed->code == sf::Keyboard::Key::F12) {
                        ShowDevTools = !ShowDevTools;
                    }
                }

                DevTools::ReadInput(Window, event);
            }

            Input.Read(event);
        }
        if (ShowDevTools) DevTools::EndReadInput();

        Input.Update();

        if (ShowDevTools) {
            DevTools::Update(Window, DeltaTime);
            DevTools::Build(*this);
        }

        Update(DeltaTime.asSeconds());
        Draw();

        return Window.isOpen();
    }

    void Game::Update(float deltaTime) {
        Scenes.GetCurrentScene().Update(deltaTime);
    }

    void Game::Draw() {
        Window.clear();

        Scenes.GetCurrentScene().Draw(Window);

        if (ShowDevTools) {
            DevTools::Render(Window);
        }

        Window.display();
    }
}
