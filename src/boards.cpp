#include "tiles.hpp"
#include "boards.hpp"
#include "types.hpp"
#include "PerlinNoise.hpp"

#include <iostream>
#include <vector>
#include <unordered_set>

Board makeBoard(int width, int height, unsigned int seed, int octaves, BiomeType biome, MissionType mission) {
    Board board {width, height, {}};
    board.tiles.reserve(width * height); // Allocate memory space for vector equal to area of board
    
    const siv::PerlinNoise noiseMap{ seed }; // Generate perlin noise map from seed

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // (x, y) coordinates are normalized in order to account for perlin noise frequency
            float nx = (float)x / width;
            float ny = (float)y / height;
            float noiseValue = noiseMap.octave2D_01((nx * 4.0f), (ny * 4.0f), octaves); // Get noise value depending on normalized (x, y) and octave 

            board.tiles.push_back({x, y, getRandomTerrain(noiseValue, biome), nullptr});
            // (x, y), terrain type, and null pointer representing no occupying piece
        }
    }

    return board;
}



/*  This is a complicated bit of code, so I'm explaining it here. board.tiles[] is a vector array, defined in boards.hpp. The two loops iterate through this array, using board.height and board.width (also defined in boards.hpp) to know when to end and begin new rows. The tile index, when given to the array board.tiles[] will return one tile struct. That tile struct is then passed through getTileSymbol, which takes a tile struct and returns a std::string object corresponding to the properties of the tile struct. That gets printed with std::cout and the index gets incremented.*/
void printBoard(const Board &board) {
    int tileIndex = 0;
    std::cout << "    "; // Empty space at the start of coordinate numbering
    for (int i = 0; i < board.width; i++) { // Column numbering
        if (i < 10) {
            std::cout << "[0" << i << "]";
        } else {
            std::cout << "[" << i << "]";
        }
    }
    std::cout << std::endl;
    for (int i = 0; i < board.height; i++) {
        if (i < 10) {
            std::cout << "[0" << i << "]"; // Row numbering
        } else {
            std::cout << "[" << i << "]";
        }

        for (int j = 0; j < board.width; j++) {
            std::cout << "\033[49m" << getTileSymbol(board.tiles[tileIndex]) << "\033[0m";
            tileIndex++;
        }
        std::cout << std::endl;
    }
}

/* This function essentially does the exact same thing, but also adds highlights for valid moves for a selected piece. It takes a vector of those valid moves as an additional input. */
void printValidTilesBoard(Board &board, std::vector<Tile *> moves) {
    std::unordered_set<Tile *> moveSet(moves.begin(), moves.end()); // This needs to be an unordered set because of the variable size of possible valid moves
    
    int tileIndex = 0;
    std::cout << "   ";
    for (int i = 0; i < board.width; i++) {
        std::cout << "[" << i << "]";
    }
    std::cout << std::endl;

    for (int i = 0; i < board.height; i++) {
        std::cout << "[" << i << "]"; 
        for (int j = 0; j < board.width; j++) {
            
            (moveSet.count(&board.tiles[tileIndex])) ? std::cout << "\033[47m" : std::cout << "\033[49m"; // If the current tile is in the set of possible moves, 47m background, else 49m background
            std::cout << getTileSymbol(board.tiles[tileIndex]);
            std::cout << "\033[0m";

            tileIndex++;
        }
        std::cout << std::endl;
    }
}
