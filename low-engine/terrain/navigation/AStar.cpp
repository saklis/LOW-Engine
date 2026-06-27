#include "AStar.h"

#include <algorithm>
#include <unordered_set>

namespace LowEngine::Terrain::Navigation {
    std::vector<NavigationCell> AStar::FindPath(const sf::Vector2u& start, const sf::Vector2u& end, MovementType movementType) const {
        // Reset all nodes before starting pathfinding
        for (auto& cell : *_navGrid) {
            cell.Parent = nullptr;
            cell.DistanceFromStartNode = 0;
            cell.HeuristicDistanceToEndNode = 0;
            cell.TotalEstimatedCost = 0;
        }

        std::priority_queue<NavigationCell*, std::vector<NavigationCell*>, NodeComparator> openList; // Cells to be evaluated
        std::pmr::unordered_set<NavigationCell*> closedList; // Cells already evaluated

        NavigationCell* firstNode = &_navGrid->at(start.x + start.y * _width);
        firstNode->Parent = nullptr;

        openList.push(firstNode);

        while (!openList.empty()) {
            NavigationCell* currentNode = openList.top();
            openList.pop();

            if (closedList.contains(currentNode)) {
                continue; // already evaluated
            }

            if (currentNode->Position == end) {
                return ReconstructPath(currentNode);
            }

            // add node to closed list
            closedList.insert(currentNode);

            // process neighbors
            std::vector<NavigationCell*> neighbors = GetNeighbors(currentNode, _navGrid, _width, _height, movementType);
            for (auto neighbor: neighbors) {
                if (closedList.contains(neighbor)) {
                    continue; // already evaluated
                }

                float tentativeDistance = currentNode->DistanceFromStartNode + neighbor->MoveCost;

                if (tentativeDistance < neighbor->DistanceFromStartNode || neighbor->DistanceFromStartNode == 0) {
                    // best path so far
                    neighbor->Parent = currentNode;
                    neighbor->DistanceFromStartNode = tentativeDistance;
                    neighbor->HeuristicDistanceToEndNode = GetHeuristicCost(currentNode, neighbor);
                    neighbor->TotalEstimatedCost = neighbor->DistanceFromStartNode + neighbor->HeuristicDistanceToEndNode;

                    openList.push(neighbor);
                }
            }
        }

        return {}; // No path found. Return an empty path.
    }

    std::vector<NavigationCell*> AStar::GetNeighbors(NavigationCell* node, std::vector<NavigationCell>* navGrid, size_t width,
                                                     size_t height, MovementType movementType) {
        std::vector<NavigationCell*> neighbors;
        sf::Vector2u position = node->Position;
        for (int y = -1; y <= 1; ++y) {
            for (int x = -1; x <= 1; ++x) {
                if (x == 0 && y == 0) continue; // Skip the current node

                sf::Vector2i neighborPos(static_cast<int>(position.x) + x, static_cast<int>(position.y) + y);

                if (neighborPos.x >= 0 && neighborPos.x < width && neighborPos.y >= 0 && neighborPos.y < height) {
                    NavigationCell* neighbor = &navGrid->at(neighborPos.x + neighborPos.y * width);

                    if (movementType == MovementType::Walk && neighbor->IsWalkable) {
                        neighbors.push_back(neighbor);
                    } else if (movementType == MovementType::Swim && neighbor->IsSwimmable) {
                        neighbors.push_back(neighbor);
                    } else if (movementType == MovementType::Fly && neighbor->IsFlyable) {
                        neighbors.push_back(neighbor);
                    }
                }
            }
        }

        return neighbors;
    }

    float AStar::GetHeuristicCost(NavigationCell* node, NavigationCell* neighbor) {
        // Chebyshev Distance
        return static_cast<float>(std::max(std::abs(static_cast<int>(node->Position.x) - static_cast<int>(neighbor->Position.x)),
                                           std::abs(static_cast<int>(node->Position.y) - static_cast<int>(neighbor->Position.y))));
    }

    std::vector<NavigationCell> AStar::ReconstructPath(NavigationCell* endNode) {
        std::vector<NavigationCell> path;
        NavigationCell* currentNode = endNode;
        while (currentNode != nullptr) {
            path.push_back(*currentNode);
            currentNode = currentNode->Parent;
        }
        std::ranges::reverse(path);
        return path;
    }
}
