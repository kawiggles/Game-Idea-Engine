#include "tiles.hpp"
#include "types.hpp"
#include "pieces.hpp"

#include <cstdlib>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <algorithm>

/*
 * Tiles are the main structure referenced by GameInstance methods.
 * Tiles hold three essential bits of informations:
 *     - The (x, y) coordinate of the tile
 *     - The terrain type of the tile
 *     - A pointer to the piece occupying the tile
 * Because of the way boards are constructed, any tile at coordiate (x, y) can be referenced by using the board vector index x * boardWidth + y
 * getRandomTerrain determines the terrain value of a tile by refrencing an associated perlin noise map and GameInstance biome
 * generateRoad uses an A* search algorithm to generate a vector of tiles which are converted to the road terrain type in makeGame
 */
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

std::vector<Tile *> generateRoad(Tile * startTile, Tile * endTile, std::vector<Tile> &board, int width, int height) {
    std::cout << "Starting A* search algorithm" << std::endl;
    // Utility lambda functions:
    // copy of getTile because the board is not generated yet
    auto getTileId = [width](Tile *tile) { 
        return tile->x + width * tile->y; 
    };
    
    // Manhattan distance heuristic function h(x) 
    auto h = [endTile](Tile * a) { 
        return std::abs(endTile->x - a->x) + std::abs(endTile->y - a->y);
    };

    // Cost function, g(x)
    auto getCost = [](TerrainType terrain) {
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

    // Get's all cardinally neighboring tiles by index
    auto getNeighbors = [width, height] (Tile &tile) {
        std::vector<int> neighbors;
        int vectors[4][2] = { { 1, 0},
                              { 0, 1},
                              {-1, 0},
                              { 0,-1} };
        for (int i = 0; i < 4; i++) {
            int nx = tile.x + vectors[i][0];
            int ny = tile.y + vectors[i][1];
            if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                neighbors.push_back(tile.x + vectors[i][0] + width * (tile.y + vectors[i][1]));
            }
        }
        return neighbors;
    };

    // A* Search Algorithm, comments detail associated pseudocode from wikipedia (thank's wikipedia)
    int startTileId = getTileId(startTile);

    std::unordered_map<int, int> closedList; // cameFrom := an empty map
    std::unordered_map<int, int> gScore; // gScore := map with default value of Infinity
    gScore[startTileId] = 0; // gScore[start] := 0

    std::unordered_map<int, int> fScore; // fScore := map with default value of Infinit
    fScore[startTileId] = h(startTile); // fScore[start] := h(start)
    
    auto compare = [&fScore](int a, int b) { // Lambda function to determine node priority
        return fScore[a] > fScore[b]; // cursed af
    }; // It needs to be here because fScore has to be declared first

    // This is also out of order because compare is needed to define a min sort priority queue, these are the only things "out of order"
    std::priority_queue<int, std::vector<int>, decltype(compare)> openSet(compare); 
    std::vector<int> checkSet; // for checking contents of openSet

    openSet.push(startTileId); // openSet := {start}
    checkSet.push_back(startTileId);
    
    std::cout << "Algorithm initiated at (" << startTile->x+1 << ", " << startTile->y+1 << ")." << std::endl;
    std::cout << "Algorithm target is (" << endTile->x+1 << ", " << endTile->y+1 << ")." << std::endl;

    while (!(openSet.empty())) { // while openSet is not empty
        int current = openSet.top(); // current := the node in openSet having the lowest fScore[] value
                                 // Because the openSet comp lambda sorts by lowest f, it will always be the top of openSet
        if (current == getTileId(endTile)) { // if current = goal 
            std::cout << "Road path found, generating path vector." << std::endl;
            std::vector<Tile *> path;
            path.push_back(&board[current]); // total_path := {current}

            while (closedList.count(current) == 1) { // while current in cameFrom.Keys:
                current = closedList[current]; // current := cameFrom[current]
                path.push_back(&board[current]); // total_path.prepend(current)
            }
            return path; // return total_path  
        }        
        
        openSet.pop(); // openSet.Remove(current)
        checkSet.erase(find(checkSet.begin(), checkSet.end(), current)); // remove current from the check set
        
        std::vector<int> neighbors = getNeighbors(board[current]);
        for (int i = 0; i < neighbors.size(); i++) { // for each neighbor of current
            int tenativeG = gScore[current] + getCost(board[neighbors[i]].terrain); // tenative_gScore := gScore[current] + d(current, neighbor) 
            if (gScore.find(neighbors[i]) == gScore.end() || tenativeG < gScore[neighbors[i]]) { // if tenative_gScore > gScore[neighbor]
                closedList[neighbors[i]] = current; // cameFrom[neighbor] := current
                gScore[neighbors[i]] = tenativeG; // gScore[neighbor] := tenative_gScore
                fScore[neighbors[i]] = tenativeG + h(&board[neighbors[i]]); // fScore[neighbor] := tenative_gScore + h(neighbor)
                
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
            case PieceType::Catapult:  symbol = "T"; break;
            case PieceType::Ballista:  symbol = "B"; break;
            case PieceType::Chariot:   symbol = "C"; break;
            case PieceType::Commander: symbol = "G"; break;
            case PieceType::Wizard:    symbol = "W"; break;
            case PieceType::Assassin:  symbol = "X"; break;
            case PieceType::Druid:     symbol = "D"; break;
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
        // Mission Tiles
        case TerrainType::Objective:bracketColor = "\033[94m"; break;  // HI Blue
    }
    
    // Construct final symbol
    return bracketColor + "[ " + "\033[39m" + 
           pieceColor + symbol + "\033[39m" + 
           bracketColor + "]" + "\033[39m";
}
