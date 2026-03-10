#include "tiles.hpp"
#include "gameinstance.hpp"
#include "printboards.hpp"

#include <iostream>
#include <unordered_set>
#include <random>

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

std::string getPieceType(Piece * piece) {
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
}

std::string getBiomeType(BiomeType biome) {
    switch (biome) {
        case BiomeType::Grassy:     return "Grassland";
        case BiomeType::Temperate:  return "Forest";
        case BiomeType::Arid:       return "Desert";
        case BiomeType::Tropical:   return "Jungle";
        case BiomeType::Alpine:     return "Alpine";
        case BiomeType::Arctic:     return "Arctic";
        default:                    return "Unknown";
    }
}

void setupGame(GameInstance &game) {
    for (int i = 0; i < game.playerPieces.size(); i++) {
        // std::system("clear");
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

        if (yin < game.boardHeight - 2) {
            std::cout << "Error, selected (x, y) coordinate outside of player deployment zone.";
            std::cin.ignore();
            i--;
            break;
        }

        if (game.addPiece(piece, xin, yin)) {
            std::cout << "Piece successfully added at (" << xin+1 << ", " << yin+1 << ")" << std::endl;
        } else {
            i--;
            std::cout << "Piece placement unsuccessful, try again" << std::endl;
        }
    }

    std::cout << "Placing enemy pieces..." << std::endl;
    for (int i = 0; i < game.enemyPieces.size(); i++) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution disX(0, game.boardWidth);
        std::uniform_int_distribution disY(0, 1);
        
        if (game.addPiece(game.enemyPieces[i], disX(gen), disY(gen))) {
            std::cout << "Enemy piece " << i+1 << " added." << std::endl;
        } else {
            i--;
        }
    }
}

void runGame(GameInstance &game, bool startingPlayer) {
    std::cout << "Starting Game" << std::endl;
    (startingPlayer) ? game.turnCount = 1 : game.turnCount = 2;
    std::cout << "Starting Player: " << ((startingPlayer) ? "player" : "enemy") << std::endl;
    
    int gameStatus = 0; // 0: redo move
                        // 1: go onto next turn
                        // 2: player wins
                        // 3: enemy wins
                        // 4: quit
    while (!(gameStatus == 4 || gameStatus == 3 || gameStatus == 2)) {
        std::cout << "Turn " << game.turnCount << std::endl;

        if (game.turnCount % 2 == 1) {
            printBoard(game.board, game.boardWidth, game.boardHeight);
            std::cout << "Starting player turn..." << std::endl;
            std::cout << "Player pieces: " << std::endl;
            Move selectedMove;

            for (Piece * piece : game.playerPieces) {
                std::cout << "    Piece Type: " << getPieceType(piece) << " with ID: " << piece->id+1 << " at (" << game.getPieceTile(piece)->x+1 << ", " << game.getPieceTile(piece)->y+1 << ")" << std::endl;
            }

            int input;
            std::cout << "Enter the ID of the piece to move (0 to quit): ";
            std::cin >> input;
            std::cin.ignore();

            if (input == 0) {
                std::cout << "Quitting..." << std::endl;
                gameStatus = 4;
                break;
            }

            bool pieceFound = false;
            for (Piece * piece : game.playerPieces) {
                if (input-1 == piece->id) {
                    pieceFound = true;
                    printValidTilesBoard(game.board, game.getValidMoves(piece), game.boardWidth, game.boardHeight);
                    int xin, yin;
                    std::cout << "Enter x coordinate: ";
                    std::cin >> xin;
                    std::cin.ignore();

                    std::cout << "Enter y cooridnate: ";
                    std::cin >> yin;
                    std::cin.ignore();

                    xin--;
                    yin--;

                    selectedMove = Move{MoveType::Move, game.getPieceTile(piece), game.getTile(xin, yin)};
                }
            }
            (pieceFound) ? gameStatus = game.takePlayerTurn(selectedMove) : gameStatus = 0;
        } else {
            std::cout << "Enemy taking turn" << std::endl;
            gameStatus = game.takeEnemyTurn();
        }

        if (gameStatus == 0) game.turnCount--;
        game.turnCount++;
    }   
    
    if (gameStatus == 2) {
        std::cout << "Player Wins!" << std::endl;
    } else {
        std::cout << "Enemy Wins." << std::endl;
    }
}
