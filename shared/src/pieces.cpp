#include "pieces.hpp"
#include <vector>

Piece makeNewPiece(std::string name, PieceMaterial material, PieceType type, bool owner) {
    Piece newPiece{name, material, type};
    newPiece.ownedByPlayer = owner;

    switch(type) {
        case PieceType::Light:
            newPiece.category = PieceCategory::Infantry;
            newPiece.canMoveThroughPieces = true;
            newPiece.strength = 1;
            newPiece.toughness = 1;
            newPiece.maxCardinal = 2;
            newPiece.maxDiagonal = 2;
            break;
        case PieceType::Shield:
            newPiece.category = PieceCategory::Infantry;
            newPiece.strength = 1;
            newPiece.toughness = 3;
            newPiece.maxCardinal = 1;
            newPiece.maxDiagonal = 0;
            break;
        case PieceType::Elite:
            newPiece.category = PieceCategory::Infantry;
            newPiece.strength = 2;
            newPiece.toughness = 2;
            newPiece.maxCardinal = 1;
            newPiece.maxDiagonal = 1;
            break;
        case PieceType::Archer:
            newPiece.category = PieceCategory::Infantry;
            newPiece.strength = 1;
            newPiece.toughness = 1;
            newPiece.maxCardinal = 2;
            newPiece.maxDiagonal = 2;
            newPiece.rangedAttackDistance = 2;
            break;
        case PieceType::LCavalry:
            newPiece.category = PieceCategory::Cavalry;
            newPiece.canMoveThroughPieces = true;
            newPiece.strength = 1;
            newPiece.toughness = 1;
            newPiece.maxCardinal = 1;
            newPiece.maxDiagonal = 6;
            newPiece.rangedAttackDistance = 2;
            break;
        case PieceType::MCavalry:
            newPiece.category = PieceCategory::Cavalry;
            newPiece.strength = 2;
            newPiece.toughness = 1;
            newPiece.maxCardinal = 5; // Effectively Infinite
            newPiece.maxDiagonal = 5;
            break;
        case PieceType::HCavalry:
            newPiece.category = PieceCategory::Cavalry;
            newPiece.strength = 3;
            newPiece.toughness = 2;
            newPiece.maxCardinal = 4; 
            newPiece.maxDiagonal = 0;
            break;
        case PieceType::Commander:
            newPiece.category = PieceCategory::Special;
            newPiece.strength = 1;
            newPiece.toughness = 2;
            newPiece.maxCardinal = 1; 
            newPiece.maxDiagonal = 1;
            newPiece.commanderRunes = true;
            break;
        case PieceType::Wizard:
            newPiece.category = PieceCategory::Special;
            newPiece.strength = 1;
            newPiece.toughness = 1;
            newPiece.maxCardinal = 2; 
            newPiece.maxDiagonal = 2;
            newPiece.wizardRunes = true;
            break;
    }

    return newPiece;

}

