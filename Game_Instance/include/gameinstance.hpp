#ifndef GAMEINSTANCE_HPP
#define GAMEINSTANCE_HPP

#include <memory>
#include <vector>
#include <ncurses.h>

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
        GameInstance(unsigned long seed, BiomeType biome, MissionType mission, int octave, bool hasRoad);

        // Public Members of GameInstance, information available to players before initialization 
        bool playerWin; // Not this one, this will do stuff in run 
        BiomeType biome;
        MissionType mission; 
        bool hasRoad; 
        int octave; 
        int boardWidth; 
        int boardHeight; 

        // Private Members of GameInstance, make private later
        std::vector<Tile> board;
        std::vector<std::unique_ptr<Piece>> playerPieces;
        std::vector<std::unique_ptr<Piece>> enemyPieces;
        unsigned long seed;
        int turnCount;

        // When a GameInstance is selected, makeGame actually generates the board
        void makeGame(std::vector<std::unique_ptr<Piece>>&& runPieces, std::vector<std::unique_ptr<Piece>>&& enemyPieces, WINDOW * window);

        // The turn functions, which alternates between the player turn and enemy turn
        int takePlayerTurn(Move move);
        int setupEnemy();
        int takeEnemyTurn();

        // Function to check if a mission is complete
        int isMissionComplete();
        bool playerHoldsObjective;
        bool enemyHoldsObjective;

        // Two methods to get tiles, one from an (x,y) coordinate, and the other from a piece object.
        Tile * getTile(int x, int y);
        int getTileId(const Tile &tile);
        Tile * getPieceTile(const Piece &piece);

        // Methods to interact with pieces
        int getToughnessMod(Tile * tile);
        int getMovementMod(Tile * tile, const Piece &piece, int i);
        std::vector<Move> getValidMovement(const Piece &piece, Tile * currentTile, int relativeStrengthMod);
        std::vector<Move> getValidRangedAttacks(const Piece &piece, Tile * currentTile, int relativeRangedStrenghtMod, int relativeRangeMax);
        std::vector<Move> getValidMoves(const Piece &piece, MoveType type);
        int movePiece(Piece * piece, Tile * target);
        int shootPiece(Piece * piece, Tile * targe);
        int addPiece(Piece * piece, int tileIndex);
        bool pieceExists(Piece * piece);
};

#endif
