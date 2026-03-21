#include "tiles.hpp"
#include "gameinstance.hpp"
#include "types.hpp"
#include "printboards.hpp"

#include <unordered_set>
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
    wclear(window);
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
    init_color(12, 700, 700, 700); // Highlight color
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
    init_pair(21, COLOR_WHITE, 0);
    init_pair(22, COLOR_RED, 0);
    init_pair(23, COLOR_WHITE, 12);
    init_pair(24, COLOR_RED, 12);
}

// String generating functions
Symbol getSymbol(const Tile &tile) {
    Symbol symbol;

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
    } else {
        symbol.pieceColor = symbol.terrainColor;
    }

    return symbol;
}

Symbol getValidTileSymbol(const Tile &tile) {
    Symbol symbol;
    
    switch (tile.terrain) {
        case TerrainType::Water:    symbol.terrainColor = 11; break;
        case TerrainType::Field:    symbol.terrainColor = 12; break;
        case TerrainType::Forest:   symbol.terrainColor = 13; break;
        case TerrainType::Mountain: symbol.terrainColor = 14; break;
        case TerrainType::Road:     symbol.terrainColor = 15; break;
        case TerrainType::Desert:   symbol.terrainColor = 16; break;
        case TerrainType::Jungle:   symbol.terrainColor = 17; break;
        case TerrainType::Peak:     symbol.terrainColor = 18; break;
        // Arctic Tiles
        case TerrainType::IceField: symbol.terrainColor = 31; break;
        case TerrainType::SnowField:symbol.terrainColor = 32; break;
        case TerrainType::Tundra:   symbol.terrainColor = 33; break;
        // Mission Tiles
        case TerrainType::Objective:symbol.terrainColor = 19; break;
    }
    
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
            symbol.pieceColor = 23;
        } else {
            symbol.pieceColor = 24;
        }
    } else {
        symbol.pieceColor = symbol.terrainColor;
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
    box(window, 0, 0);
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
            if (tileIndex == cursorY * width + cursorX) wattron(window, A_REVERSE);
            if (board[tileIndex].terrain == TerrainType::Water) {
                wattron(window, COLOR_PAIR(symbol.terrainColor));
                wprintw(window, "[~~]");
                wattroff(window, COLOR_PAIR(symbol.terrainColor));
                if (tileIndex == cursorY * width + cursorX) wattroff(window, A_REVERSE);
                tileIndex++;
                continue;
            }
            wattron(window, COLOR_PAIR(symbol.terrainColor));
            wprintw(window, "[ ");
            wattroff(window, COLOR_PAIR(symbol.terrainColor));

            if (tileIndex == cursorY * width + cursorX) {
                wattron(window, COLOR_PAIR(symbol.terrainColor));
                wprintw(window, "%c", symbol.pieceSymbol);
                wattroff(window, COLOR_PAIR(symbol.terrainColor));
            } else {
                if (tileIndex == cursorY * width + cursorX) wattroff(window, A_REVERSE);
                wattron(window, COLOR_PAIR(symbol.pieceColor));
                wprintw(window, "%c", symbol.pieceSymbol);
                wattroff(window, COLOR_PAIR(symbol.pieceColor));
                if (tileIndex == cursorY * width + cursorX) wattron(window, A_REVERSE);
            }

            wattron(window, COLOR_PAIR(symbol.terrainColor));
            wprintw(window, "]"); wattroff(window, COLOR_PAIR(symbol.terrainColor));
            if (tileIndex == cursorY * width + cursorX) wattroff(window, A_REVERSE);

            tileIndex++;
        }
    }
    wrefresh(window);
}

