#include "runes.hpp"
#include "pieces.hpp"
#include "tiles.hpp"
#include "boards.hpp"
#include "gameinstance.hpp"

#include <iostream>
#include <windows.h>
#include <assert.h>

void enableANSI() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}

int main() {
    enableANSI();

    std::cout << "Test Begin..." << std::endl;

    // Create arrays of test of pieces
    std::vector<Piece> testPlayerPieces = {
        makeNewPiece("test1", PieceMaterial::Wood, PieceType::Light, true),
        makeNewPiece("test2", PieceMaterial::Gold, PieceType::LCavalry, true),
        makeNewPiece("test3", PieceMaterial::Diamond, PieceType::HCavalry, true)
    };

    std::vector<Piece> testEnemyPieces = {
        makeNewPiece("test4", PieceMaterial::Iron, PieceType::Archer, false),
        makeNewPiece("test5", PieceMaterial::Silver, PieceType::Wizard, false)
    };

    // Make a new game instance
    GameInstance testGame(testPlayerPieces, testEnemyPieces, 8, 8);

    // Add player pieces
    testGame.addPiece(&testGame.playerPieces[0], 1, 6);
    testGame.addPiece(&testGame.playerPieces[1], 3, 6);
    testGame.addPiece(&testGame.playerPieces[2], 5, 6);

    // Add enemy pieces 
    testGame.addPiece(&testGame.enemyPieces[0], 6, 1);
    testGame.addPiece(&testGame.enemyPieces[1], 4, 1);

    // Moving pieces and reprinting board:
    bool endProgram = false;
    while (!endProgram) {
        std::system("cls");
        printBoard(testGame.board);

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
            case ('P'): case ('p'): selectedPiece = &testGame.playerPieces[0]; break;
            case ('L'): case ('l'): selectedPiece = &testGame.playerPieces[1]; break;
            case ('H'): case ('h'): selectedPiece = &testGame.playerPieces[2]; break;
            case ('A'): case ('a'): selectedPiece = &testGame.enemyPieces[0]; break;
            case ('W'): case ('w'): selectedPiece = &testGame.enemyPieces[1]; break;
            default:
                std::cout << "Piece identifier not recognized. Enter to continue.";
                std::cin.ignore();
                continue;
        }

        std::vector <Tile *> validMoves = testGame.getValidMoves(selectedPiece);
        
        printValidTilesBoard(testGame.board, validMoves);

        int xin, yin;
        std::cout << "\nEnter new x coordinate:";
        std::cin >> xin;
        std::cout << "Enter new y coordinate:";
        std::cin >> yin;
        std::cin.ignore();

        if (testGame.movePiece(selectedPiece, xin, yin)) {
            std::cout << "\nMove Successful.\n";
            std::cin.ignore();
        } else {
            std::cout << "\nMove Failed.\n";
            std::cin.ignore();
        }
        
    }

    return 0;
}