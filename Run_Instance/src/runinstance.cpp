#include "runinstance.hpp"
#include "maps.hpp"
#include "types.hpp"
#include "PerlinNoise.hpp"

#include <random>
#include <iostream>

RunInstance::RunInstance(Map oldMap, LeaderType leader) {
    std::random_device rd;
    seed = rd();

    this->leader = leader;
    map = alterMap(oldMap);
    runPieces = getInitialPieces();
    
}

std::vector<Piece *> RunInstance::getInitialPieces() {
    switch (leader) {
        case LeaderType::Scout:
            Piece(PieceMaterial::Wood, PieceType::Light, true);
            Piece(PieceMaterial::Wood, PieceType::Light, true);
            Piece(PieceMaterial::Wood, PieceType::Archer, true);
            Piece(PieceMaterial::Stone, PieceType::LCavalry, true);
            Piece(PieceMaterial::Stone, PieceType::LCavalry, true);
            break;
        default:
            std::cout << "Error, initial pieces not generated." << std::endl;
    }
}

