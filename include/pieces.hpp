#ifndef PIECES_HPP
#define PIECES_HPP

#include <string>
#include "runes.hpp"
#include "types.hpp"

struct RuneSentence; // Forward declaration of Rune struct, which is in runes.hpp

struct Piece {
    std::string name = "default";
    PieceMaterial material = PieceMaterial::Wood;
    PieceType type = PieceType::Light;
    PieceCategory category = PieceCategory::Infantry;
    bool wizardRunes = false;
    RuneSentence * runeSentence = nullptr;
    bool commanderRunes = false;
    RuneSentence * commanderRuneSentence = nullptr; //The extra set of runes only available to commander pieces  

    bool ownedByPlayer = true;
    bool canMoveThroughPieces  = false;
    int strength = 0;
    int toughness = 0;
    int maxCardinal = 0;
    int maxDiagonal = 0;
    int rangedAttackDistance = 0;

};

Piece makeNewPiece(std::string name, PieceMaterial material, PieceType type, bool owner);

#endif
