#ifndef PIECES_HPP
#define PIECES_HPP

#include <string>
#include "runes.hpp"

// All the different types of pieces, defines movement and base stats for a piece
enum class PieceType {
    Light,
    Shield,
    Elite,
    Archer,
    LCavalry,
    MCavalry,
    HCavalry,
    Commander,
    Wizard
};

enum class PieceCategory {
    Infantry, //Light, Shield, Elite, Archer
    Cavalry, // Light, Medium, and Heavy Cavalry
    Special // Commander and Wizard
};

// The materials each piece can be made of, works as a class for the rune logic system
enum class PieceMaterial {
    Wood,
    Iron,
    Silver,
    Gold,
    Diamond
};

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