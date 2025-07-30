#pragma once

#include "SFML/Audio/SoundBuffer.hpp"

namespace LowEngine::Files {
	class SoundBuffer : public sf::SoundBuffer {
	public:
		/**
		 * @brief Path to the sound file.
		 *
		 * This is used to store the path from which the sound was loaded.
		 */
		std::string Path;

		SoundBuffer() = default;
		SoundBuffer(const std::string& path)
			: Path(path) {
			if (!loadFromFile(path)) {
				throw std::runtime_error("Failed to load sound from file: " + path);
			}
		}
	};
}