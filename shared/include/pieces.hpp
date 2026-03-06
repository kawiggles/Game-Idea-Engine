#ifndef PIECES_HPP
#define PIECES_HPP

#include "types.hpp"

struct RuneSentence; // Forward declaration of Rune struct, which is in runes.hpp

struct RangedAttack {
    int maxRange;
    int minRange;
    int strength;
};

struct Piece {
    // Core Attributes
    const unsigned int id;
    PieceMaterial material;
    PieceType type;
    PieceCategory category;
    
    // Stats
    bool ownedByPlayer = true;
    bool canMoveThroughPieces  = false;
    int strength = 0;
    int toughness = 0;
    int maxCardinal = 0;
    int maxDiagonal = 0;

    // Special Piece Attributes
    bool wizardRunes = false;
    RuneSentence * runeSentence = nullptr;
    bool commanderRunes = false;
    RuneSentence * commanderRuneSentence = nullptr; //The extra set of runes only available to commander pieces  
    bool druidRunes = false;

    // Constructor Functions
    static unsigned int nextId() {
        static unsigned int counter = 0;
        return counter++;
    }

    Piece(PieceMaterial m, PieceType t, bool o);
    
};


#endif
