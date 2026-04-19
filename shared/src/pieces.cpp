#include "pieces.hpp"
#include "types.hpp"

/*
 * This ugly piece of code is used to define the basic values for a piece struct upon it's initialization.
 * This is all it does. Is there a more efficient way to do this? Probably.
 * Do I care? No.
 * Also this makes it easy for me to adjust piece stats for balancing, so ha.
 */

Piece::Piece(PieceMaterial m, PieceType t, Player o)
    : id(nextId()), material(m), type(t), owner(o) {

    switch(type) {
        case PieceType::Light:
            category = PieceCategory::Infantry;
            canMoveThroughPieces = true;
            maxCardinal = 2;
            maxDiagonal = 2;
            hasRangedAttack = true;
            rangedAttack.strength = 1;
            rangedAttack.maxRange = 2;
            break;
        case PieceType::Shield:
            category = PieceCategory::Infantry;
            toughness = 3;
            maxDiagonal = 0;
            break;
        case PieceType::Elite:
            category = PieceCategory::Infantry;
            strength = 2;
            toughness = 2;
            break;
        case PieceType::Archer:
            category = PieceCategory::Infantry;
            strength = 0;
            maxCardinal = 2;
            maxDiagonal = 2;
            hasRangedAttack = true;
            rangedAttack.strength = 2;
            rangedAttack.maxRange = 3;
            rangedAttack.canShootThroughPieces = true;
            break;
        case PieceType::LCavalry:
            category = PieceCategory::Cavalry;
            canMoveThroughPieces = true;
            maxDiagonal = 6;
            hasRangedAttack = true;
            rangedAttack.strength = 1;
            rangedAttack.maxRange = 2;
            break;
        case PieceType::MCavalry:
            category = PieceCategory::Cavalry;
            strength = 2;
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
            maxCardinal = 0;
            maxDiagonal = 0;
            hasRangedAttack = true;
            rangedAttack.strength = 5;
            rangedAttack.maxRange = 7;
            rangedAttack.minRange = 4;
            rangedAttack.canShootThroughPieces = true;
            break;
        case PieceType::Ballista:
            category = PieceCategory::Siege;
            strength = 0;
            hasRangedAttack = true;
            rangedAttack.strength = 3;
            rangedAttack.minRange = 2;
            rangedAttack.maxRange = 5;
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
            toughness = 2;
            commanderRunes = true;
            break;
        case PieceType::Wizard:
            category = PieceCategory::Special;
            maxCardinal = 2; 
            maxDiagonal = 2;
            wizardRunes = true;
            break;
        case PieceType::Assassin:
            category = PieceCategory::Special;
            strength = 6;
            maxCardinal = 3;
            maxDiagonal = 3;
            break;
        case PieceType::Druid:
            category = PieceCategory::Special;
            maxCardinal = 2;
            maxDiagonal = 2;
            druidRunes = true;
            break;
    } 

}

