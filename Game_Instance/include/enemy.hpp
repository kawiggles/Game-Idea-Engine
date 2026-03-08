#ifndef ENEMY_HPP
#define ENEMY_HPP

#include <vector>

struct Tile;
struct Board;

Tile * enemyAlgo1(std::vector<Tile *> possibleMoves, const Board &board);

#endif
