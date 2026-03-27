#pragma once
#include "tiles.hpp"
#include "types.hpp"
#include "pieces.hpp"

#include <ncurses.h>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <memory>
#include <cmath>

/*
 * Tiles are the main structure referenced by GameInstance methods.
 * Tiles hold three essential bits of informations:
 *     - The (x, y) coordinate of the tile
 *     - The terrain type of the tile
 *     - A pointer to the piece occupying the tile
 * Tiles are also polymorphic, with a child class for each possible TerrainType. These polymorphic classes contain instances of virtual methods which return terrain modifier values (Roads are weird)
 * Because of the way boards are constructed, any tile at coordiate (x, y) can be referenced by using the board vector index x * boardWidth + y
 * getRandomTerrain determines the terrain value of a tile by refrencing an associated perlin noise map and GameInstance biome, and generates a unique pointer to that tile
 * generateRoad uses an A* search algorithm to generate a vector of tiles which are converted to the road terrain type in makeGame
 */

int Water::getMovementMod(const Piece &piece) { return -1000; }
int Forest::getMovementMod(const Piece &piece) {
    if ((piece.category == PieceCategory::Cavalry && 
         piece.type != PieceType::LCavalry) || 
         piece.category == PieceCategory::Siege) return -1;
    return 0;
}
int Mountain::getMovementMod(const Piece &piece) {
    if (piece.category == PieceCategory::Cavalry || 
        piece.category == PieceCategory::Siege) return -1000;
    return -1;
}
int Road::getRoadMovementMod(const Piece &piece, Tile * nextTile) {
    if (nextTile != nullptr &&
        nextTile->terrain != TerrainType::Water &&
        nextTile->terrain == TerrainType::Road) return 1;
    return 0;
}
int Desert::getMovementMod(const Piece &piece) { return -1; }
int Jungle::getMovementMod(const Piece &piece) {
    if (piece.category == PieceCategory::Siege) return -100;
    int mod = 0;
    if (!(piece.type == PieceType::Light || piece.type == PieceType::LCavalry)) mod--;
    if (piece.category == PieceCategory::Cavalry) mod--;
    return mod;
}
int Peak::getMovementMod(const Piece &piece) { return -1000; }
int SnowField::getMovementMod(const Piece &piece) { return -1; }

int Forest::getToughnessMod() { return 1; }
int Mountain::getToughnessMod() { return 1; }
int Desert::getToughnessMod() { return -1; }
int Jungle::getToughnessMod() { return 1; }
int Peak::getToughnessMod() { return 2; }
int Objective::getToughnessMod() { return 1; }

int Field::getStrengthMod(const Piece &piece) {
    if (piece.category == PieceCategory::Cavalry) return 1;
    return 0;
}
int Forest::getStrengthMod(const Piece &piece) {
    if (piece.category == PieceCategory::Infantry) return 1;
    return 0;
}
int Road::getStrengthMod(const Piece &piece) {
    if (piece.category == PieceCategory::Cavalry) return 1;
    return 0;
}
int Desert::getStrengthMod(const Piece &piece) { return -1; }
int Jungle::getStrengthMod(const Piece &piece) {
    if (piece.type == PieceType::Light || piece.type == PieceType::LCavalry) return 0;
    return -1;
}

int Mountain::getRangedStrengthMod(const Piece &piece) {
    if (piece.category == PieceCategory::Siege) return 1;
    return 0;
}
int Jungle::getRangedStrengthMod(const Piece &piece) { return -1; }
int Peak::getRangedStrengthMod(const Piece &piece) { return -1; }

int Forest::getRangeMaxMod() { return -1; }
int Mountain::getRangeMaxMod() { return 1; }
int Jungle::getRangeMaxMod() { return -1; }
int Peak::getRangeMaxMod() { return 1; }



