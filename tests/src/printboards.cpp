#include "tiles.hpp"
#include "gameinstance.hpp"
#include "printboards.hpp"

#include <iostream>
#include <unordered_set>
#include <cstdlib>

/*  This is a complicated bit of code, so I'm explaining it here. board.tiles[] is a vector array, defined in boards.hpp. The two loops iterate through this array, using height and width (also defined in boards.hpp) to know when to end and begin new rows. The tile index, when given to the array board.tiles[] will return one tile struct. That tile struct is then passed through getTileSymbol, which takes a tile struct and returns a std::string object corresponding to the properties of the tile struct. That gets printed with std::cout and the index gets incremented.*/
void printBoard(std::vector<Tile> &board, int width, int height) {
    int tileIndex = 0;
    std::cout << "    "; // Empty space at the start of coordinate numbering
    for (int i = 0; i < width; i++) { // Column numbering
        if (i < 9) {
            std::cout << "[ " << i+1 << "]";
        } else {
            std::cout << "[" << i+1 << "]";
        }
    }
    std::cout << std::endl;
    for (int i = 0; i < height; i++) {
        if (i < 9) {
            std::cout << "[ " << i+1 << "]"; // Row numbering
        } else {
            std::cout << "[" << i+1 << "]";
        }

        for (int j = 0; j < width; j++) {
            std::cout << "\033[49m" << getTileSymbol(board[tileIndex]) << "\033[0m";
            tileIndex++;
        }
        std::cout << std::endl;
    }
}

/* This function essentially does the exact same thing, but also adds highlights for valid moves for a selected piece. It takes a vector of those valid moves as an additional input. */
void printValidTilesBoard(std::vector<Tile> &board, std::vector<Move> moves, int width, int height) {
    std::unordered_set<Tile *> moveSet(moves.size()); 
    
    for (int i = 0; i < moves.size(); i++) moveSet.insert(moves[i].to);
    
    int tileIndex = 0;
    std::cout << "    ";
    for (int i = 0; i < width; i++) { // Column numbering
        if (i < 9) {
            std::cout << "[ " << i+1 << "]";
        } else {
            std::cout << "[" << i+1 << "]";
        }
    }
    std::cout << std::endl;
    for (int i = 0; i < height; i++) {
        if (i < 9) {
            std::cout << "[ " << i+1 << "]"; // Row numbering
        } else {
            std::cout << "[" << i+1 << "]";
        }

        for (int j = 0; j < width; j++) {
            
            (moveSet.count(&board[tileIndex])) ? std::cout << "\033[47m" : std::cout << "\033[49m"; // If the current tile is in the set of possible moves, 47m background, else 49m background
            std::cout << getTileSymbol(board[tileIndex]);
            std::cout << "\033[0m";

            tileIndex++;
        }
        std::cout << std::endl;
    }
}

void setupGame(GameInstance &game) {
    auto getPieceType = [](Piece * piece) {
        switch(piece->type) {
            case PieceType::Light:      return "Light Infantry";
            case PieceType::Shield:     return "Shield Infantry";
            case PieceType::Elite:      return "Elite Infantry";
            case PieceType::Archer:     return "Archer";
            case PieceType::LCavalry:   return "Light Cavalry";
            case PieceType::MCavalry:   return "Medium Cavalry";
            case PieceType::HCavalry:   return "Heavy Cavalry";
            case PieceType::Catapult:   return "Trebuchet";
            case PieceType::Ballista:   return "Ballista";
            case PieceType::Chariot:    return "Chariot";
            case PieceType::Commander:  return "Commander";
            case PieceType::Wizard:     return "Wizard";
            case PieceType::Assassin:   return "Assassin";
            case PieceType::Druid:      return "Druid";
            default:                    return "Unknown";
        }
    };
    for (int i = 0; i < game.playerPieces.size(); i++) {
        std::system("clear");
        printBoard(game.board, game.boardWidth, game.boardHeight);

        Piece * piece = game.playerPieces[i];
        std::cout << "Placing piece " << i+1 << " of " << game.playerPieces.size() << ":" << std::endl;
        std::cout << "    Piece type: " << getPieceType(piece) << std::endl; 

        int xin, yin;
        std::cout << "Enter x coordinate: ";
        std::cin >> xin;
        std::cin.ignore();

        std::cout << "Enter y cooridnate: ";
        std::cin >> yin;
        std::cin.ignore();

        xin--;
        yin--;

        if (xin >= game.boardWidth || xin < 0 || yin >= game.boardHeight || yin < 0) {
            std::cout << "Error, selected (x, y) coordinate outside of board range.";
            std::cin.ignore();
            i--;
            break;
        }

        if (yin < game.boardHeight - 3) {
            std::cout << "Error, selected (x, y) coordinate outside of player deployment zone.";
            std::cin.ignore();
            i--;
            break;
        }

        if (game.addPiece(piece, xin, yin)) {
            std::cout << "Piece successfully added at (" << xin+1 << ", " << yin+1 << ")";
            std::cin.ignore();
        } else {
            i--;
            std::cout << "Piece placement unsuccessful, try again";
            std::cin.ignore();
        }
    }
}

