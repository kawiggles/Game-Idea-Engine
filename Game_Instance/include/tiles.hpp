#ifndef TILES_HPP
#define TILES_HPP

#include "types.hpp"

#include <string>
#include <vector>
#include <ncurses.h>

// See tiles.cpp for how tiles are constructed on a board for a Game Instance

// Structures
struct Piece; // Forward declaration of Piece struct, which is in pieces.hpp

struct Tile {
    int x = 0;
    int y = 0;
    TerrainType terrain = TerrainType::Field;
    Piece * occupyingPiece = nullptr; // Pointer to the piece currently on this tile, if any
};


// Functions:

// Function to randomly generate a terrain type for a tile, will be updated later for perlin noise
TerrainType getRandomTerrain(float noise, BiomeType biome);

// Function to generate a road on a board.
std::vector<Tile *> generateRoad(Tile * startTile, Tile * endTile, std::vector<Tile> &board, int width, int height, WINDOW * window);

// Function to get a terminal symbol for a tile object, used by the printboard functions
std::string getTileSymbol(const Tile &tile);

#endif
