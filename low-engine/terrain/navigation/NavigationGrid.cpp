#include "NavigationGrid.h"

std::vector<LowEngine::Terrain::Navigation::NavigationCell> LowEngine::Terrain::Navigation::NavigationGrid::FindPath(const sf::Vector2u& start,
    const sf::Vector2u& end, MovementType movementType) {
    AStar aStar(&Cells, Width, Height);
    return aStar.FindPath(start, end, movementType);
}
