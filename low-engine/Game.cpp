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

        sf::Vector2f viewSize = {static_cast<float>(width), static_cast<float>(height)};
        MainView.setSize(viewSize);
        MainView.setCenter({viewSize.x / 2, viewSize.y / 2});
        Window.setView(MainView);

        ImGui::SFML::Init(Window);

        return Window.isOpen();
    }

    bool Game::IsWindowOpen() {
        DeltaTime = _clock.restart(); // time elapsed since last loop iteration

        Input.ClearActionState();
        while (const std::optional event = Window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                Scenes.DestroyAll();
                Assets::UnloadAll();
                ImGui::SFML::Shutdown();
                Window.close();
                return false;
            }

            if (event->is<sf::Event::Resized>()) {
                auto windowSize = static_cast<sf::Vector2f>(Window.getSize());
                auto viewSize = MainView.getSize();

                if (windowSize.x / windowSize.y > viewSize.x / viewSize.y) {
                    viewSize.x = windowSize.x;
                    viewSize.y = windowSize.x / (viewSize.x / viewSize.y);
                    MainView.setSize(viewSize);
                } else {
                    viewSize.y = windowSize.y;
                    viewSize.x = windowSize.y * (viewSize.x / viewSize.y);
                    MainView.setSize(viewSize);
                }

                Window.setView(MainView);
            }

            if (AllowDevTools) {
                if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                    if (keyPressed->code == sf::Keyboard::Key::F12 && keyPressed->control) {
                        ShowDevTools = !ShowDevTools;
                    }
                }
            }

            ImGui::SFML::ProcessEvent(Window, *event);
            Input.Read(event);
        }
        Input.Update();

        ImGui::SFML::Update(Window, DeltaTime);
        if (ShowDevTools) {
            DevTools::Display(*this);
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
        ImGui::SFML::Render(Window);

        Window.display();
    }
}
