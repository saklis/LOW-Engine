#pragma once

#include <filesystem>
#include "SFML/Graphics/Texture.hpp"

namespace LowEngine::Files
{
	class Texture : public sf::Texture
	{
	public:
		/**
		 * @brief Path to the texture file.
		 *
		 * This is used to store the path from which the texture was loaded.
		 */
		std::filesystem::path Path;

		Texture() = default;
		Texture(const std::string& path)
			: Path(std::filesystem::path(path).lexically_normal())
		{
			if (!loadFromFile(path)) {
				throw std::runtime_error("Failed to load texture from file: " + path);
			}
		}
	};
}
