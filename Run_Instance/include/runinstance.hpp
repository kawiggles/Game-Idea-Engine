#ifndef RUNINSTANCE_HPP 
#define RUNINSTANCE_HPP

#include <vector>
#include "types.hpp"
#include "maps.hpp"
#include "gameinstance.hpp"

// Forward Declarations

struct Piece;

class RunInstance {
    public:
        // Constructor
        RunInstance(Map oldMap, Leader leader);

        // Public Members
        int seed;
        Leader leader;
        Map map;
        std::vector<Piece> runPieces;
        std::vector<GameInstance> gameInstances;

        // Public Methods
        std::vector<GameInstance> generateGames();
        std::vector<Piece> getInitialPieces();
};

#endif
