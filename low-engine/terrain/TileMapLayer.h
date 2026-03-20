#pragma once
#include "assets/Assets.h"
#include "graphics/Sprite.h"

namespace LowEngine::TileMap {
	class TileMapLayer {
	public:
		TileMapLayer() = default;

		/**
		 * @brief Name of this layer.
		 */
		std::string Name = "Terrain Layer";

		/**
		 * @brief The draw order of all sprites in this layer.
		 *
		 * Sprites with lower draw orders will be drawn first, and thus appear behind sprites with higher draw orders.
		 * The default draw order is 0.
		 */
		int DrawOrder = 0;

		/**
		 * @brief Id of the texture that is used by this layer as a source of tiles.
		 */
		std::size_t TextureId = 0;
	protected:
	};
}
