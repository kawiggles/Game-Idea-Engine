#ifndef GAMEINSTANCE_HPP
#define GAMEINSTANCE_HPP

#include <vector>

#include "tiles.hpp"
#include "pieces.hpp"
#include "types.hpp"

// See gameinstance.cpp for details on the operation of a Game Instance

// A move structure, denoting the origin tile and target tile of a move
struct Move {
    MoveType type;
    Tile * from;
    Tile * to;
};

// An instance of a game. Basically contains everything needed to run the core mechanics of the game
class GameInstance {
    public:
        // Constructor
        GameInstance(unsigned int seed, BiomeType biome, MissionType mission, int octave, bool hasRoad);

        // Public members of GameInstance,
        // Public because information will be displayed to player
        bool playerWin; // Not this one, this will do stuff in run
        BiomeType biome;
        MissionType mission; 
        bool hasRoad; 
        int octave; 
        int boardWidth; 
        int boardHeight; 

        // Private Members of GameInstance, make private later
        std::vector<Tile> board;
        std::vector<Piece *> playerPieces;
        std::vector<Piece *> enemyPieces;
        unsigned int seed;
        int turnCount;

        // When a GameInstance is selected, makeGame actually generates the board
        void makeGame(std::vector<Piece *> runPieces, std::vector<Piece *> enemyPieces);

        // The turn functions, which alternates between the player turn and enemy turn
        int takePlayerTurn(Move move);
        int takeEnemyTurn();

        // Function to check if a mission is complete
        int isMissionComplete();
        bool playerHoldsObjective;
        bool enemyHoldsObjective;

        // Two methods to get tiles, one from an (x,y) coordinate, and the other from a piece object.
        Tile * getTile(int x, int y);
        Tile * getPieceTile(Piece * piece);

        // Methods to interact with pieces
        std::vector<Move> getValidMoves(Piece * piece);
        bool movePiece(Piece * piece, Tile * target);
        bool addPiece(Piece * piece, int x, int y);
        bool pieceExists(Piece * piece);
};

#endif
