#include "tiles.hpp"
#include "gameinstance.hpp"
#include "printboards.hpp"

#include <iostream>
#include <unordered_set>
#include <random>
#include <ncurses.h>
/*
 * This is a temporary program which uses ncurses to represent the game state
 * The first set of functions controls how ncurses windows are created and destroyed
 * The second set returns string values for various object properties in the game
 * The third set prints the board state of a given game instance
 * Finally, the last functions control a test game instance
 */ 

// Functions for controlling ncurses
WINDOW * createNewWindow(int height, int width, int starty, int startx) {
    WINDOW * newWindow = newwin(height, width, starty, startx);
    box(newWindow, 0, 0);
    wrefresh(newWindow);

    return newWindow;
}

void destroyWindow(WINDOW * window) {
    wborder(window, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
    wrefresh(window);
    delwin(window);
}

// String generating functions
std::string getTileSymbol(const Tile &tile) {
    std::string symbol = " ";  // Default: empty space
    std::string pieceColor = ""; // Default: no color
    
    if (tile.occupyingPiece) {
        // Determine piece letter
        switch (tile.occupyingPiece->type) {
            case PieceType::Light:     symbol = "P"; break;
            case PieceType::Shield:    symbol = "S"; break;
            case PieceType::Elite:     symbol = "E"; break;
            case PieceType::Archer:    symbol = "A"; break;
            case PieceType::LCavalry:  symbol = "L"; break;
            case PieceType::MCavalry:  symbol = "M"; break;
            case PieceType::HCavalry:  symbol = "H"; break;
            case PieceType::Catapult:  symbol = "T"; break;
            case PieceType::Ballista:  symbol = "B"; break;
            case PieceType::Chariot:   symbol = "C"; break;
            case PieceType::Commander: symbol = "G"; break;
            case PieceType::Wizard:    symbol = "W"; break;
            case PieceType::Assassin:  symbol = "X"; break;
            case PieceType::Druid:     symbol = "D"; break;
        }
        
        // Color enemy pieces red
        if (!tile.occupyingPiece->ownedByPlayer) {
            pieceColor = "";  // Red
        }
    }
    
    // Get terrain bracket color
    std::string bracketColor;
    switch (tile.terrain) {
        case TerrainType::Water:    return "[~~]";
        case TerrainType::Field:    bracketColor = ""; break;  // Yellow
        case TerrainType::Forest:   bracketColor = ""; break;  // Green
        case TerrainType::Mountain: bracketColor = ""; break;  // Purple
        case TerrainType::Road:     bracketColor = ""; break;  // HI Red
        case TerrainType::Desert:   bracketColor = ""; break;  // HI Yellow
        case TerrainType::Jungle:   bracketColor = ""; break;  // HI Green
        case TerrainType::Peak:     bracketColor = ""; break;  // Black
        // Arctic Tiles
        case TerrainType::IceField: bracketColor = ""; break;  // Cyan
        case TerrainType::SnowField:bracketColor = ""; break;  // White
        case TerrainType::Tundra:   bracketColor = ""; break;  // Green again, but it's fine because it replaces forests
        // Mission Tiles
        case TerrainType::Objective:bracketColor = ""; break;  // HI Blue
    }
    
    // Construct final symbol
    return bracketColor + "[ " + "" + 
           pieceColor + symbol + "" + 
           bracketColor + "]" + "";
}

std::string getPieceType(const Piece * piece) {
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

std::string getBiomeType(const BiomeType biome) {
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

// Functions for printing board state
void printBoard(std::vector<Tile> &board, int width, int height, WINDOW * window) {
    int tileIndex = 0;
    wprintw(window, "    "); // Empty space at the start of coordinate numbering
    for (int i = 0; i < width; i++) { // Column numbering
        if (i < 9) {
            wprintw(window, "[ %d]", i+1);
        } else {
            wprintw(window, "[%d]", i+1);
        }
    }
    wprintw(window, "\n");;
    for (int i = 0; i < height; i++) {
        if (i < 9) {
            wprintw(window, "[ %d]", i+1); // Row numbering
        } else {
            wprintw(window, "[%d]", i+1);
        }

        for (int j = 0; j < width; j++) {
            wprintw(window, "%s", getTileSymbol(board[tileIndex]).c_str());
            tileIndex++;
        }
        wprintw(window, "\n");
    }
    refresh();
}

void printValidTilesBoard(std::vector<Tile> &board, std::vector<Move> moves, int width, int height) {
    std::unordered_set<Tile *> moveSet(moves.size()); 
    
    for (int i = 0; i < moves.size(); i++) moveSet.insert(moves[i].to);
    
    int tileIndex = 0;
    printw("    ");
    for (int i = 0; i < width; i++) { // Column numbering
        if (i < 9) {
            printw("[ %d]", i+1);
        } else {
            printw("[%d]", i+1);
        }
    }
    printw("\n");
    for (int i = 0; i < height; i++) {
        if (i < 9) {
            printw("[ %d]", i+1); // Row numbering
        } else {
            printw("[%d]", i+1);
        }

        for (int j = 0; j < width; j++) {
            
            (moveSet.count(&board[tileIndex])) ? printw("\033[47m") : printw("\033[49m"); // If the current tile is in the set of possible moves, 47m background, else 49m background
            printw("%s", getTileSymbol(board[tileIndex]).c_str());
            printw("\033[0m");

            tileIndex++;
        }
        printw("\n");
    }
    refresh();
}

// Functions for running test game instance
void setupGame(GameInstance &game) {
    auto getUserInput = []() {
        std::string input = "";
        int ch;       
        while ((ch = getch()) != '\n') {
            if (ch == KEY_BACKSPACE || ch == 263) {
                if (!input.empty()) {
                    input.pop_back();
                    int y, x;
                    getyx(stdscr, y, x);
                    mvprintw(y, x-1, " ");
                    move(y, x-1);
                    refresh();
                }
            } else if (ch >= '0' && ch <= '9') {
                input += (char)ch;
                addch(ch);
                refresh();
            }
        }
        return input;
    };

    int winWidth = 4*(game.boardWidth+1);
    int winHeight = game.boardHeight+1;
    WINDOW * window = createNewWindow(winHeight, winWidth, (LINES-winHeight)/2, (COLS-winWidth)/2);

    for (int i = 0; i < game.playerPieces.size(); i++) {
        printBoard(game.board, game.boardWidth, game.boardHeight, window);

        Piece * piece = game.playerPieces[i];
        printw("Placing piece %d of %lu:\n", i+1, game.playerPieces.size());
        printw("    Piece type: %s\n", getPieceType(piece).c_str()); 

        std::string coordInput;
        int xin, yin;
        printw("Enter x coordinate: ");
        refresh();
        coordInput = getUserInput();
        if (coordInput.empty()) {
            printw("\nError: no coordinate entered\n");
            refresh();
            i--;
            continue;
        } else {
            xin = stoul(coordInput);
        }

        coordInput = "";
        printw("Enter y cooridnate: ");
        refresh();
        if (coordInput.empty()) {
            printw("\nError: no coordinate entered\n");
            refresh();
            i--;
            continue;
        } else {
            xin = stoul(coordInput);
        }

        xin--;
        yin--;

        if (xin >= game.boardWidth || xin < 0 || yin >= game.boardHeight || yin < 0) {
            printw("Error, selected (x, y) coordinate outside of board range.\n");
            refresh();
            i--;
            continue;
        }

        if (yin < game.boardHeight - 1) {
            printw("Error, selected (x, y) coordinate outside of player deployment zone.\n");
            refresh();
            i--;
            continue;
        }

        if (game.addPiece(piece, xin, yin)) {
            printw("Piece successfully added at (%d, %d)\n", xin+1, yin+1);
        } else {
            i--;
            printw("Piece placement unsuccessful, try again\n");
            continue;
        }
    }

    printw("Placing enemy pieces...\n");
    for (int i = 0; i < game.enemyPieces.size(); i++) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution disX(0, game.boardWidth);
        // std::uniform_int_distribution disY(0, 1);
        
        if (game.addPiece(game.enemyPieces[i], disX(gen), 0)) {
            printw("Enemy piece %d added.\n", i+1);
            refresh();
        } else {
            printw("Error, enemy piece #%d failed to be added to board, trying again", i+1);
            i--;
        }
    }
}

void runGame(GameInstance &game, bool startingPlayer) {
    printw("Starting Game\n");;
    (startingPlayer) ? game.turnCount = 1 : game.turnCount = 2;
    printw("Starting Player: %s\n", (startingPlayer) ? "player" : "enemy");
    refresh();
    int winWidth = 4*(game.boardWidth+1);
    int winHeight = game.boardHeight+1;
    WINDOW * window = createNewWindow(winHeight, winWidth, (LINES-winHeight)/2, (COLS-winWidth)/2);
    
    int gameStatus = 0; // 0: redo move
                        // 1: go onto next turn
                        // 2: player wins
                        // 3: enemy wins
                        // 4: quit
    while (!(gameStatus == 4 || gameStatus == 3 || gameStatus == 2)) {
        printw("Turn %d\n", game.turnCount);
        refresh();

        if (game.turnCount % 2 == 1) {
            printBoard(game.board, game.boardWidth, game.boardHeight, window);
            printw("Starting player turn...\n");
            printw("Player pieces: \n");
            refresh();
            Move selectedMove;

            for (Piece * piece : game.playerPieces) {
                printw("    Piece Type: %s with ID: %d  at (%d, %d)", getPieceType(piece).c_str(), piece->id, game.getPieceTile(piece)->x+1, game.getPieceTile(piece)->y+1);
            }

            int input;
            printw("Enter the ID of the piece to move (0 to quit): ");
            std::cin >> input;
            std::cin.ignore();

            if (input == 0) {
                printw("Quitting...");
                gameStatus = 4;
                break;
            }

            bool pieceFound = false;
            for (Piece * piece : game.playerPieces) {
                if (input-1 == piece->id) {
                    pieceFound = true;
                    printValidTilesBoard(game.board, game.getValidMoves(piece), game.boardWidth, game.boardHeight);
                    int xin, yin;
                    printw("Enter x coordinate: ");
                    std::cin >> xin;
                    std::cin.ignore();

                    printw("Enter y cooridnate: ");
                    std::cin >> yin;
                    std::cin.ignore();

                    xin--;
                    yin--;

                    selectedMove = Move{MoveType::Move, game.getPieceTile(piece), game.getTile(xin, yin)};
                }
            }
            (pieceFound) ? gameStatus = game.takePlayerTurn(selectedMove) : gameStatus = 0;
        } else {
            printw("Enemy taking turn\n");
            gameStatus = game.takeEnemyTurn();
        }

        if (gameStatus == 0) game.turnCount--;
        game.turnCount++;
    }   
    
    if (gameStatus == 2) {
        printw("Player Wins!");
    } else {
        printw("Enemy Wins.");
    }
}
