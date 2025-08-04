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

		_log->info("##### {} started #####", Title);
		_log->info("LOWEngine initialized");
	}

	void Game::StopLog() {
		_log->info("LOWEngine instance destroyed");
		_log->info("##### {} stopped #####", Title);
		spdlog::drop(Config::LOGGER_NAME);
	}

	void Game::OnWindowClosed() {
		CloseProject();
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

	bool Game::SaveProject(const std::string& filePath) {
		std::ofstream file(filePath);
		if (!file.is_open()) {
			_log->error("Failed to open file for saving project: {}", filePath);
			return false;
		}

		_log->debug("Saving project to file: {}", filePath);

		nlohmann::ordered_json projectJson;
		projectJson["title"] = Title;
		projectJson["assets"] = Assets::SerializeToJSON();
		projectJson["inputActions"] = Input.SerializeActionsToJSON();

		file << projectJson.dump(4); // pretty print with 4 spaces

		file.close();
		if (file.fail()) {
			_log->error("Failed to write project data to file: {}", filePath);
			return false;
		}
		_log->info("Project saved successfully to: {}", filePath);
		return true;
	}

	bool Game::LoadProject(const std::string& filePath) {
		_log->info("Loading project from file: {}", filePath);

		std::ifstream file(filePath);
		if (!file.is_open()) {
			_log->error("Failed to open file for loading project: {}", filePath);
			return false;
		}
		nlohmann::ordered_json projectJson;
		file >> projectJson;

		if (projectJson.contains("title")) {
			Title = projectJson["title"].get<std::string>();
			_log->info("Project title loaded: {}", Title);
		} else {
			_log->error("Project JSON does not contain 'title' field");
			return false;
		}

		if (projectJson.contains("assets")) {
			auto assetsJson = projectJson["assets"];
			if (!Assets::LoadFromJSON(assetsJson)) {
				_log->error("Failed to load assets from project JSON");
				return false;
			}
			_log->info("Assets loaded successfully from project");
		} else {
			_log->error("Project JSON does not contain 'assets' field");
			return false;
		}

		if (projectJson.contains("inputActions")) {
			auto actionsJson = projectJson["inputActions"];
			if (!Input.LoadActionsFromJSON(actionsJson)) {
				_log->error("Failed to load input actions from project JSON");
				return false;
			}
			_log->info("Input actions loaded successfully from project");
		} else {
			_log->error("Project JSON does not contain 'inputActions' field");
			return false;
		}

		// TODO: Load scenes

		if (!Scenes.IsDefaultSceneExists()) {
			if (const auto defaultScene = Scenes.CreateDefaultScene()) {
				Scenes.SelectScene(defaultScene);
			}
		}

		file.close();
		if (file.fail()) {
			_log->error("Failed to read project data from file: {}", filePath);
			return false;
		}
		return true;
	}

	void Game::CloseProject() {
		Scenes.DestroyAll();
		Input.RemoveAllActions();
		Assets::UnloadAll();
		_log->info("Project closed successfully");
	}

	void Game::Update(float deltaTime) {
		if (!Scenes.GetCurrentScene()->IsPaused) { Scenes.GetCurrentScene()->Update(deltaTime); }
	}
}
