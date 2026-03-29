#include "enemy.hpp"
#include "gameinstance.hpp"
#include "tiles.hpp"
#include "logs.hpp"

#include <vector>
#include <unordered_map>
#include <memory>
#include <random>

Move enemyAlgoRandom(std::vector<Move> possibleMoves, const std::unordered_map<int, std::unique_ptr<Tile>> &board) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distrib(0, possibleMoves.size()-1);
    int randomIndex = distrib(gen);
    
    return possibleMoves[randomIndex];
}