void printValidTilesBoard(std::vector<Tile> &board, std::vector<Move> moves, int width, int height, WINDOW * window, int cursorX, int cursorY) {
    wclear(window);
    box(window, 0, 0);
    std::unordered_set<Tile *> moveSet(moves.size()); 
    std::unordered_set<Tile *> rangedSet(moves.size());
    for (int i = 0; i < moves.size(); i++) {
        if (moves[i].type == MoveType::Move) {
            moveSet.insert(moves[i].to);
        } else if (moves[i].type == MoveType::Shoot) {
            rangedSet.insert(moves[i].to);
        }
    }
    
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
            Symbol symbol;
            (moveSet.count(&board[tileIndex])) ? symbol = getValidTileSymbol(board[tileIndex]) : symbol = getSymbol(board[tileIndex]); // Changes background depending on if the move is in the valid tiles set
            if (tileIndex == cursorY * width + cursorX) wattron(window, A_REVERSE);
            if (board[tileIndex].terrain == TerrainType::Water) {
                wattron(window, COLOR_PAIR(symbol.terrainColor));
                wprintw(window, "[~~]");
                wattroff(window, COLOR_PAIR(symbol.terrainColor));
                if (tileIndex == cursorY * width + cursorX) wattroff(window, A_REVERSE);
                tileIndex++;
                continue;
            }
            wattron(window, COLOR_PAIR(symbol.terrainColor));
            (rangedSet.count(&board[tileIndex])) ? wprintw(window, "[x") : wprintw(window, "[ ");
            wattroff(window, COLOR_PAIR(symbol.terrainColor));

            if (tileIndex == cursorY * width + cursorX) {
                wattron(window, COLOR_PAIR(symbol.terrainColor));
                wprintw(window, "%c", symbol.pieceSymbol);
                wattroff(window, COLOR_PAIR(symbol.terrainColor));
            } else {
                if (tileIndex == cursorY * width + cursorX) wattroff(window, A_REVERSE);
                wattron(window, COLOR_PAIR(symbol.pieceColor));
                wprintw(window, "%c", symbol.pieceSymbol);
                wattroff(window, COLOR_PAIR(symbol.pieceColor));
                if (tileIndex == cursorY * width + cursorX) wattron(window, A_REVERSE);
            }

            wattron(window, COLOR_PAIR(symbol.terrainColor));
            wprintw(window, "]");
            wattroff(window, COLOR_PAIR(symbol.terrainColor));
            if (tileIndex == cursorY * width + cursorX) wattroff(window, A_REVERSE);

            tileIndex++;
        }
    }
    wrefresh(window);
}

// Functions for running test game instance
void setupGame(GameInstance &game, WINDOW * terminalWindow) {
    int winWidth = 4*(game.boardWidth+1) + 2;
    int winHeight = game.boardHeight + 1 + 2;
    WINDOW * boardWindow = createNewWindow(winHeight, winWidth, (LINES-winHeight)/4, (COLS-winWidth)/2);
    int cursorX = 0, cursorY = game.boardHeight-1;

    auto getUserInput = [&game, terminalWindow, boardWindow, &cursorX, &cursorY]() {
        printBoard(game.board, game.boardWidth, game.boardHeight, boardWindow, cursorX,  cursorY);
        int ch;       
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
            printBoard(game.board, game.boardWidth, game.boardHeight, boardWindow, cursorX, cursorY);
        }
        return cursorY * game.boardWidth + cursorX;
    };

    for (int i = 0; i < game.playerPieces.size(); i++) {
        Piece * piece = game.playerPieces[i].get();
        wprintw(terminalWindow, "Placing piece %d of %lu:\n", i+1, game.playerPieces.size());
        wprintw(terminalWindow, "    Piece type: %s\n", getPieceType(piece).c_str()); 
        wrefresh(terminalWindow);

        int coordInput = getUserInput();
        cursorX = game.board[coordInput].x;
        cursorY = game.board[coordInput].y;

        if (coordInput > game.board.size() || coordInput < 0) {
            wprintw(terminalWindow, "Error, selected (x, y) coordinate outside of board range.\n");
            wprintw(terminalWindow, "Failed to add piece to board, try again...\n");
            i--;
            continue;
        }

        if (coordInput < game.board.size() - game.boardWidth - 1) {
            wprintw(terminalWindow, "Error, selected (x, y) coordinate outside of player deployment zone.\n");
            wprintw(terminalWindow, "Failed to add piece to board, try again...\n");
            i--;
            continue;
        }
        
        if (int code = game.addPiece(piece, coordInput) == 1) {
            wprintw(terminalWindow, "Piece successfully placed at (%d, %d)\n", cursorX+1, cursorY+1);
        } else {
            i--;
            wprintw(terminalWindow, "Error code: %d\n", code);
            wprintw(terminalWindow, "Failed to add piece to board, try again...\n");
        }
        wrefresh(terminalWindow);
    }

    wprintw(terminalWindow, "Placing enemy pieces...\n");
    if (game.setupEnemy() == 1) wprintw(terminalWindow, "Enemy pieces successfully added.\n");
    wrefresh(terminalWindow);

    printBoard(game.board, game.boardWidth, game.boardHeight, boardWindow, 300, 300);
    wprintw(terminalWindow, "Game set up, press enter to begin game...");
    wgetch(terminalWindow);
    wrefresh(terminalWindow);
    destroyWindow(boardWindow);
}

