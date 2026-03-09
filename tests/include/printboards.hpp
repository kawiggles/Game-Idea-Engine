#ifndef PRINTBOARDS_HPP 
#define PRINTBOARDS_HPP 

#include <vector>
#include "tiles.hpp"
#include "gameinstance.hpp"

// Function to print board to terminal, for testing purposes
void printBoard(std::vector<Tile> &board, int width, int height);

void printValidTilesBoard(std::vector<Tile> &board, std::vector<Move> moves, int width, int height);

// Function to set up a game instance, prompting user for piece placement
void setupGame(GameInstance &game);

// Function to get user input for a game instance


#endif
