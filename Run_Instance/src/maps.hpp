#include "maps.hpp"
#include "runinstance.hpp"
#include "PerlinNoise.hpp"

Map makeMap(int seed) {
    const siv::PerlinNoise noise { seed };
    Map map;

    return map;
}
