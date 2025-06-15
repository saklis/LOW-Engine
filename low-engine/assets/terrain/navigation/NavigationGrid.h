#pragma once

#include <vector>

#include "SFML/System/Vector2.hpp"
#include "NavigationCell.h"
#include "AStar.h"

namespace LowEngine::Terrain::Navigation {
    /**
     * @brief Navigation grid that holds navigation data for the map.
     *
     * This grid is used for pathfinding and other navigation-related tasks.
     */
    class NavigationGrid {
    public:
        /**
         * @brief Width of the navigation grid in cells.
         */
        size_t Width = 0;
        /**
         * @brief Height of the navigation grid in cells.
         */
        size_t Height = 0;

        /**
         * @brief Collection of cells in this NavGrid.
         */
        std::vector<NavigationCell> Cells;

        /**
         * @brief Find a path from start to end position on the navigation grid.
         *
         * @param start Starting position in NavGrid Space coordinates.
         * @param end Ending position in NavGrid Space coordinates.
         * @param movementType Type of movement (walk, swim, fly).
         * @return A vector of NavigationCell representing the path from start to end (with NavGrid Space positions). Returns empty vector if path is not found.
         */
        [[nodiscard]] std::vector<NavigationCell> FindPath(const sf::Vector2u& start, const sf::Vector2u& end, MovementType movementType);
    };
}
