#pragma once

#include <vector>
#include "types.hpp"
#include "maps.hpp"
#include "gameinstance.hpp"

// Forward Declarations

struct Piece;

class RunInstance {
    public:
        // Constructor
        RunInstance(Map oldMap, LeaderType leader);

        // Public Members
        int seed;
        LeaderType leader;
        Map map;
        std::unordered_map<int, std::unique_ptr<Piece>> pieceInventory;

        // Public Methods
        std::vector<GameInstance> generateGames();
        std::unordered_map<int, std::unique_ptr<Piece>>  getInitialPieces();

        bool checkFullInventory();
};
