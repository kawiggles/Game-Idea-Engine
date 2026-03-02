#include "tiles.hpp"
#include "pieces.hpp"
#include "PerlinNoise.hpp"

#include <cstdlib>
#include <iostream>
#include <string>

TerrainType getRandomTerrain() {
    int randValue = rand() % 10; // Currently 10 because only 4 terrain type, but weights, will need to be updated as more are added
    switch (randValue) {
        case 0: case 1: case 2: case 3: 
            return TerrainType::Field;
        case 4: case 5: 
            return TerrainType::Forest;
        case 6: case 7: 
            return TerrainType::Swamp;
        case 8: case 9:  
            return TerrainType::Mountain;
        default: 
            return TerrainType::Field;
    }
}

std::string getTileSymbol(const Tile &tile) {
    std::string symbol = " ";  // Default: empty space
    std::string pieceColor = "";
    
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
        case TerrainType::Swamp:    bracketColor = "\033[90m"; break;  // Dark gray
        case TerrainType::Forest:   bracketColor = "\033[32m"; break;  // Green
        case TerrainType::Mountain: bracketColor = "\033[37m"; break;  // White
    }
    
    // Construct final symbol
    return bracketColor + "[" + "\033[39m" + 
           pieceColor + symbol + "\033[39m" + 
           bracketColor + "]" + "\033[39m";
}
