#include "Game.h"

#include "LogMemoryBufferSink.h"

namespace LowEngine {
    void Game::StartLog() {
        auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("lowengine.log", true);
        auto memorySink = std::make_shared<LogMemoryBufferSink>(_logContent);
        std::vector<spdlog::sink_ptr> sinks{fileSink, memorySink};

        _log = std::make_shared<spdlog::logger>(Config::LOGGER_NAME, sinks.begin(), sinks.end());

        _log->set_level(spdlog::level::debug);
        _log->set_pattern("[%Y-%m-%d %H:%M:%S] [%l] %v");
        _log->flush_on(spdlog::level::trace);

    	_log->info("##### {} #####", Title);
        _log->info("LOWEngine started");
    }

    void Game::StopLog() {
        _log->info("LOWEngine stopped");
        spdlog::drop(Config::LOGGER_NAME);
    }

    void Game::OnWindowClosed() {
        Scenes.DestroyAll();
        Assets::UnloadAll();
        Window.close();
    }

    bool Game::OpenWindow(unsigned int width, unsigned int height, unsigned int framerateLimit) {
        Window.create(sf::VideoMode({width, height}), Title);
        Window.setFramerateLimit(framerateLimit);
        Window.setKeyRepeatEnabled(false); // leave Input system to trace state of Actions

        return Window.isOpen();
    }

    bool Game::IsWindowOpen() {
        DeltaTime = _clock.restart(); // time elapsed since last loop iteration

        WindowEvents.clear();
        Input.ClearActionState();

        bool isScenePaused = Scenes.GetCurrentScene()->IsPaused;

        while (const std::optional event = Window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                OnWindowClosed();
                return false;
            }

            if (event->is<sf::Event::Resized>()) {
                auto windowSize = static_cast<sf::Vector2f>(Window.getSize());
                Scenes.GetCurrentScene()->SetWindowSize(windowSize);
            }

            if (!isScenePaused) Input.Read(event);

            WindowEvents.emplace_back(event);
        }

        if (!isScenePaused) Input.Update();

        Update(DeltaTime.asSeconds());

        return Window.isOpen();
    }

    std::string Game::GetProjectJsonString()
    {
		// create JSON structure for assets
        nlohmann::ordered_json assetsJson;
		nlohmann::ordered_json texturesJson;

        auto textureAliases = Assets::GetTextureAliases();
        for (const auto& alias : textureAliases) {
			if (alias == Config::DEFAULT_TEXTURE_ALIAS) continue; // skip default texture

            auto texture = Assets::GetTexture(alias);

			nlohmann::ordered_json textureJson;
			textureJson["alias"] = alias;
            textureJson["path"] = texture.Path;

            texturesJson.emplace_back(std::move(textureJson));
        }

		assetsJson["textures"] = texturesJson;

		// final project JSON
        nlohmann::ordered_json projectJson;
        projectJson["title"] = Title;
		projectJson["assets"] = assetsJson;

		return projectJson.dump(4); // pretty print with 4 spaces
    }

    void Game::Update(float deltaTime) {
        if (!Scenes.GetCurrentScene()->IsPaused) {
            Scenes.GetCurrentScene()->Update(deltaTime);
        }
    }
}
