#ifndef RUNINSTANCE_HPP 
#define RUNINSTANCE_HPP

#include "pieces.hpp"
#include "maps.hpp"
#include <vector>

class RunInstance {
    public:
        // Constructor
        RunInstance(Map oldMap, Leader leader);

        // Public Members
        Leader leader;
        Map map;
        std::vector<Piece> runPieces;

        // Public Methods
        std::vector<Piece> getInitialPieces(Leader leader);
};

#endif
