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

// Function to print board to terminal, for testing purposes
void printBoard(std::vector<Tile> &board, int width, int height);

void printValidTilesBoard(std::vector<Tile> &board, std::vector<Move> moves, int width, int height);

// Gets a piece's type as a string
std::string getPieceType(const Piece * piece);

// Gets a biome's type as a string
std::string getBiomeType(const BiomeType biome);

// Gets the symbol for a tile as a string depending on tile terrain and occupying piece
std::string getTileSymbol(const Tile &tile);

// Function to set up a game instance, prompting user for piece placement
void setupGame(GameInstance &game);

// Function to get user input for a game instance
void runGame(GameInstance &game, bool startingPlayer);

#endif
