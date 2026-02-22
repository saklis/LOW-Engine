#include "Game.h"

#include <algorithm>

#include "scene/Scene.h"

#include "log/LogMemoryBufferSink.h"

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

		while (const std::optional event = Window.pollEvent()) {
			if (event->is<sf::Event::Closed>()) {
				OnWindowClosed();
				return false;
			}

			if (event->is<sf::Event::Resized>()) {
				auto windowSize = static_cast<sf::Vector2f>(Window.getSize());
				Scenes.GetCurrentScene()->SetWindowSize(windowSize);
			}
			
			Input.Read(event);
			WindowEvents.emplace_back(event);
		}
		
		Input.Update();

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
		projectJson["defaultSceneName"] = DefaultSceneName;
		projectJson["assets"] = Assets::SerializeToJSON(ProjectDirectory);
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
			ProjectDirectory = std::filesystem::path(filePath).parent_path();

			Window.setTitle(Title);
			_log->info("Project title loaded: {}", Title);
		} else {
			_log->error("Project JSON does not contain 'title' field");
			return false;
		}

		Assets::LoadDefaultAssets();
		if (projectJson.contains("assets")) {
			auto assetsJson = projectJson["assets"];
			if (!Assets::LoadFromJSON(assetsJson, ProjectDirectory)) {
				_log->error("Failed to load assets from project JSON");
				return false;
			}
			_log->info("Assets loaded successfully from project");
		} else {
			_log->warn("Project JSON does not contain 'assets' field");
		}

		if (projectJson.contains("inputActions")) {
			auto actionsJson = projectJson["inputActions"];
			if (!Input.LoadActionsFromJSON(actionsJson)) {
				_log->error("Failed to load input actions from project JSON");
				return false;
			}
			_log->info("Input actions loaded successfully from project");
		} else {
			_log->warn("Project JSON does not contain 'inputActions' field");
		}

		// TODO: Load scenes

		if (!Scenes.DefaultSceneExists()) {
			if (const auto defaultScene = Scenes.CreateDefaultScene()) {
				DefaultSceneName = defaultScene->Name;
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

	bool Game::SaveCurrentScene() {
		std::filesystem::path sceneFilePath = ProjectDirectory / Config::SCENES_FOLDER_NAME;
		std::filesystem::create_directories(sceneFilePath);
		sceneFilePath /= Scenes.GetCurrentScene()->Name + Config::SCENE_FILE_EXTENSION;
		
		std::ofstream file(sceneFilePath);
		if (!file.is_open()) {
			_log->error("Failed to open file for saving scene: {}", sceneFilePath.string());
			return false;
		}

		_log->debug("Saving scene to file: {}", sceneFilePath.string());

		auto sceneJson = Scenes.GetCurrentScene()->SerializeToJSON();

		file << sceneJson.dump(4); // pretty print with 4 spaces

		file.close();
		if (file.fail()) {
			_log->error("Failed to write scene data to file: {}", sceneFilePath.string());
			return false;
		}
		_log->info("Scene saved successfully to: {}", sceneFilePath.string());

		return true;
	}

	void Game::CloseProject() {
		Scenes.DestroyAll();
		Input.RemoveAllActions();
		Assets::UnloadAll();
		_log->info("Project closed successfully");
	}

	void Game::LoadScene(const std::string& sceneName)
	{
		std::filesystem::path sceneFilePath = ProjectDirectory / Config::SCENES_FOLDER_NAME / (sceneName + Config::SCENE_FILE_EXTENSION);
		_log->info("Loading scene from file: {}", sceneFilePath.string());
		std::ifstream file(sceneFilePath);
		if (!file.is_open()) {
			_log->error("Failed to open scene file: {}", sceneFilePath.string());
			return;
		}
		nlohmann::ordered_json sceneJson;
		file >> sceneJson;
		auto scene = Scenes.CreateEmptyScene(sceneName);
		if (!scene) {
			_log->error("Failed to create empty scene: {}", sceneName);
			return;
		}
		if (!scene->DeserializeFromJSON(sceneJson)) {
			_log->error("Failed to load scene data from JSON: {}", sceneName);
			return;
		}
		Scenes.SelectScene(scene);
		_log->info("Scene loaded successfully: {}", sceneName);
	}

	void Game::Update(float deltaTime) {
		_fixedUpdateAccumulator += deltaTime;
		_fixedUpdateAccumulator = std::min(_fixedUpdateAccumulator, _maxFixedDeltaTime);

		if (_fixedUpdateAccumulator >= _fixedDeltaTime) {
			Scenes.GetCurrentScene()->FixedUpdate(_fixedDeltaTime);
			_fixedUpdateAccumulator -= _fixedDeltaTime;
		}

		Scenes.GetCurrentScene()->Update(deltaTime);
		Music.Update(deltaTime);
	}

}
