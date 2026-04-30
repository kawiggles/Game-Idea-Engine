#include "enemy.hpp"
#include "gameinstance.hpp"
#include "tiles.hpp"
#include "logs.hpp"

#include <unordered_set>
#include <unordered_map>
#include <memory>

Move enemyAlgoBasic(std::unordered_set<Move, MoveHash> possibleMoves, const std::unordered_map<int, std::unique_ptr<Tile>> &board) {
    log("Running basic enemy algorithm");
    Move bestMove;
    int bestMoveScore = 0;
    for (Move m : possibleMoves) {
        int moveScore = 0;
        if (m.type == MoveType::Capture) moveScore += 1000;
        if (m.to->terrain == TerrainType::Objective) moveScore += 1000;
        if (m.to->occupyingPiece) moveScore += 100;
        if (m.to->terrain == TerrainType::Road) moveScore += 20;
        moveScore += (abs(m.to->x) - abs(m.from->y)) + (abs(m.to->y) - abs(m.from->y));
        
        if (moveScore > bestMoveScore) {
            bestMove = m;
            bestMoveScore = moveScore;
        }
    }
    
    log("Best move found: Movetype %s from (%d, %d) to (%d, %d)", getMoveType(bestMove.type).c_str(),
            bestMove.from->x+1, bestMove.from->y+1, bestMove.to->x+1, bestMove.to->y+1);
    return bestMove;
}