std::unique_ptr<Tile> makeRandomTerrain(float noise, BiomeType biome, int x, int y) {
    switch (biome) {
        case BiomeType::Temperate:
            if (noise < 0.3f) return std::unique_ptr<Tile>(std::make_unique<Water>(x, y));
            if (noise < 0.45f) return std::unique_ptr<Tile>(std::make_unique<Field>(x, y));
            if (noise < 0.7f) return std::unique_ptr<Tile>(std::make_unique<Forest>(x, y));
            return std::unique_ptr<Tile>(std::make_unique<Mountain>(x, y));
        case BiomeType::Grassy:
            if (noise < 0.3f) return std::unique_ptr<Tile>(std::make_unique<Water>(x, y));
            if (noise < 0.55f) return std::unique_ptr<Tile>(std::make_unique<Field>(x, y));
            if (noise < 0.7f) return std::unique_ptr<Tile>(std::make_unique<Forest>(x, y));
            return std::unique_ptr<Tile>(std::make_unique<Mountain>(x, y));
        case BiomeType::Arid:
            if (noise < 0.1f) return std::unique_ptr<Tile>(std::make_unique<Water>(x, y));
            if (noise < 0.45f) return std::unique_ptr<Tile>(std::make_unique<Field>(x, y));
            if (noise < 0.75f) return std::unique_ptr<Tile>(std::make_unique<Desert>(x, y));
            if (noise < 0.9f) return std::unique_ptr<Tile>(std::make_unique<Forest>(x, y));
            return std::unique_ptr<Tile>(std::make_unique<Mountain>(x, y));
        case BiomeType::Tropical:
            if (noise < 0.3f) return std::unique_ptr<Tile>(std::make_unique<Water>(x, y));
            if (noise < 0.4f) return std::unique_ptr<Tile>(std::make_unique<Field>(x, y));
            if (noise < 0.55f) return std::unique_ptr<Tile>(std::make_unique<Forest>(x, y));
            if (noise < 0.8f) return std::unique_ptr<Tile>(std::make_unique<Jungle>(x, y));
            return std::unique_ptr<Tile>(std::make_unique<Mountain>(x, y));
        case BiomeType::Alpine:
            if (noise < 0.15f) return std::unique_ptr<Tile>(std::make_unique<Water>(x, y));
            if (noise < 0.3f) return std::unique_ptr<Tile>(std::make_unique<Field>(x, y));
            if (noise < 0.5f) return std::unique_ptr<Tile>(std::make_unique<Forest>(x, y));
            if (noise < 0.75f) return std::unique_ptr<Tile>(std::make_unique<Mountain>(x, y));
            return std::unique_ptr<Tile>(std::make_unique<Peak>(x, y));
        case BiomeType::Arctic:
            if (noise < 0.2f) return std::unique_ptr<Tile>(std::make_unique<Water>(x, y));
            if (noise < 0.4f) return std::unique_ptr<Tile>(std::make_unique<IceField>(x, y));
            if (noise < 0.6f) return std::unique_ptr<Tile>(std::make_unique<SnowField>(x, y));
            if (noise < 0.8f) return std::unique_ptr<Tile>(std::make_unique<Tundra>(x, y));
            return std::unique_ptr<Tile>(std::make_unique<Mountain>(x, y));
        default: 
            return std::unique_ptr<Tile>(std::make_unique<Field>(x, y));
    }
}

std::vector<int> generateRoad(Tile * startTile, Tile * endTile, std::unordered_map<int, std::unique_ptr<Tile>> &board, int width, int height, WINDOW * window) {
    wprintw(window, "Starting A* search algorithm\n");
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
            case TerrainType::Water:    return 15;
            case TerrainType::Peak:     return 20;
            default:                    return 999; // Includes roads, water, and arctic environments
        }
    };

    // Gets all cardinally neighboring tiles by index
    auto getNeighbors = [width, height] (Tile &tile) {
        std::vector<int> neighbors;
        for (int i = 0; i < 4; i++) {
            int nx = tile.x + vectors[i].dx;
            int ny = tile.y + vectors[i].dy;
            if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                neighbors.push_back(tile.x + vectors[i].dx + width * (tile.y + vectors[i].dy));
            }
        }
        return neighbors;
    };

    // A* Search Algorithm, comments detail associated pseudocode from wikipedia (thanks wikipedia)
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
    std::unordered_set<int> checkSet; // for checking contents of openSet

    openSet.push(startTileId); // openSet := {start}
    checkSet.insert(startTileId);
    
    wprintw(window, "Algorithm initiated at (%d, %d)\n", startTile->x+1, startTile->y+1);
    wprintw(window, "Algorithm target is (%d, %d)\n", endTile->x+1, endTile->y+1);

    while (!(openSet.empty())) { // while openSet is not empty
        int current = openSet.top(); // current := the node in openSet having the lowest fScore[] value
                                 // Because the openSet comp lambda sorts by lowest f, it will always be the top of openSet
        if (current == getTileId(endTile)) { // if current = goal 
            wprintw(window, "Road path found, generating path vector.\n");
            std::vector<int> path;
            path.push_back(current); // total_path := {current}

            while (closedList.count(current) == 1) { // while current in cameFrom.Keys:
                current = closedList[current]; // current := cameFrom[current]
                path.push_back(current); // total_path.prepend(current)
            }
            return path; // return total_path  
        }        
        
        openSet.pop(); // openSet.Remove(current)
        checkSet.erase(current); // remove current from the check set
        
        std::vector<int> neighbors = getNeighbors(*board[current]);
        for (int i = 0; i < neighbors.size(); i++) { // for each neighbor of current
            int tentativeG = gScore[current] + getCost(board[neighbors[i]]->terrain); // tentative_gScore := gScore[current] + d(current, neighbor) 
            if (gScore.find(neighbors[i]) == gScore.end() || tentativeG < gScore[neighbors[i]]) { // if tentative_gScore > gScore[neighbor]
                closedList[neighbors[i]] = current; // cameFrom[neighbor] := current
                gScore[neighbors[i]] = tentativeG; // gScore[neighbor] := tentative_gScore
                fScore[neighbors[i]] = tentativeG + h(board[neighbors[i]].get()); // fScore[neighbor] := tentative_gScore + h(neighbor)
                
                if (checkSet.find(neighbors[i]) == checkSet.end()) { // if neighbor not in openSet
                    openSet.push(neighbors[i]); // openSet.add(neighbor)
                    checkSet.insert(neighbors[i]); // and same for the check set
                }
            }

        }

    }
    wprintw(window, "Algorithm failed.\n");
    std::vector<int> failure = { };
    return failure;
}

