#include "tiles.hpp"
#include "gameinstance.hpp"
#include "types.hpp"
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
    curs_set(0);

    return newWindow;
}

void destroyWindow(WINDOW * window) {
    wborder(window, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
    wrefresh(window);
    delwin(window);
}

void initColors() {
    start_color();
    can_change_color();
    // Terrain Colors
    init_color(8, 0, 500, 0); // Light Green for forest
    init_color(9, 500, 500, 0); // Light Yellow for fields
    init_color(10, 500, 0, 0); // Light Red for roads
    init_color(11, 188, 98, 203); // Dark Purple for peaks
    init_color(12, 800, 800, 800); // Highlight color
    // Normal pairs
    init_pair(1, 4, 0);     // Water
    init_pair(2, 9, 0);     // Field
    init_pair(3, 8, 0);     // Forest
    init_pair(4, 5, 0);     // Mountain
    init_pair(5, 10, 0);    // Road
    init_pair(6, 3, 0);     // Desert
    init_pair(7, 2, 0);     // Jungle
    init_pair(8, 11, 0);    // Peak
    init_pair(9, 6, 0);     // Objective
    // Pairs for when valid
    init_pair(11, 4, 12);
    init_pair(12, 9, 12);
    init_pair(13, 8, 12);
    init_pair(14, 5, 12);
    init_pair(15, 10, 12);
    init_pair(16, 3, 12);  
    init_pair(17, 2, 12);
    init_pair(18, 11, 12);
    init_pair(19, 6, 12);
    // Pairs for pieces
    init_pair(21, 7, 0);
    init_pair(22, 1, 0);
    init_pair(23, 7, 12);
    init_pair(24, 1, 12);
}

// String generating functions
Symbol getSymbol(const Tile &tile) {
    Symbol symbol;
    
    if (tile.occupyingPiece) {
        switch (tile.occupyingPiece->type) {
            case PieceType::Light:     symbol.pieceSymbol = 'P'; break;
            case PieceType::Shield:    symbol.pieceSymbol = 'S'; break;
            case PieceType::Elite:     symbol.pieceSymbol = 'E'; break;
            case PieceType::Archer:    symbol.pieceSymbol = 'A'; break;
            case PieceType::LCavalry:  symbol.pieceSymbol = 'L'; break;
            case PieceType::MCavalry:  symbol.pieceSymbol = 'M'; break;
            case PieceType::HCavalry:  symbol.pieceSymbol = 'H'; break;
            case PieceType::Catapult:  symbol.pieceSymbol = 'T'; break;
            case PieceType::Ballista:  symbol.pieceSymbol = 'B'; break;
            case PieceType::Chariot:   symbol.pieceSymbol = 'C'; break;
            case PieceType::Commander: symbol.pieceSymbol = 'G'; break;
            case PieceType::Wizard:    symbol.pieceSymbol = 'W'; break;
            case PieceType::Assassin:  symbol.pieceSymbol = 'X'; break;
            case PieceType::Druid:     symbol.pieceSymbol = 'D'; break;
        }

        if (tile.occupyingPiece->ownedByPlayer) {
            symbol.pieceColor = 21;
        } else {
            symbol.pieceColor = 22;
        }
    }
    
    switch (tile.terrain) {
        case TerrainType::Water:    symbol.terrainColor = 1; break;
        case TerrainType::Field:    symbol.terrainColor = 2; break;
        case TerrainType::Forest:   symbol.terrainColor = 3; break;
        case TerrainType::Mountain: symbol.terrainColor = 4; break;
        case TerrainType::Road:     symbol.terrainColor = 5; break;
        case TerrainType::Desert:   symbol.terrainColor = 6; break;
        case TerrainType::Jungle:   symbol.terrainColor = 7; break;
        case TerrainType::Peak:     symbol.terrainColor = 8; break;
        // Arctic Tiles
        case TerrainType::IceField: symbol.terrainColor = 31; break;
        case TerrainType::SnowField:symbol.terrainColor = 32; break;
        case TerrainType::Tundra:   symbol.terrainColor = 33; break;
        // Mission Tiles
        case TerrainType::Objective:symbol.terrainColor = 9; break;
    }
    
    return symbol;
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
void printBoard(const std::vector<Tile> &board, int width, int height, WINDOW * window, int cursorX, int cursorY) {
    wclear(window);
    int tileIndex = 0;
    mvwprintw(window, 1, 1, "    "); // Empty space at the start of coordinate numbering
    for (int i = 0; i < width; i++) { // Column numbering
        if (i < 9) {
            wprintw(window, "[ %d]", i+1);
        } else {
            wprintw(window, "[%d]", i+1);
        }
    }
    for (int i = 0; i < height; i++) { // Row numbering
        wmove(window, i + 2, 1);
        if (i < 9) {
            wprintw(window, "[ %d]", i+1);
        } else {
            wprintw(window, "[%d]", i+1);
        }

        for (int j = 0; j < width; j++) {
            Symbol symbol = getSymbol(board[tileIndex]);
            if (tileIndex == cursorY * width + cursorX) wattron(window, A_BLINK);
            if (board[tileIndex].terrain == TerrainType::Water) {
                wattron(window, COLOR_PAIR(symbol.terrainColor));
                wprintw(window, "[~~]");
                wattroff(window, COLOR_PAIR(symbol.terrainColor));
                tileIndex++;
                continue;
            }
            wattron(window, COLOR_PAIR(symbol.terrainColor));
            wprintw(window, "[ ");
            wattroff(window, COLOR_PAIR(symbol.terrainColor));
            wattron(window, COLOR_PAIR(symbol.pieceColor));
            wprintw(window, "%c", symbol.pieceSymbol);
            wattroff(window, COLOR_PAIR(symbol.pieceColor));
            wattron(window, COLOR_PAIR(symbol.terrainColor));
            wprintw(window, "]");
            wattroff(window, COLOR_PAIR(symbol.terrainColor));
            if (tileIndex == cursorY * width + cursorX) wattroff(window, A_BLINK);

            tileIndex++;
        }
    }
    wrefresh(window);
}

void printValidTilesBoard(std::vector<Tile> &board, std::vector<Move> moves, int width, int height, WINDOW * window) {
    std::unordered_set<Tile *> moveSet(moves.size()); 
    for (int i = 0; i < moves.size(); i++) moveSet.insert(moves[i].to);
    
    int tileIndex = 0;
    mvwprintw(window, 1, 1, "    ");
    for (int i = 0; i < width; i++) { // Column numbering
        if (i < 9) {
            wprintw(window, "[ %d]", i+1);
        } else {
            wprintw(window, "[%d]", i+1);
        }
    }
    for (int i = 0; i < height; i++) {
        wmove(window, i + 2, 1);
        if (i < 9) {
            wprintw(window, "[ %d]", i+1); // Row numbering
        } else {
            wprintw(window,"[%d]", i+1);
        }

        for (int j = 0; j < width; j++) {
            
            (moveSet.count(&board[tileIndex])) ? printw("\033[47m") : printw("\033[49m"); // If the current tile is in the set of possible moves, 47m background, else 49m background
            wprintw(window, "%s", getSymbol(board[tileIndex]));
            printw("\033[0m");

            tileIndex++;
        }
    }
    wrefresh(window);
}

// Functions for running test game instance
void setupGame(GameInstance &game, WINDOW * terminalWindow) {
    void (*func_ptr)(const std::vector<Tile> &board, int width, int height, WINDOW * window, int cursorX, int cursorY) = &printBoard;
    auto getUserInput = [game, terminalWindow, func_ptr](WINDOW * boardWindow, int cursorX, int cursorY) {
        int ch;       
        func_ptr(game.board, game.boardWidth, game.boardHeight, boardWindow, cursorX, cursorY);
        while ((ch = wgetch(terminalWindow)) != '\n') {
            switch (ch) {
                case KEY_UP:
                    if (cursorY > 0) cursorY--;
                    break;
                case KEY_DOWN:
                    if (cursorY < game.boardHeight - 1) cursorY++; 
                    break;
                case KEY_LEFT:
                    if (cursorX > 0) cursorX--;
                    break;
                case KEY_RIGHT:
                    if (cursorX < game.boardWidth - 1) cursorX++;
                    break;
            }
            func_ptr(game.board, game.boardWidth, game.boardHeight, boardWindow, cursorX, cursorY);
        }
        return cursorY * game.boardWidth + cursorX;
    };

    wrefresh(terminalWindow);

    int winWidth = 4*(game.boardWidth+1) + 2;
    int winHeight = game.boardHeight + 1 + 2;
    WINDOW * boardWindow = createNewWindow(winHeight, winWidth, (LINES-winHeight)/4, (COLS-winWidth)/2);
    
    int cursorX = 0, cursorY = game.boardHeight-1;
    for (int i = 0; i < game.playerPieces.size(); i++) {
        Piece * piece = game.playerPieces[i];
        wprintw(terminalWindow, "Placing piece %d of %lu:\n", i+1, game.playerPieces.size());
        wprintw(terminalWindow, "    Piece type: %s\n", getPieceType(piece).c_str()); 

        wprintw(terminalWindow, "Select tile to place piece: ");
        wrefresh(terminalWindow);

        int coordInput = getUserInput(boardWindow, cursorX, cursorY);
        cursorX = game.board[coordInput].x;
        cursorY = game.board[coordInput].y;

        if (coordInput > game.board.size() || coordInput < 0) {
            wprintw(terminalWindow, "Error, selected (x, y) coordinate outside of board range.\n");
            i--;
            continue;
        }

        if (coordInput < game.board.size() - game.boardWidth - 1) {
            wprintw(terminalWindow, "Error, selected (x, y) coordinate outside of player deployment zone.\n");
            i--;
            continue;
        }

        if (game.addPiece(piece, coordInput)) {
            wprintw(terminalWindow, "\nPiece successfully added at (%d, %d)\n", cursorX+1, cursorY+1);
        } else {
            i--;
            wprintw(terminalWindow, "Piece placement unsuccessful, try again\n");
            continue;
        }
        wrefresh(terminalWindow);
    }

    wprintw(terminalWindow, "Placing enemy pieces...\n");
    for (int i = 0; i < game.enemyPieces.size(); i++) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution disX(0, game.boardWidth-1);
        // std::uniform_int_distribution disY(0, 1);
        
        if (game.addPiece(game.enemyPieces[i], disX(gen))) {
            wprintw(terminalWindow, "Enemy piece %d added.\n", i+1);
            wrefresh(terminalWindow);
        } else {
            wprintw(terminalWindow, "Error, enemy piece #%d failed to be added to board, trying again\n", i+1);
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
            printBoard(game.board, game.boardWidth, game.boardHeight, window, 2, 4);
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
                    printValidTilesBoard(game.board, game.getValidMoves(piece), game.boardWidth, game.boardHeight, window);
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
