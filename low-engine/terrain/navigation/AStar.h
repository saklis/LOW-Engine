#pragma once

#include <vector>
#include <queue>
#include "NavigationCell.h"

namespace LowEngine::Terrain::Navigation {

    /**
     * @brief Comparator for NavigationCell pointers based on TotalEstimatedCost.
     */
    struct NodeComparator {
        bool operator()(const NavigationCell* a, const NavigationCell* b) const {
            return a->TotalEstimatedCost > b->TotalEstimatedCost;
        }
    };

    /**
     * @brief A* Pathfinding algorithm implementation.
     *
     * This class provides methods for finding paths on a navigation grid using the A* algorithm.
     */
    class AStar {
    public:
        /**
         * @brief Constructor for AStar pathfinding algorithm.
         *
         * @param navGrid Navigation grid containing cells for pathfinding.
         */
        AStar(std::vector<NavigationCell>* navGrid, size_t width, size_t height)
                    : _navGrid(navGrid), _width(width), _height(height) {
        }

        /**
         * @brief Find a path from start to end position on the navigation grid.
         *
         * @param start Starting position (in NavGrid coords) in the navigation grid.
         * @param end Ending position (in NavGrid coords) in the navigation grid.
         * @param movementType Type of movement (walk, swim, fly).
         * @return A vector of NavigationCell representing the path from start to end. Returns empty vector if path is not found.
         */
        [[nodiscard]] std::vector<NavigationCell> FindPath(const sf::Vector2u& start, const sf::Vector2u& end, MovementType movementType) const;

    protected:
        /**
         * @brief Width of the navigation grid in cells.
         */
        size_t _width = 0;
        /**
         * @brief Height of the navigation grid in cells.
         */
        size_t _height = 0;
        /**
         * @brief Collection of cells in this NavGrid.
         */
        std::vector<NavigationCell>* _navGrid;

        /**
         * @brief Get the neighbors of a given node in the navigation grid.
         *
         * @param node The NavigationCell for which to find neighbors.
         * @param navGrid The navigation grid containing all cells.
         * @param width Width of the navigation grid in cells.
         * @param height Height of the navigation grid in cells.
         * @param movementType Type of movement (walk, swim, fly).
         * @return A vector of pointers to neighboring NavigationCell objects.
         */
        [[nodiscard]] static std::vector<NavigationCell*> GetNeighbors(NavigationCell* node, std::vector<NavigationCell>* navGrid, size_t width, size_t height, MovementType movementType);

        /**
         * @brief Calculate the heuristic cost between two navigation cells.
         *
         * This function estimates the cost to move from one cell to another based on their positions.
         *
         * @param node The current NavigationCell.
         * @param neighbor The neighboring NavigationCell.
         * @return The heuristic cost as a float.
         */
        [[nodiscard]] static float GetHeuristicCost(NavigationCell* node, NavigationCell* neighbor);

        /**
         * @brief Reconstruct the path from the end node back to the start node.
         *
         * This function traces back the parent pointers from the end node to reconstruct the path.
         *
         * @param endNode The NavigationCell at the end of the path.
         * @return A vector of NavigationCell representing the reconstructed path. Returns empty vector if no path was found.
         */
        [[nodiscard]] static std::vector<NavigationCell> ReconstructPath(NavigationCell* endNode);
    };
}
