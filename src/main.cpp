#include "runes.hpp"
#include "pieces.hpp"
#include "tiles.hpp"
#include "boards.hpp"
#include "gameinstance.hpp"

#include <iostream>
#include <assert.h>
#include <cstdlib>

/*
void enableANSI() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}
*/

int main() {
    // enableANSI();
    srand(time(NULL));

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

    // Add player pieces

    // Add enemy pieces 

    // Moving pieces and reprinting board:
    bool endProgram = false;
    while (!endProgram) {
        std::system("cls");

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
//            case ('P'): case ('p'): selectedPiece = &testGame.playerPieces[0]; break;
//            case ('L'): case ('l'): selectedPiece = &testGame.playerPieces[1]; break;
//            case ('H'): case ('h'): selectedPiece = &testGame.playerPieces[2]; break;
//            case ('A'): case ('a'): selectedPiece = &testGame.enemyPieces[0]; break;
//            case ('W'): case ('w'): selectedPiece = &testGame.enemyPieces[1]; break;
            default:
                std::cout << "Piece identifier not recognized. Enter to continue.";
                std::cin.ignore();
                continue;
        }

//        std::vector <Tile *> validMoves = testGame.getValidMoves(selectedPiece);
        
//        printValidTilesBoard(testGame.board, validMoves);

        int xin, yin;
        std::cout << "\nEnter new x coordinate:";
        std::cin >> xin;
        std::cout << "Enter new y coordinate:";
        std::cin >> yin;
        std::cin.ignore();
/*
        if (testGame.movePiece(selectedPiece, xin, yin)) {
            std::cout << "\nMove Successful. Press enter to build new board.\n";
            std::cin.ignore();
        } else {
            std::cout << "\nMove Failed. Press enter to try again.\n";
            std::cin.ignore();
        }
*/        
    }

    return 0;
}
