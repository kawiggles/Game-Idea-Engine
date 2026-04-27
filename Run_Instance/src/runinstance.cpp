#include "runinstance.hpp"
#include "maps.hpp"
#include "types.hpp"

#include <random>
#include <iostream>

RunInstance::RunInstance(Map oldMap, LeaderType leader) {
    std::random_device rd;
    seed = rd();

    this->leader = leader;
    pieceInventory = getInitialPieces();
    map = alterMap(oldMap);
    
}

std::unordered_map<int, std::unique_ptr<Piece>> RunInstance::getInitialPieces() {
    std::unordered_map<int, std::unique_ptr<Piece>> newInventory;
    switch (leader) {
        case LeaderType::Scout:
            newInventory[0] = std::make_unique<Piece>(PieceMaterial::Wood, PieceType::Light, Player::Human);
            newInventory[1] = std::make_unique<Piece>(PieceMaterial::Wood, PieceType::Light, Player::Human);
            newInventory[2] = std::make_unique<Piece>(PieceMaterial::Wood, PieceType::Archer, Player::Human);
            newInventory[3] = std::make_unique<Piece>(PieceMaterial::Stone, PieceType::LCavalry, Player::Human);
            newInventory[4] = std::make_unique<Piece>(PieceMaterial::Stone, PieceType::LCavalry, Player::Human);
            break;
        default:
            std::cout << "Error, initial pieces not generated." << std::endl;
            return newInventory;
    }

    return newInventory;
}

bool RunInstance::checkFullInventory() {
    if (leader == LeaderType::MobLeader) {
        if (pieceInventory.size() >= 30) return false;
        else return true;
    } else {
        if (pieceInventory.size() >= 20) return false;
        else return true;
    }
}
