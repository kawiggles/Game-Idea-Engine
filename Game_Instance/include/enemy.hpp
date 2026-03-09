#ifndef ENEMY_HPP
#define ENEMY_HPP

#include <vector>

// Forward Declarations
struct Move;
struct Tile;

Move enemyAlgoBasic(std::vector<Move> possibleMoves, const std::vector<Tile> &board);

#endif
