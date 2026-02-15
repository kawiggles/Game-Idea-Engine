#ifndef GAMEINSTANCE_HPP
#define GAMEINSTANCE_HPP

#include <vector>

// Forward Declarations
#include "boards.hpp"
#include "tiles.hpp"
#include "pieces.hpp"

// An instance of a game. Basically contains everything needed to run the core mechanics of the game
class GameInstance {
    public:

        // Members of GameInstance
        bool playerWin;
        Board board;
        std::vector<Piece> playerPieces;
        std::vector<Piece> enemyPieces;

        // Constructor for GameInstance
        GameInstance(std::vector<Piece> runPieces, std::vector<Piece> enemyPieces, int boardX, int boardY);

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