#ifndef ENEMY_HPP
#define ENEMY_HPP

#include <vector>

// Forward Declarations
struct Move;
struct Tile;

int enemySetupAlgoRandom(const std::vector<Tile> &board);
Move enemyAlgoRandom(std::vector<Move> possibleMoves, const std::vector<Tile> &board);

#endif
