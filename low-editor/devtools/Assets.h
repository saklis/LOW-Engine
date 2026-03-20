#pragma once

namespace sf
{
	class Texture;
}

namespace LowEngine
{
	class EditorAssets
	{
		public:
		static bool LoadEditorAssets();
		static bool UnloadEditorAssets();

		static sf::Texture* PlayTexture();
		static sf::Texture* PauseTexture();
		static sf::Texture* StopTexture();
		static sf::Texture* NextTexture();
		static sf::Texture* LoopTexture();
		static sf::Texture* TrashTexture();
		static sf::Texture* ProjectIconTexture();
		static sf::Texture* SoundIconTexture();
		static sf::Texture* MusicIconTexture();

	protected:
		static EditorAssets* GetInstance() {
			static EditorAssets instance;
			return &instance;
		}

		sf::Texture* _playTexture;
		sf::Texture* _pauseTexture;
		sf::Texture* _stopTexture;
		sf::Texture* _nextTexture;
		sf::Texture* _loopTexture;
		sf::Texture* _trashTexture;
		sf::Texture* _projectIconTexture;
		sf::Texture* _soundIconTexture;
		sf::Texture* _musicIconTexture;
	};
}