void runGame(GameInstance &game, bool startingPlayer, WINDOW * terminalWindow) {
    wprintw(terminalWindow, "Starting Game\n");
    (startingPlayer) ? game.turnCount = 1 : game.turnCount = 2;
    wprintw(terminalWindow, "Starting Player: %s\n", (startingPlayer) ? "player" : "enemy");
    wrefresh(terminalWindow);
    int winWidth = 4*(game.boardWidth+1) + 2;
    int winHeight = game.boardHeight + 1 + 2;
    WINDOW * boardWindow = createNewWindow(winHeight, winWidth, (LINES-winHeight)/4, (COLS-winWidth)/2);
    int cursorX = 0, cursorY = game.boardHeight-1;
    int gameStatus = 0; // 0: redo move
                        // 1: go onto next turn
                        // 2: player wins
                        // 3: enemy wins
                        // 4: quit

    auto getUserInput = [&game, terminalWindow, boardWindow, &cursorX, &cursorY](bool valid, Piece * piece) {
        int ch = 0;      
        while (ch != '\n' && ch != 27) { 
            (valid) ? printValidTilesBoard(game.board, game.getValidMoves(*piece, MoveType::Any), game.boardWidth, game.boardHeight, boardWindow, cursorX, cursorY) : printBoard(game.board, game.boardWidth, game.boardHeight, boardWindow, cursorX,  cursorY);
            ch = wgetch(terminalWindow);
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
        } 

        if (ch == 27) return -1;
        return cursorY * game.boardWidth + cursorX;
    };

    while (!(gameStatus == 4 || gameStatus == 3 || gameStatus == 2)) {
        wprintw(terminalWindow, "Turn %d\n", game.turnCount);
        wrefresh(terminalWindow);

        int cursorX = 0, cursorY = game.boardHeight-1;
        if (game.turnCount % 2 == 1) {
            wprintw(terminalWindow, "Starting player turn...\n");
            wprintw(terminalWindow, "Select a piece to move (esc to quit)...\n");

            Move selectedMove;
            bool moveSelected = false;
            int coordInput = getUserInput(false, nullptr);
            wrefresh(terminalWindow);

            if (coordInput == -1) {
                wprintw(terminalWindow, "Quitting...");
                wrefresh(terminalWindow);
                gameStatus = 4;
                break;
            }

            if (game.board[coordInput].occupyingPiece && game.board[coordInput].occupyingPiece->ownedByPlayer) {
                Piece * piece = game.board[coordInput].occupyingPiece;
                wprintw(terminalWindow, "Select a piece...\n");
                wrefresh(terminalWindow);
                coordInput = getUserInput(true, piece);
                if (piece->hasRangedAttack) {
                    wprintw(terminalWindow, "Press s to shoot or m to move: ");
                    wrefresh(terminalWindow);
                    int ch = wgetch(terminalWindow);
                    if (ch == 's') {
                        selectedMove = Move{MoveType::Shoot, game.getPieceTile(*piece), &game.board[coordInput]};
                        moveSelected = true;
                    } else if (ch == 'm') {
                        selectedMove = Move{MoveType::Move, game.getPieceTile(*piece), &game.board[coordInput]};
                        moveSelected = true;
                    }
                } else {
                    selectedMove = Move{MoveType::Move, game.getPieceTile(*piece), &game.board[coordInput]};
                    moveSelected = true;
                }
            }
            gameStatus = (moveSelected) ? game.takePlayerTurn(selectedMove) : 0;
        } else {
            wprintw(terminalWindow, "Enemy taking turn...\n");
            wrefresh(terminalWindow);
            gameStatus = game.takeEnemyTurn();
        }

        if (gameStatus == 0) game.turnCount--;
        game.turnCount++;
    }   
    
    if (gameStatus == 2) {
        wprintw(terminalWindow, "Player Wins!");
    } else {
        wprintw(terminalWindow, "Enemy Wins.");
    }

    wrefresh(terminalWindow);
    destroyWindow(boardWindow);
}
