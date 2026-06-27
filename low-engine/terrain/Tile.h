#pragma once
#include <cstdint>
#include <string>

#include "SFML/Graphics/Rect.hpp"

namespace LowEngine::TileMap {
    enum class TileType : std::uint8_t {
        Static,
        Animated
    };

    /**
     * @brief Traversal bitflags for Tile::TraversalMask.
     *
     * Each flag represents one movement type an entity can use to enter a tile.
     * Values are powers of two so they can be combined with bitwise OR and
     * tested with bitwise AND.
     *
     * @warning Bit positions MUST match the Navigation::MovementType enum
     * values (Walk=0, Swim=1, Fly=2) so that pathfinding can check a cell
     * with `mask & (1 << movementType)`. Reordering MovementType without
     * updating these constants will silently break pathfinding.
     *
     * @par Usage
     *
     * Assign a combination of flags:
     * @code
     * tile.TraversalMask = Traversal::Walk | Traversal::Fly;   // walkers + fliers only
     * tile.TraversalMask = Traversal::All;                     // every movement type (default)
     * tile.TraversalMask = Traversal::None;                    // fully blocked
     * @endcode
     *
     * Test whether a movement type is allowed:
     * @code
     * if (tile.TraversalMask & Traversal::Walk) {
     *     // entity on foot may enter this tile
     * }
     * @endcode
     *
     * Add or remove a single flag in place:
     * @code
     * tile.TraversalMask |= Traversal::Swim;    // also allow swimming
     * tile.TraversalMask &= ~Traversal::Fly;    // disallow flying over this tile
     * @endcode
     */
    struct Traversal {
        /**
         * @brief No traversal allowed. Tile blocks every movement type.
         */
        static constexpr std::uint8_t None = 0;

        /**
         * @brief Ground-based movement (walking, running, driving).
         */
        static constexpr std::uint8_t Walk = 1 << 0;

        /**
         * @brief Water-based movement (swimming, boats).
         */
        static constexpr std::uint8_t Swim = 1 << 1;

        /**
         * @brief Air-based movement (flying units).
         */
        static constexpr std::uint8_t Fly = 1 << 2;

        /**
         * @brief All movement types allowed. Convenience alias for the default passable tile.
         */
        static constexpr std::uint8_t All = Walk | Swim | Fly;
    };

    class Tile {
    public:
        TileType Type = TileType::Static;
        sf::IntRect SpriteRect = sf::IntRect();

        std::string AnimationClipName = std::string();

        float AnimSpriteTimer = 0.0f;
        std::size_t AnimSpriteCurrentFrame = 0;

        bool HasCollision = false;
        std::uint8_t TraversalMask = Traversal::All;

        std::uint8_t EntryCost = 1;

    protected:
    };
}
