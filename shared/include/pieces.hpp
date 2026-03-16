#ifndef PIECES_HPP
#define PIECES_HPP

#include "types.hpp"

struct RuneSentence; // Forward declaration of Rune struct, which may or may not be used 

struct RangedAttack {
    int maxRange;
    int minRange;
    int strength;
    bool canShootThroughPieces;
};

// The piece struct simply contains a bunch of values which various rules call to evaluate game actions
// The piece struct is the only structure which is used by multiple "layers" of the game
struct Piece {
    // Core Attributes
    unsigned int id;
    PieceMaterial material;
    PieceType type;
    PieceCategory category;
    
    // Stats
    bool onBoard = false;
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
    // This piece index will go up forever, ensuring that every piece has a unique identifier by which it can be identified
    static unsigned int nextId() {
        static unsigned int counter = 0;
        return counter++;
    }

    Piece(PieceMaterial m, PieceType t, bool o);
    
};


#endif
