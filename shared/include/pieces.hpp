#ifndef PIECES_HPP
#define PIECES_HPP

#include "types.hpp"

struct RuneSentence; // Forward declaration of Rune struct, which may or may not be used

// The piece struct simply contains a bunch of values which various rules call to evaluate game actions
// The piece struct is the only structure which is used by multiple "layers" of the game
class Piece {
    private:
        const unsigned int id;
        // This piece index will go up forever, ensuring that every piece has a unique identifier by which it can be identified
        static unsigned int nextId() {
            static unsigned int counter = 0;
            return counter++;
        }

    public:
        Piece(PieceMaterial m, PieceType t, bool o);

        PieceMaterial material;
        PieceType type;
        PieceCategory category;
        bool onBoard = false;
        
        // Movement Stats
        bool ownedByPlayer = true;
        bool canMoveThroughPieces  = false;
        int strength = 1;
        int toughness = 1;
        int maxCardinal = 1;
        int maxDiagonal = 1;

        // Ranged Stats
        struct RangedAttack {
            int maxRange = 0;
            int minRange = 0;
            int strength = 0;
            bool canShootThroughPieces = false;
        };
        bool hasRangedAttack = false;
        RangedAttack rangedAttack;

        // Special Piece Attributes
        bool wizardRunes = false;
        RuneSentence * runeSentence = nullptr;
        bool commanderRunes = false;
        RuneSentence * commanderRuneSentence = nullptr; //The extra set of runes only available to commander pieces  
        bool druidRunes = false;
};


#endif
