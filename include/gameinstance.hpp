#ifndef GAMEINSTANCE_HPP
#define GAMEINSTANCE_HPP

#include <vector>

#include "boards.hpp"
#include "tiles.hpp"
#include "pieces.hpp"
#include "types.hpp"

// An instance of a game. Basically contains everything needed to run the core mechanics of the game
class GameInstance {
    public:
        // Constructor
        GameInstance(BiomeType biome, MissionType mission, int octave);

        // Public Members of GameInstance
        bool playerWin;
        BiomeType biome;
        MissionType mission;
        int octave;
        int boardWidth;
        int boardHeight;

    // Private Members of GameInstance, make private later
        Board board;
        std::vector<Piece> playerPieces;
        std::vector<Piece> enemyPieces;
        unsigned int seed;

        // When a GameInstance is selected, makeGame actually generates the board
        void makeGame(std::vector<Piece> runPieces, std::vector<Piece> enemyPieces);

        // Two methods to get tiles, one from an (x,y) coordinate, and another from a piece object.
        Tile * getTile(int x, int y);
        Tile * getPieceTile(Piece * piece);

        // Methods over pieces
        std::vector<Tile *> getValidMoves(Piece * piece);
        bool movePiece(Piece * piece, int x, int y);
        bool addPiece(Piece * piece, int x, int y);
        bool pieceExists(Piece * piece);
};



#endif
