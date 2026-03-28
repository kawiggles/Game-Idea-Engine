#pragma once

#include <vector>
#include <unordered_map>
#include <memory>

// Forward Declarations
struct Move;
struct Tile;

int enemySetupAlgoRandom(const std::unordered_map<int, std::unique_ptr<Tile>> &board);
Move enemyAlgoRandom(std::vector<Move> possibleMoves, const std::unordered_map<int, std::unique_ptr<Tile>> &board);
