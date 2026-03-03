#include "tiles.hpp"
#include "types.hpp"
#include "boards.hpp"
#include "pieces.hpp"

#include <cstdlib>
#include <iostream>
#include <unordered_map>
#include <queue>
#include <algorithm>

TerrainType getRandomTerrain(float noise, BiomeType biome) {
    switch (biome) {
        case BiomeType::Temperate:
            if (noise < 0.3f) return TerrainType::Water;
            if (noise < 0.45f) return TerrainType::Field;
            if (noise < 0.7f) return TerrainType::Forest;
            return TerrainType::Mountain;
        case BiomeType::Grassy:
            if (noise < 0.3f) return TerrainType::Water;
            if (noise < 0.55f) return TerrainType::Field;
            if (noise < 0.7f) return TerrainType::Forest;
            return TerrainType::Mountain;
        case BiomeType::Arid:
            if (noise < 0.15f) return TerrainType::Water;
            if (noise < 0.45f) return TerrainType::Field;
            if (noise < 0.75f) return TerrainType::Desert;
            if (noise < 0.9f) return TerrainType::Forest;
            return TerrainType::Mountain;
        case BiomeType::Tropical:
            if (noise < 0.3f) return TerrainType::Water;
            if (noise < 0.4f) return TerrainType::Field;
            if (noise < 0.55f) return TerrainType::Forest;
            if (noise < 0.8f) return TerrainType::Jungle;
            return TerrainType::Mountain;
        case BiomeType::Alpine:
            if (noise < 0.1f) return TerrainType::Water;
            if (noise < 0.3f) return TerrainType::Field;
            if (noise < 0.5f) return TerrainType::Forest;
            if (noise < 0.75f) return TerrainType::Mountain;
            return TerrainType::Peak;
        case BiomeType::Arctic:
            if (noise < 0.2f) return TerrainType::Water;
            if (noise < 0.4f) return TerrainType::IceField;
            if (noise < 0.6f) return TerrainType::SnowField;
            if (noise < 0.8f) return TerrainType::Tundra;
            return TerrainType::Mountain;
        default: 
            return TerrainType::Field;
    }
}

std::vector<Tile *> generateRoad(Tile * startTile, Tile * endTile, Board &board) {
    std::cout << "Starting A* search algorithm" << std::endl;
    // Utility functions
    auto getTileId = [&board](Tile *tile) { 
        return tile->x + board.width * tile->y; 
    };

    auto h = [endTile](Tile * a) { // Lambda function for h 
        return std::abs(endTile->x - a->x) + std::abs(endTile->y - a->y);
    };

    auto getCost = [](TerrainType terrain) { // gScore
        switch(terrain) {
            case TerrainType::Field:    return 1;
            case TerrainType::Forest:   return 3;
            case TerrainType::Mountain: return 10;
            case TerrainType::Desert:   return 2;
            case TerrainType::Jungle:   return 5;
            case TerrainType::Peak:     return 20;
            default:                    return 999; // Includes roads, water, and arctic environments
        }
    };

    auto getNeighbors = [&board] (Tile *tile) {
        std::vector<int> neighbors;
        int vectors[4][2] = { { 1, 0},
                              { 0, 1},
                              {-1, 0},
                              { 0,-1} };
        for (int i = 0; i < 4; i++) {
            int nx = tile->x + vectors[i][0];
            int ny = tile->y + vectors[i][1];
            if (nx >= 0 && nx < board.width && ny >= 0 && ny < board.height) {
                neighbors.push_back(tile->x + vectors[i][0] + board.width * (tile->y + vectors[i][1]));
            }
        }
        return neighbors;
    };

    // A* Search to find road pathing
    int startTileId = getTileId(startTile);

    std::unordered_map<int, int> closedList; // cameFrom := an empty map
    std::unordered_map<int, int> gScore; // gScore := map with default value of Infinity
                                          // maps TileId to gScore
    gScore[startTileId] = 0; // gScore[start] := 0

    std::unordered_map<int, int> fScore; // fScore := map with default value of Infinit
                                          // maps TileId to fScore
    fScore[startTileId] = h(startTile); // fScore[start] := h(start)
    
    auto compare = [&fScore](int a, int b) { // Lambda function to determine node priority
        return fScore[a] > fScore[b]; // cursed af
    };

    std::priority_queue<int, std::vector<int>, decltype(compare)> openSet(compare); // openSet := {start}
    std::vector<int> checkSet; // for checking contents of openSet

    openSet.push(startTileId);
    checkSet.push_back(startTileId);
    
    std::cout << "Algorithm initiated at (" << startTile->x+1 << ", " << startTile->y+1 << ")." << std::endl;
    std::cout << "Algorithm target is (" << endTile->x+1 << ", " << endTile->y+1 << ")." << std::endl;

    while (!(openSet.empty())) { // while openSet is not empty
        int current = openSet.top(); // current := the node in openSet having the lowest fScore[] value
                                 // Because the openSet comp lambda sorts by lowest f, it will always be the top of openSet
        if (current == getTileId(endTile)) { // if current = goal 
            std::cout << "Road path found, generating path vector." << std::endl;
            std::vector<Tile *> path;
            path.push_back(&board.tiles[current]); // total_path := {current}

            while (closedList.count(current) == 1) { // while current in cameFrom.Keys:
                current = closedList[current]; // current := cameFrom[current]
                path.push_back(&board.tiles[current]); // total_path.prepend(current)
            }
            return path; // return total_path  
        }        
        
        openSet.pop(); // openSet.Remove(current)
        checkSet.erase(find(checkSet.begin(), checkSet.end(), current)); // remove current from the check set
        
        std::vector<int> neighbors = getNeighbors(&board.tiles[current]);
        for (int i = 0; i < neighbors.size(); i++) { // for each neighbor of current
            int tenativeG = gScore[current] + getCost(board.tiles[neighbors[i]].terrain); // tenative_gScore := gScore[current] + d(current, neighbor) 
            if (gScore.find(neighbors[i]) == gScore.end() || tenativeG < gScore[neighbors[i]]) { // if tenative_gScore > gScore[neighbor]
                closedList[neighbors[i]] = current; // cameFrom[neighbor] := current
                gScore[neighbors[i]] = tenativeG; // gScore[neighbor] := tenative_gScore
                fScore[neighbors[i]] = tenativeG + h(&board.tiles[neighbors[i]]); // fScore[neighbor] := tenative_gScore + h(neighbor)
                
                if (find(checkSet.begin(), checkSet.end(), neighbors[i]) == checkSet.end()) { // if neighbor not in openSet
                    openSet.push(neighbors[i]); // openSet.add(neighbor)
                    checkSet.push_back(neighbors[i]); // and same for the check set
                }
            }

        }

    }
    std::cout << "Algorithm failed." << std::endl;
    std::vector<Tile *> failure = { nullptr };
    return failure;
}

