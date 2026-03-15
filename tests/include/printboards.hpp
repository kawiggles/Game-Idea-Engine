#ifndef PRINTBOARDS_HPP 
#define PRINTBOARDS_HPP 

#include <vector>
#include <ncurses.h>
#include "tiles.hpp"
#include "gameinstance.hpp"
#include "types.hpp"

// Functions to control ncurses windows
WINDOW * createNewWindow(int height, int width, int starty, int startx);

void destroyWindow(WINDOW * window);

void initColors();

// Function to print board to terminal, for testing purposes
void printBoard(const std::vector<Tile> &board, int width, int height, WINDOW * window, int cursorX, int cursorY);

void printValidTilesBoard(std::vector<Tile> &board, std::vector<Move> moves, int width, int height, WINDOW * window, int cursorX, int cursorY);

struct Symbol {
    int terrainColor = 0;
    int pieceColor = 0;
    char pieceSymbol = ' ';
};

// Gets the symbol and colors for a tile as a the above structure
Symbol getSymbol(const Tile &tile);

Symbol getValidTileSymbol(const Tile &tile);

// Gets a piece's type as a string
std::string getPieceType(const Piece * piece);

// Gets a biome's type as a string
std::string getBiomeType(const BiomeType biome);

// Function to set up a game instance, prompting user for piece placement
void setupGame(GameInstance &game, WINDOW * terminalWindow);

// Function to get user input for a game instance
void runGame(GameInstance &game, bool startingPlayer, WINDOW * terminalWindow);

#endif
