#ifndef BOARDS_HPP
#define BOARDS_HPP

#include <vector>
#include "instancetypes.hpp"

// Forward declarations
struct Tile; 
struct Piece;

struct Board {
    int width;
    int height;
    std::vector<Tile> tiles;
};

// Fuctions here:

// Function that generates a board of dimensions (width, height) by creating a tile for each increment of x and y
Board makeBoard(int width, int height, unsigned int seed, int octaves, BiomeType biome, MissionType mission);

// Function to find tiles on a board using it's (x, y) coordinates, very useful
Tile * getTile(Board &board, int xCoord, int yCoord);

// Function to print board to terminal, for testing purposes
void printBoard(const Board &board);
void printValidTilesBoard(Board &board, std::vector<Tile *> moves);

#endif
