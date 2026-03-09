#include "printboards.hpp"
#include "pieces.hpp"
#include "tiles.hpp"
#include "gameinstance.hpp"
#include "types.hpp"

#include <iostream>
#include <assert.h>

int main() {
    std::cout << "Test Begin..." << std::endl;

    // Create arrays of test of pieces
    std::vector<Piece *> testPlayerPieces;
    testPlayerPieces.push_back(new Piece(PieceMaterial::Wood, PieceType::Light, true));
    testPlayerPieces.push_back(new Piece(PieceMaterial::Wood, PieceType::Light, true));
    testPlayerPieces.push_back(new Piece(PieceMaterial::Wood, PieceType::Archer, true));
    testPlayerPieces.push_back(new Piece(PieceMaterial::Stone, PieceType::LCavalry, true));
    testPlayerPieces.push_back(new Piece(PieceMaterial::Stone, PieceType::LCavalry, true));
    
    std::vector<Piece *> testEnemyPieces;
    // Make a new game instance
    GameInstance testGame(1234, BiomeType::Tropical, MissionType::FillerMission, 2, true);
    testGame.makeGame(testPlayerPieces, testEnemyPieces);

    // Add player pieces
    testGame.addPiece(testGame.playerPieces[0], testGame.boardWidth-1, testGame.boardHeight-1);
    testGame.addPiece(testGame.playerPieces[1], testGame.boardWidth-2, testGame.boardHeight-1);
    testGame.addPiece(testGame.playerPieces[2], testGame.boardWidth-3, testGame.boardHeight-1);

    // Moving pieces and reprinting board:
    bool endProgram = false;
    while (!endProgram) {
        // std::system("clear");
        printBoard(testGame.board, testGame.boardWidth, testGame.boardHeight);

        char input;
        std::cout << "Select a piece to move (q to quit): ";
        std::cin >> input;
        std::cin.ignore();

        if (input == 'q' || input == 'Q') {
            endProgram = true;
            continue;
        }

        Piece * selectedPiece = nullptr;

        switch (input) {
            case ('P'): case ('p'): selectedPiece = testGame.playerPieces[0]; break;
            case ('L'): case ('l'): selectedPiece = testGame.playerPieces[1]; break;
            case ('H'): case ('h'): selectedPiece = testGame.playerPieces[2]; break;
            default:
                std::cout << "Piece identifier not recognized. Enter to continue." << std::endl;
                continue;
        }

        printValidTilesBoard(testGame.board, testGame.getValidMoves(selectedPiece), testGame.boardWidth, testGame.boardHeight);

        int xin, yin;
        std::cout << "\nEnter new x coordinate: ";
        std::cin >> xin;
        std::cout << "Enter new y coordinate: ";
        std::cin >> yin;
        std::cin.ignore();

        if (testGame.movePiece(selectedPiece, xin-1, yin-1)) {
            std::cout << "\nMove Successful. Press enter to build new board.\n";
            std::cin.ignore();
        } else {
            std::cout << "\nMove Failed. Press enter to try again.\n";
            std::cin.ignore();
        }
        
    }
//
    return 0;
}
