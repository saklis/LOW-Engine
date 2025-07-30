#pragma once

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
		std::string Path;

		Texture() = default;
		Texture(const std::string& path)
			: Path(path)
		{
			if (!loadFromFile(path)) {
				throw std::runtime_error("Failed to load texture from file: " + path);
			}
		}
	};
}
