#include "pieces.hpp"
#include "tiles.hpp"
#include "boards.hpp"

#include <iostream>
#include <vector>
#include <unordered_set>

Board makeBoard(int width, int height) {
    Board board {width, height, {}};
    board.tiles.reserve(width * height);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            board.tiles.push_back({x, y, getRandomTerrain(), nullptr});
        }
    }

    return board;
}



/*  This is a complicated bit of code, so I'm explaining it here. board.tiles[] is a vector array, defined in boards.hpp.
    The two loops iterate through this array, using board.height and board.width (also defined in boards.hpp) to know when to
    end and begin new rows. The tile index, when given to the array board.tiles[] will return one tile struct. That tile
    struct is then passed through getTileSymbol, which takes a tile struct and returns a std::string object corresponding
    to the properties of the tile struct. That gets printed with std::cout and the index gets incremented.*/
void printBoard(const Board &board) {
    int tileIndex = 0;
    std::cout << "   ";
    for (int i = 0; i < board.width; i++) {
        std::cout << "[" << i << "]";
    }
    std::cout << std::endl;
    for (int i = 0; i < board.height; i++) {
        std::cout << "[" << i << "]"; 
        for (int j = 0; j < board.width; j++) {
            std::cout << getTileSymbol(board.tiles[tileIndex]);
            tileIndex++;
        }
        std::cout << std::endl;
    }
}

void printValidTilesBoard (Board &board, std::vector<Tile *> moves) {
    std::unordered_set<Tile *> moveSet(moves.begin(), moves.end());
    
    int tileIndex = 0;
    std::cout << "   ";
    for (int i = 0; i < board.width; i++) {
        std::cout << "[" << i << "]";
    }
    std::cout << std::endl;

    for (int i = 0; i < board.height; i++) {
        std::cout << "[" << i << "]"; 
        for (int j = 0; j < board.width; j++) {
            
            if (moveSet.count(&board.tiles[tileIndex])) std::cout << "\x1b[4m";
            std::cout << getTileSymbol(board.tiles[tileIndex]);
            if (moveSet.count(&board.tiles[tileIndex])) std::cout << "\x1b[0m";

            tileIndex++;
        }
        std::cout << std::endl;
    }
}