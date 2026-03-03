#include "tiles.hpp"
#include "pieces.hpp"
#include "types.hpp"

#include <string>

TerrainType getRandomTerrain(float noise, BiomeType biome) {
    switch (biome) {
        case BiomeType::Temperate:
            if (noise < 0.2f) return TerrainType::Water;
            if (noise < 0.45f) return TerrainType::Field;
            if (noise < 0.8f) return TerrainType::Forest;
            return TerrainType::Mountain;
        case BiomeType::Grassy:
            if (noise < 0.2f) return TerrainType::Water;
            if (noise < 0.55f) return TerrainType::Field;
            if (noise < 0.8f) return TerrainType::Forest;
            return TerrainType::Mountain;
        case BiomeType::Arid:
            if (noise < 0.1f) return TerrainType::Water;
            if (noise < 0.4f) return TerrainType::Field;
            if (noise < 0.7f) return TerrainType::Desert;
            if (noise < 0.8f) return TerrainType::Forest;
            return TerrainType::Mountain;
        case BiomeType::Tropical:
            if (noise < 0.2f) return TerrainType::Water;
            if (noise < 0.3f) return TerrainType::Field;
            if (noise < 0.55f) return TerrainType::Forest;
            if (noise < 0.9f) return TerrainType::Jungle;
            return TerrainType::Mountain;
        case BiomeType::Alpine:
            if (noise < 0.1f) return TerrainType::Water;
            if (noise < 0.3f) return TerrainType::Field;
            if (noise < 0.6f) return TerrainType::Forest;
            if (noise < 0.8f) return TerrainType::Mountain;
            return TerrainType::Peak;
        case BiomeType::Arctic:
            if (noise < 0.2f) return TerrainType::Water;
            if (noise < 0.4f) return TerrainType::IceField;
            if (noise < 0.6f) return TerrainType::SnowField;
            if (noise < 0.8f) return TerrainType::Tundra;
            return TerrainType::Mountain;
        default: 
            return TerrainType::Field;
    }
}

std::string getTileSymbol(const Tile &tile) {
    std::string symbol = " ";  // Default: empty space
    std::string pieceColor = ""; // Default: no color
    
    if (tile.terrain == TerrainType::Water) return "\033[34m[~]\033[0m"; // Water tiles can never hold pieces

    if (tile.occupyingPiece) {
        // Determine piece letter
        switch (tile.occupyingPiece->type) {
            case PieceType::Light:     symbol = "P"; break;
            case PieceType::Shield:    symbol = "S"; break;
            case PieceType::Elite:     symbol = "E"; break;
            case PieceType::Archer:    symbol = "A"; break;
            case PieceType::LCavalry:  symbol = "L"; break;
            case PieceType::MCavalry:  symbol = "M"; break;
            case PieceType::HCavalry:  symbol = "H"; break;
            case PieceType::Commander: symbol = "C"; break;
            case PieceType::Wizard:    symbol = "W"; break;
        }
        
        // Color enemy pieces red
        if (!tile.occupyingPiece->ownedByPlayer) {
            pieceColor = "\033[31m";  // Red
        }
    }
    
    // Get terrain bracket color
    std::string bracketColor;
    switch (tile.terrain) {
        case TerrainType::Field:    bracketColor = "\033[33m"; break;  // Yellow
        case TerrainType::Forest:   bracketColor = "\033[32m"; break;  // Green
        case TerrainType::Mountain: bracketColor = "\033[35m"; break;  // Purple
        case TerrainType::Desert:   bracketColor = "\033[93m"; break;  // HI Yellow
        case TerrainType::Jungle:   bracketColor = "\033[92m"; break;  // HI Green
        case TerrainType::Peak:     bracketColor = "\033[30m"; break;  // Black
        // Arctic Tiles
        case TerrainType::IceField: bracketColor = "\033[36m"; break;  // Cyan
        case TerrainType::SnowField:bracketColor = "\033[37m"; break;  // White
        case TerrainType::Tundra:   bracketColor = "\033[32m"; break;  // Green again, but it's fine because it replaces forests
    }
    
    // Construct final symbol
    return bracketColor + "[" + "\033[39m" + 
           pieceColor + symbol + "\033[39m" + 
           bracketColor + "]" + "\033[39m";
}