std::string getTileSymbol(const Tile &tile) {
    std::string symbol = " ";  // Default: empty space
    std::string pieceColor = ""; // Default: no color
    
    if (tile.terrain == TerrainType::Water) return "\033[34m[~~]\033[0m"; // Water tiles can never hold pieces

    if (tile.occupyingPiece) {
        // Determine piece letter
        switch (tile.occupyingPiece->type) {
            case PieceType::Light:     symbol = "P"; break;
            case PieceType::Shield:    symbol = "S"; break;
            case PieceType::Elite:     symbol = "E"; break;
            case PieceType::Archer:    symbol = "A"; break;
            case PieceType::LCavalry:  symbol = "L"; break;
            case PieceType::MCavalry:  symbol = "M"; break;
            case PieceType::HCavalry:  symbol = "H"; break;
            case PieceType::Commander: symbol = "C"; break;
            case PieceType::Wizard:    symbol = "W"; break;
        }
        
        // Color enemy pieces red
        if (!tile.occupyingPiece->ownedByPlayer) {
            pieceColor = "\033[31m";  // Red
        }
    }
    
    // Get terrain bracket color
    std::string bracketColor;
    switch (tile.terrain) {
        case TerrainType::Field:    bracketColor = "\033[93m"; break;  // Yellow
        case TerrainType::Forest:   bracketColor = "\033[92m"; break;  // Green
        case TerrainType::Mountain: bracketColor = "\033[35m"; break;  // Purple
        case TerrainType::Road:     bracketColor = "\033[91m"; break;  // HI Red
        case TerrainType::Desert:   bracketColor = "\033[33m"; break;  // HI Yellow
        case TerrainType::Jungle:   bracketColor = "\033[32m"; break;  // HI Green
        case TerrainType::Peak:     bracketColor = "\033[30m"; break;  // Black
        // Arctic Tiles
        case TerrainType::IceField: bracketColor = "\033[36m"; break;  // Cyan
        case TerrainType::SnowField:bracketColor = "\033[37m"; break;  // White
        case TerrainType::Tundra:   bracketColor = "\033[32m"; break;  // Green again, but it's fine because it replaces forests
    }
    
    // Construct final symbol
    return bracketColor + "[ " + "\033[39m" + 
           pieceColor + symbol + "\033[39m" + 
           bracketColor + "]" + "\033[39m";
}
