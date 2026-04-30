#include "logs.hpp"
#include "types.hpp"
#include "pieces.hpp"

std::string getPieceType(const Piece &piece) {
    switch(piece.type) {
        case PieceType::Light:      return  "Light Infantry";
        case PieceType::Shield:     return  "Shield Infantry";
        case PieceType::Elite:      return  "Elite Infantry";
        case PieceType::Archer:     return  "Archer";
        case PieceType::LCavalry:   return  "Light Cavalry";
        case PieceType::MCavalry:   return  "Medium Cavalry";
        case PieceType::HCavalry:   return  "Heavy Cavalry";
        case PieceType::Catapult:   return  "Trebuchet";
        case PieceType::Ballista:   return  "Ballista";
        case PieceType::Chariot:    return  "Chariot";
        case PieceType::Commander:  return  "Commander";
        case PieceType::Wizard:     return  "Wizard";
        case PieceType::Assassin:   return  "Assassin";
        case PieceType::Druid:      return  "Druid";
        default:                    return  "Unknown";
    }
}

std::string getBiomeType(const BiomeType biome) {
    switch (biome) {
        case BiomeType::Grassy:     return  "Grassland";
        case BiomeType::Temperate:  return  "Forest";
        case BiomeType::Arid:       return  "Desert";
        case BiomeType::Tropical:   return  "Jungle";
        case BiomeType::Alpine:     return  "Alpine";
        case BiomeType::Arctic:     return  "Arctic";
        default:                    return  "Unknown";
    }
}

std::string getTerrainType(const TerrainType terrain) {
    switch (terrain) {
        case TerrainType::Water:    return  "Water";
        case TerrainType::Field:    return  "Field";
        case TerrainType::Forest:   return  "Forest";
        case TerrainType::Mountain: return  "Mountain";
        case TerrainType::Road:     return  "Road";
        case TerrainType::Desert:   return  "Desert";
        case TerrainType::Jungle:   return  "Jungle";
        case TerrainType::Peak:     return  "Peak";
        case TerrainType::IceField: return  "IceField";
        case TerrainType::SnowField:return  "SnowField";
        case TerrainType::Tundra:   return  "Tundra";
        case TerrainType::Objective:return  "Objective";
        default:                    return  "Unknown";
    }
}

std::string getMoveType(const MoveType move) {
    switch (move) {
        case MoveType::Move:    return  "Move";
        case MoveType::Shoot:   return  "Shoot";
        case MoveType::Capture: return  "Capture";
        case MoveType::Any:     return  "All";
        default:                return  "Null";
    }
}

std::string getMaterialType(const PieceMaterial material) {
    switch (material) {
        case PieceMaterial::Wood:   return "Wood";
        case PieceMaterial::Stone:  return "Stone";
        case PieceMaterial::Iron:   return "Iron";
        case PieceMaterial::Gold:   return "Gold";
        case PieceMaterial::Diamond: return "Diamond";
        default:                    return "Null";
    }
}
