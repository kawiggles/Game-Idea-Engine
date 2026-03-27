#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
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
        BiomeType biome;
        MissionType mission; 
        bool hasRoad; 
        int octave; 
        int boardWidth; 
        int boardHeight; 
        
        // Public method to set up game
        void makeGame(std::vector<std::unique_ptr<Piece>>&& runPieces, std::vector<std::unique_ptr<Piece>>&& enemyPieces, WINDOW * window);
        std::vector<std::unique_ptr<Piece>> playerPieces;
        int setupEnemy();
        int addPiece(Piece * piece, int tileIndex);

        // Public members and methods to run game
        std::unordered_map<int, std::unique_ptr<Tile>> board;
        int turnCount;
        int status; // <0: redo move
                    // 1: go to next turn
                    // 2: player wins
                    // 3: enemy wins
                    // 4: quit
        int takePlayerTurn(MoveType moveType, Piece * piece, int coord);
        int takeEnemyTurn();
        std::vector<Move> getValidMoves(const Piece &piece, MoveType type);

    private:
        std::vector<std::unique_ptr<Piece>> enemyPieces;
        unsigned long seed;

        int isMissionComplete();
        bool playerHoldsObjective;
        bool enemyHoldsObjective;

        // Two methods to get tiles, one from an (x,y) coordinate, and the other from a piece object.
        Tile * getTile(int x, int y);
        int getTileId(const Tile &tile);
        Tile * getPieceTile(const Piece &piece);

        // Methods to interact with pieces
        std::vector<Move> getValidMovement(const Piece &piece, Tile * currentTile, int relativeStrengthMod);
        std::vector<Move> getValidRangedAttacks(const Piece &piece, Tile * currentTile, int relativeRangedStrenghtMod, int relativeRangeMax);
        int movePiece(Piece * piece, Tile * target);
        int shootPiece(Piece * piece, Tile * target);
        int captureObjective(Piece * piece);
        bool pieceExists(Piece * piece);
};

