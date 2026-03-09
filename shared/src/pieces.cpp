#include "pieces.hpp"
#include "types.hpp"

#include <vector>

// This ugly piece of code is used to define the basic values for a piece struct upon it's initialization.
// This is all it does. Is there a more efficient way to do this? Probably.
// Do I care? No.
// Also this makes it easy for me to adjust piece stats for balancing
Piece::Piece(PieceMaterial m, PieceType t, bool o)
    : id(nextId()), material(m), type(t), ownedByPlayer(o) {

    switch(type) {
        case PieceType::Light:
            category = PieceCategory::Infantry;
            canMoveThroughPieces = true;
            strength = 1;
            toughness = 1;
            maxCardinal = 2;
            maxDiagonal = 2;
            break;
        case PieceType::Shield:
            category = PieceCategory::Infantry;
            strength = 1;
            toughness = 3;
            maxCardinal = 1;
            maxDiagonal = 0;
            break;
        case PieceType::Elite:
            category = PieceCategory::Infantry;
            strength = 2;
            toughness = 2;
            maxCardinal = 1;
            maxDiagonal = 1;
            break;
        case PieceType::Archer:
            category = PieceCategory::Infantry;
            strength = 1;
            toughness = 1;
            maxCardinal = 2;
            maxDiagonal = 2;
            break;
        case PieceType::LCavalry:
            category = PieceCategory::Cavalry;
            canMoveThroughPieces = true;
            strength = 1;
            toughness = 1;
            maxCardinal = 1;
            maxDiagonal = 6;
            break;
        case PieceType::MCavalry:
            category = PieceCategory::Cavalry;
            strength = 2;
            toughness = 1;
            maxCardinal = 5; 
            maxDiagonal = 5;
            break;
        case PieceType::HCavalry:
            category = PieceCategory::Cavalry;
            strength = 3;
            toughness = 2;
            maxCardinal = 4; 
            maxDiagonal = 0;
            break;
        case PieceType::Catapult:
            category = PieceCategory::Siege;
            strength = 0;
            toughness = 1;
            maxCardinal = 0;
            maxDiagonal = 0;
            break;
        case PieceType::Ballista:
            category = PieceCategory::Siege;
            strength = 0;
            toughness = 1;
            maxCardinal = 1;
            maxDiagonal = 1;
            break;
        case PieceType::Chariot:
            category = PieceCategory::Siege;
            strength = 4;
            toughness = 3;
            maxCardinal = 5;
            maxDiagonal = 0;
            break;
        case PieceType::Commander:
            category = PieceCategory::Special;
            strength = 1;
            toughness = 2;
            maxCardinal = 1; 
            maxDiagonal = 1;
            commanderRunes = true;
            break;
        case PieceType::Wizard:
            category = PieceCategory::Special;
            strength = 1;
            toughness = 1;
            maxCardinal = 2; 
            maxDiagonal = 2;
            wizardRunes = true;
            break;
        case PieceType::Assassin:
            category = PieceCategory::Special;
            strength = 5;
            toughness = 1;
            maxCardinal = 3;
            maxDiagonal = 3;
            break;
        case PieceType::Druid:
            category = PieceCategory::Special;
            strength = 1;
            toughness = 1;
            maxCardinal = 2;
            maxDiagonal = 2;
            druidRunes = true;
            break;
    } 

}

