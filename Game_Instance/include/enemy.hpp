#pragma once

#include <unordered_set>
#include <unordered_map>
#include <memory>

// Forward Declarations
struct Move;
struct MoveHash;
struct Tile;

int enemySetupAlgoRandom(const std::unordered_map<int, std::unique_ptr<Tile>> &board);
Move enemyAlgoBasic(std::unordered_set<Move, MoveHash> possibleMoves, const std::unordered_map<int, std::unique_ptr<Tile>> &board);
