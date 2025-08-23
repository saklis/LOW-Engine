#pragma once
#include "SFML/Audio/Music.hpp"

namespace LowEngine::Files {
    class Music : public sf::Music {
    public:
        /**
         * @brief Path to the music file.
         *
         * This is used to store the path from which the music was loaded.
         */
        std::filesystem::path Path;

        Music() = default;
        Music(const std::string& path)
            : Path(std::filesystem::path(path).lexically_normal()) {
            if (!openFromFile(path)) {
                throw std::runtime_error("Failed to load music from file: " + path);
            }
        }
    };
}
