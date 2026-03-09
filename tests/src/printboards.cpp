#include "tiles.hpp"
#include "gameinstance.hpp"
#include "printboards.hpp"

#include <iostream>
#include <unordered_set>

/*  This is a complicated bit of code, so I'm explaining it here. board.tiles[] is a vector array, defined in boards.hpp. The two loops iterate through this array, using height and width (also defined in boards.hpp) to know when to end and begin new rows. The tile index, when given to the array board.tiles[] will return one tile struct. That tile struct is then passed through getTileSymbol, which takes a tile struct and returns a std::string object corresponding to the properties of the tile struct. That gets printed with std::cout and the index gets incremented.*/
void printBoard(std::vector<Tile> &board, int width, int height) {
    int tileIndex = 0;
    std::cout << "    "; // Empty space at the start of coordinate numbering
    for (int i = 0; i < width; i++) { // Column numbering
        if (i < 9) {
            std::cout << "[ " << i+1 << "]";
        } else {
            std::cout << "[" << i+1 << "]";
        }
    }
    std::cout << std::endl;
    for (int i = 0; i < height; i++) {
        if (i < 9) {
            std::cout << "[ " << i+1 << "]"; // Row numbering
        } else {
            std::cout << "[" << i+1 << "]";
        }

        for (int j = 0; j < width; j++) {
            std::cout << "\033[49m" << getTileSymbol(board[tileIndex]) << "\033[0m";
            tileIndex++;
        }
        std::cout << std::endl;
    }
}

/* This function essentially does the exact same thing, but also adds highlights for valid moves for a selected piece. It takes a vector of those valid moves as an additional input. */
void printValidTilesBoard(std::vector<Tile> &board, std::vector<Move> moves, int width, int height) {
    std::unordered_set<Tile *> moveSet(moves.size()); 
    
    for (int i = 0; i < moves.size(); i++) moveSet.insert(moves[i].to);
    
    int tileIndex = 0;
    std::cout << "    ";
    for (int i = 0; i < width; i++) { // Column numbering
        if (i < 9) {
            std::cout << "[ " << i+1 << "]";
        } else {
            std::cout << "[" << i+1 << "]";
        }
    }
    std::cout << std::endl;
    for (int i = 0; i < height; i++) {
        if (i < 9) {
            std::cout << "[ " << i+1 << "]"; // Row numbering
        } else {
            std::cout << "[" << i+1 << "]";
        }

        for (int j = 0; j < width; j++) {
            
            (moveSet.count(&board[tileIndex])) ? std::cout << "\033[47m" : std::cout << "\033[49m"; // If the current tile is in the set of possible moves, 47m background, else 49m background
            std::cout << getTileSymbol(board[tileIndex]);
            std::cout << "\033[0m";

            tileIndex++;
        }
        std::cout << std::endl;
    }
}
