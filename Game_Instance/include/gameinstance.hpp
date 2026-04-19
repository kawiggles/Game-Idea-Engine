#pragma once

#include <cstddef>
#include <memory>
#include <vector>
#include <unordered_map>
#include <unordered_set>
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

    bool operator==(const Move &other) const {
        return type == other.type && to == other.to && from == other.from;
    }
};

struct MoveHash {
    size_t operator()(const Move &m) const {
        size_t h1 = std::hash<Tile *>{}(m.from);
        size_t h2 = std::hash<Tile *>{}(m.to);
        size_t h3 = std::hash<int>{}(static_cast<int>(m.type));
        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
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
        void makeGame(std::vector<std::unique_ptr<Piece>> &&runPieces, std::vector<std::unique_ptr<Piece>> &&enemyPieces);
        std::vector<std::unique_ptr<Piece>> playerPieces;
        int setupEnemy();
        int addPiece(Piece * piece, int tileIndex);

        // Public members and methods to run game
        std::unordered_map<int, std::unique_ptr<Tile>> board;
        int turnCount;

        enum class Status {
            Redo,
            Next,
            PlayerWin,
            EnemyWin,
            Quit
        };
        Status status;

        Status makePlayerMove(const Move &move, const std::unordered_set<Move, MoveHash> validMoves);
        Status takeEnemyTurn();
        Status getWinStatus();
        std::unordered_set<Move, MoveHash> getValidMoves(const Piece &piece, MoveType type);

    private:
        std::vector<std::unique_ptr<Piece>> enemyPieces;
        std::unordered_map<const Piece *, Tile *>  piecePositions;
        std::vector<Objective *> objectives;
        unsigned long seed;

        // Two methods to get tiles, one from an (x,y) coordinate, and the other from a piece object.
        Tile * getTile(int x, int y);
        int getTileId(const Tile &tile);
        Tile * getPieceTile(const Piece &piece);

        // Methods to interact with pieces
        std::unordered_set<Move, MoveHash> getValidMovement(const Piece &piece, Tile * currentTile, int relativeStrengthMod);
        std::unordered_set<Move, MoveHash> getValidRangedAttacks(const Piece &piece, Tile * currentTile, int relativeRangedStrenghtMod, int relativeRangeMax);
        bool pieceExists(Piece * piece);
};

