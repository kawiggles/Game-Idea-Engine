#include "runinstance.hpp"
#include "maps.hpp"
#include "PerlinNoise.hpp"

#include <random>

RunInstance::RunInstance(Map oldMap, Leader leader) {
    std::random_device rd;
    seed = rd();

    this->leader = leader;
    map = alterMap(oldMap);
    runPieces = getInitialPieces();

    
}
