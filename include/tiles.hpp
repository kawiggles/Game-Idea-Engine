#ifndef TILES_HPP
#define TILES_HPP

#include "types.hpp"

#include <string>

// Objects:
struct Piece; // Forward declaration of Piece struct, which is in pieces.hpp

struct Tile {
    int x = 0;
    int y = 0;
    TerrainType terrain = TerrainType::Field;
    Piece * occupyingPiece = nullptr; // Pointer to the piece currently on this tile, if any
    std::string symbol = "[ ]"; // Used for testing
};


// Functions:

// Function to randomly generate a terrain type for a tile, will be updated later for perlin noise
TerrainType getRandomTerrain();

// Function to get a terminal symbol for a tile object
std::string getTileSymbol(const Tile &tile);

#endif
