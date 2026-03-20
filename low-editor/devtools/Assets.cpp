#include "Assets.h"

#include "SFML/Graphics/Texture.hpp"


namespace LowEngine {
	bool EditorAssets::LoadEditorAssets()
	{
		GetInstance()->_playTexture = new sf::Texture();
		if (!GetInstance()->_playTexture->loadFromFile("assets/editor/icons/forward.png"))
			return false;

		GetInstance()->_pauseTexture = new sf::Texture();
		if (!GetInstance()->_pauseTexture->loadFromFile("assets/editor/icons/pause.png"))
			return false;

		GetInstance()->_stopTexture = new sf::Texture();
		if (!GetInstance()->_stopTexture->loadFromFile("assets/editor/icons/stop.png"))
			return false;

		GetInstance()->_nextTexture = new sf::Texture();
		if (!GetInstance()->_nextTexture->loadFromFile("assets/editor/icons/next.png"))
			return false;

		GetInstance()->_loopTexture = new sf::Texture();
		if (!GetInstance()->_loopTexture->loadFromFile("assets/editor/icons/return.png"))
			return false;

		GetInstance()->_trashTexture = new sf::Texture();
		if (!GetInstance()->_trashTexture->loadFromFile("assets/editor/icons/trashcan.png"))
			return false;

		GetInstance()->_projectIconTexture = new sf::Texture();
		if (!GetInstance()->_projectIconTexture->loadFromFile("assets/editor/icons/projecticon.png"))
			return false;

		GetInstance()->_soundIconTexture = new sf::Texture();
		if (!GetInstance()->_soundIconTexture->loadFromFile("assets/editor/icons/soundicon.png"))
			return false;

		GetInstance()->_musicIconTexture = new sf::Texture();
		if (!GetInstance()->_musicIconTexture->loadFromFile("assets/editor/icons/musicicon.png"))
			return false;

		return true;
	}

	bool EditorAssets::UnloadEditorAssets()
	{
		delete GetInstance()->_playTexture;
		GetInstance()->_playTexture = nullptr;

		delete GetInstance()->_pauseTexture;
		GetInstance()->_pauseTexture = nullptr;

		delete GetInstance()->_stopTexture;
		GetInstance()->_stopTexture = nullptr;

		delete GetInstance()->_nextTexture;
		GetInstance()->_nextTexture = nullptr;

		delete GetInstance()->_loopTexture;
		GetInstance()->_loopTexture = nullptr;

		delete GetInstance()->_trashTexture;
		GetInstance()->_trashTexture = nullptr;

		delete GetInstance()->_projectIconTexture;
		GetInstance()->_projectIconTexture = nullptr;

		delete GetInstance()->_soundIconTexture;
		GetInstance()->_soundIconTexture = nullptr;

		delete GetInstance()->_musicIconTexture;
		GetInstance()->_musicIconTexture = nullptr;
		return true;
	}

	sf::Texture* EditorAssets::PlayTexture()
	{
		return GetInstance()->_playTexture;
	}

	sf::Texture* EditorAssets::PauseTexture()
	{
		return GetInstance()->_pauseTexture;
	}

	sf::Texture* EditorAssets::StopTexture()
	{
		return GetInstance()->_stopTexture;
	}

	sf::Texture* EditorAssets::NextTexture()
	{	
		return GetInstance()->_nextTexture;
	}

	sf::Texture* EditorAssets::LoopTexture()
	{
		return GetInstance()->_loopTexture;
	}

	sf::Texture* EditorAssets::TrashTexture()
	{
		return GetInstance()->_trashTexture;
	}

	sf::Texture* EditorAssets::ProjectIconTexture()
	{
		return GetInstance()->_projectIconTexture;
	}

	sf::Texture* EditorAssets::SoundIconTexture()
	{
		return GetInstance()->_soundIconTexture;
	}

	sf::Texture* EditorAssets::MusicIconTexture()
	{	
		return GetInstance()->_musicIconTexture;
	}
}
