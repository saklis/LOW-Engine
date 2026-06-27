#pragma once
#include <utility>

#include "SFML/System/Vector2.hpp"

namespace LowEngine::Terrain::Navigation {
    /**
     * @brief Type of movement for pathfinding.
     */
    enum MovementType {
        Walk,
        Swim,
        Fly
    };

    /**
     * @brief Single cell of NavGrid.
     */
    class NavigationCell {
    public:
        /**
         * @brief Position of this cell in the NavGrid.
         */
        sf::Vector2u Position;

        /**
         * @brief Can entity move to this cell if they are walking?
         */
        bool IsWalkable = false;
        /**
         * @brief Can entity move to this cell if they are swimming?
         */
        bool IsSwimmable = false;
        /**
         * @brief Can entity move to this cell if they are flying?
         */
        bool IsFlyable = false;

        /**
         * @brief Cost of moving to this cell.
         */
        float MoveCost = 1.0f;

        /**
         * @brief Pointer to the parent cell in the pathfinding tree.
         *
         * This is used to reconstruct the path after pathfinding is complete through A* algorithm.
         */
        NavigationCell* Parent = nullptr;

        /**
         * @brief Cost of moving to this cell from the start position. (F)
         *
         * This is used in A* algorithm to determine the best path.
         */
        float TotalEstimatedCost = 0.0f;

        /**
         * @brief Distance from the start node to this cell. (G)
         *
         * This is used in A* algorithm to determine the best path.
         */
        float DistanceFromStartNode = 0.0f;

        /**
         * @brief Heuristic distance to the end node. (H)
         *
         * This is used in A* algorithm to determine the best path.
         */
        float HeuristicDistanceToEndNode = 0.0f;

        NavigationCell() = default;
    };
}
