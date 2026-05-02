#include "panels.hpp"
#include "interface.hpp"
#include "logs.hpp"
#include "tiles.hpp"
#include "gameinstance.hpp"

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

Panel::Panel(int height, int width, int startx, int starty)
    : height(height), width(width), x(startx), y(starty) {
    
    window = newwin(height, width, x, y);
    box(window, 0, 0);
    wrefresh(window);
    curs_set(0);
}

Panel::~Panel() {
    wborder(window, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
    wclear(window);
    wrefresh(window);
    delwin(window);
}

void BoardPanel::draw(const GameInterface &interface) {
    log("Drawing board panel");
    std::unordered_set<Tile *> validToTiles;
    for (Move m : validMoves)
        validToTiles.insert(m.to);

    wclear(window);
    box(window, 0, 0);
    mvwprintw(window, 1, 1, "    "); // Four spaces at start of coordinate numbering

    int h = game->boardHeight;
    int w = game->boardWidth;

    for (int i = 0; i < w; i++) { // Columns
        if (i < 9)
            wprintw(window, "[ %d]", i + 1);
        else
            wprintw(window, "[%d]", i + 1);
    }

    int tileIndex = 0;
    int cursorPos = cursorY * w + cursorX;
    for (int i = 0; i < h; i++) {
        wmove(window, h + 1 - i, 1);
        if (i < 9)
            wprintw(window, "[ %d]", i + 1);
        else
            wprintw(window, "[%d]", i + 1);

        for (int j = 0; j < w; j++) {
            Tile * tile = game->board.at(tileIndex).get();
            GameInterface::Symbol symbol = interface.getSymbol(*tile, !validMoves.empty() && validToTiles.count(tile));
            if (tileIndex == cursorPos)
                wattron(window, A_REVERSE);

            if (tile->terrain == TerrainType::Water) {
                wattron(window, COLOR_PAIR(symbol.terrainColor));
                wprintw(window, "[~~]");
                wattroff(window, COLOR_PAIR(symbol.terrainColor));
                if (tileIndex == cursorPos)
                    wattroff(window, A_REVERSE);
                tileIndex++;
                continue;
            }

            wattron(window, COLOR_PAIR(symbol.terrainColor));
            wprintw(window, "[ ");
            wattron(window, COLOR_PAIR(symbol.terrainColor));

            if (tileIndex == cursorPos)
                wattron(window, COLOR_PAIR(symbol.terrainColor));
            else
                wattron(window, COLOR_PAIR(symbol.pieceColor));
            wprintw(window, "%c", symbol.pieceSymbol);
            if (tileIndex == cursorPos)
                wattroff(window, COLOR_PAIR(symbol.terrainColor));
            else
                wattroff(window, COLOR_PAIR(symbol.pieceColor));

            wattron(window, COLOR_PAIR(symbol.terrainColor));
            wprintw(window, "]");
            wattroff(window, COLOR_PAIR(symbol.terrainColor));

            if (tileIndex == cursorPos)
                wattroff(window, A_REVERSE);

            tileIndex++;
        }
    }
    wrefresh(window);
};

void BoardPanel::handleInput(int ch, GameInterface &interface) {
    ASSERT(interface.activePanel == this);
    log("Running handleInput with BoardPanel, input is: %c", static_cast<char>(ch));
    switch(ch) {
        case 119: case KEY_UP:
            if (cursorY < game->boardHeight - 1) cursorY++; break;
        case 115: case KEY_DOWN:
            if (cursorY > 0) cursorY--; break;
        case 97: case KEY_LEFT:
            if (cursorX > 0) cursorX--; break;
        case 100: case KEY_RIGHT:
            if (cursorX < game->boardWidth - 1) cursorX++; break;
        case '\n': case KEY_ENTER: {
            if (interface.moveChoice == MoveType::Null) { // No move type selected -> select a piece to move
                Tile * choice = game->board.at(cursorY * game->boardWidth + cursorX).get();
                ASSERT(choice);
                log("Tile selected at (%d, %d)", choice->x+1, choice->y+1);
                if (!choice->occupyingPiece || choice->occupyingPiece->owner != Player::Human) break;
                log("Selected tile is valid");
                interface.tileChoice = choice;
                const Piece piece = *interface.tileChoice->occupyingPiece;
                if (game->getValidMoves(*interface.tileChoice, MoveType::Move).size() > 0) 
                    interface.input.get()->activeOptions.push_back(MoveType::Move);
                if (piece.hasRangedAttack && game->getValidMoves(*interface.tileChoice, MoveType::Shoot).size() > 0) 
                    interface.input.get()->activeOptions.push_back(MoveType::Shoot);
                if (interface.tileChoice->terrain == TerrainType::Objective) 
                    interface.input.get()->activeOptions.push_back(MoveType::Capture);

                if(interface.input.get()->activeOptions.size() > 1) {
                    interface.activePanel = interface.input.get();
                } else if (interface.input.get()->activeOptions.size() == 1) {
                    interface.moveChoice = interface.input.get()->activeOptions[0];
                    interface.input.get()->activeOptions.clear();
                    validMoves = game->getValidMoves(*choice, interface.moveChoice);
                } else {
                    game->status = GameInstance::Status::Redo;
                    interface.moveChoice = MoveType::Null;
                    interface.input.get()->activeOptions.clear();
                    validMoves.clear();
                    interface.tileChoice = nullptr;
                }
            } else { // Move type selected -> perform the move and then take the enemy turn
                ASSERT(interface.moveChoice != MoveType::Any && interface.moveChoice != MoveType::Null);
                Tile * toChoice = game->board.at(cursorY * game->boardWidth + cursorX).get();
                ASSERT(toChoice);
                log("Executing selected player move at tile (%d, %d)", toChoice->x+1, toChoice->y+1);
                Move move = Move{
                        interface.moveChoice,
                        interface.tileChoice,
                        toChoice
                        }; // ugh
                if (validMoves.count(move))
                    game->status = game->executeMove(move);
                else
                    game->status = GameInstance::Status::Redo;
                validMoves.clear();
                interface.moveChoice = MoveType::Null;
                interface.tileChoice = nullptr;
                if (game->status == GameInstance::Status::Next)
                    game->status = game->takeEnemyTurn();
            }
            break;
        }
    }
}

void BoardPanel::handleSetup(int ch, GameInterface &interface, Piece * piece) {
    log("Running handleSetup with input %c", static_cast<char>(ch));
    
    switch (ch) {
        case KEY_UP: case 119:
            if (cursorY < game->boardHeight - 1) cursorY++; break;
        case KEY_DOWN: case 115:
            if (cursorY > 0) cursorY--; break;
        case KEY_LEFT: case 97:
            if (cursorX > 0) cursorX--; break;
        case KEY_RIGHT: case 100:
            if (cursorX < game->boardWidth - 1) cursorX++; break;
        case '\n': case KEY_ENTER:
            if (game->board.at(cursorY * game->boardWidth + cursorX)->terrain != TerrainType::Water
                && !game->board.at(cursorY * game->boardWidth + cursorX)->occupyingPiece) {
                game->addPiece(piece, cursorY * game->boardWidth + cursorX);
                interface.setupIterator++;
            }
    } 
}

void InfoPanel::draw(const GameInterface &interface) {
    log("Drawing InfoPanel");
    wclear(window);
    box(window, 0, 0);
    int w = getmaxx(window);

    Tile * tile = interface.board->game->board.at(
            interface.board->cursorY * interface.board->game->boardWidth + interface.board->cursorX).get();
    GameInterface::Symbol symbol = interface.getSymbol(*tile, false);

    mvwprintw(window, 1, 1, (interface.setupIterator == -1) ? "Turn: %d" : "Setup:", (interface.board->game->turnCount / 2) + 1);
    wmove(window, 2, 1);
    for (int i = 0; i < w - 2; i++) wprintw(window, "-");

    mvwprintw(window, 3, 1, "Terrain: ");
    wattron(window, COLOR_PAIR(symbol.terrainColor));
    mvwprintw(window, 4, 1, "\tType: %s", getTerrainType(tile->terrain).c_str());
    wattroff(window, COLOR_PAIR(symbol.terrainColor));
    mvwprintw(window, 5, 1, "\tCoordinates: (%d, %d)", tile->x+1, tile->y+1);

    if (tile->occupyingPiece) {
        const Piece piece = *tile->occupyingPiece;
        mvwprintw(window, 3, w/2, "|Piece: ");
        mvwprintw(window, 4, w/2, "|\tOwner: ");
        wattron(window, COLOR_PAIR(symbol.pieceColor));
        wprintw(window, "%s", (piece.owner == Player::Human) ? "Player" : "Enemy");
        wattroff(window, COLOR_PAIR(symbol.pieceColor));
        mvwprintw(window, 5, w/2, "|\tType: ");
        wprintw(window, "%s", getPieceType(piece).c_str());
        mvwprintw(window, 6, w/2, "|\tMaterial: ");
        wprintw(window, "%s", getMaterialType(piece.material).c_str());
        mvwprintw(window, 7, w/2, "|\tStats: ");
        mvwprintw(window, 8, w/2, "|\t\tStrength: ");
        wprintw(window, "%d", piece.strength);
        mvwprintw(window, 9, w/2, "|\t\tToughness: ");
        wprintw(window, "%d", piece.toughness);
        if (piece.hasRangedAttack) {
            mvwprintw(window, 8, (w*3)/4, "Ranged Attack Range: ");
            wprintw(window, "%d/%d", piece.rangedAttack.minRange, piece.rangedAttack.maxRange);
            mvwprintw(window, 9, (w*3)/4, "Ranged Attack Strength: ");
            wprintw(window, "%d", piece.rangedAttack.strength);
        }
    } 
    
    wrefresh(window);
};

void InfoPanel::handleInput(int ch, GameInterface &interface) { return; }

void InputPanel::draw(const GameInterface &interface) {
    log("Drawing InputPanel");
    wclear(window);
    box(window, 0, 0);

    std::string prompt;
    auto xPos = [&]() {
        return (getmaxx(window) - prompt.length()) / 2;
    };

    if (interface.activePanel == this) {
        prompt = "Select a move type:";
        mvwprintw(window, 1, xPos(), "%s", prompt.c_str());
        const Piece piece = *interface.tileChoice->occupyingPiece;
        int s = activeOptions.size();
        ASSERT(s > 0);
        for (int i = 0; i < s; i++) {
            if (selected == i + 1) wattron(window, A_REVERSE);
            mvwprintw(window, (getmaxy(window) / 2) - 1, (getmaxx(window) * i / (s + 1)) + (getmaxx(window) / (s + 1)), "%s", getMoveType(activeOptions[i]).c_str());
            if (selected == i + 1) wattroff(window, A_REVERSE);
        }
    } else if (interface.moveChoice == MoveType::Null) {
       prompt = "Select a piece to move:"; 
       mvwprintw(window, 1, xPos(), "%s", prompt.c_str());
    } else {
        prompt = "Select a destination tile:";
        mvwprintw(window, 1, xPos(), "%s", prompt.c_str());
        prompt = "MoveType: ";
        prompt += getMoveType(interface.moveChoice);
        mvwprintw(window, 3, xPos(), "%s", prompt.c_str());
    }

    wrefresh(window);
}

void InputPanel::setupDraw(const GameInterface &interface, Piece * piece) {
    log("Drawing InputPanel for setup, on piece %d", interface.setupIterator);
    wclear(window);
    box(window, 0, 0);

    std::string prompt = "Placing piece " + std::to_string(interface.setupIterator + 1) + " of " + std::to_string(interface.board->game->playerPieces.size()) + ":";
    auto xPos = [&]() {
        return (getmaxx(window) - prompt.length()) / 2;
    };

    mvwprintw(window, 1, xPos(), "%s", prompt.c_str());
    prompt = "Piece Type: ";
    prompt += getPieceType(*piece).c_str();
    mvwprintw(window, 3, xPos(), "%s", prompt.c_str());
    prompt = "Remaining Pieces:";
    mvwprintw(window, 5, xPos(), "%s", prompt.c_str());
    if (interface.setupIterator + 1 == interface.board->game->playerPieces.size()) {
        prompt = "None";
    } else {
        prompt = "";
        for (int i = interface.setupIterator + 1; i < interface.board->game->playerPieces.size(); i++) {
            prompt += getPieceType(*interface.board->game->playerPieces.at(i).get()); 
            if (i != interface.board->game->playerPieces.size() - 1) prompt += " - ";
        }
    }
    mvwprintw(window, 6, xPos(), "%s", prompt.c_str());

    wrefresh(window);

}

void InputPanel::handleInput(int ch, GameInterface &interface) {
    log("Running handleInput with InputPanel, input is %c", static_cast<char>(ch));
    ASSERT(interface.activePanel == this);
    switch (ch) {
        case KEY_LEFT: case 97:
            if (selected == 1)
                selected = activeOptions.size();
            else selected--;
            break;
        case KEY_RIGHT: case 100:
            if (selected == activeOptions.size())
                selected = 1;
            else selected++;
            break;
        case '\n': case KEY_ENTER:
            interface.moveChoice = activeOptions[selected - 1];
            if (interface.moveChoice == MoveType::Capture) {
                log("Executing player capture objective capture at tile (%d, %d)",
                        interface.tileChoice->x+1, interface.tileChoice->y+1);
                interface.board->game->status = interface.board->game->executeMove(Move{
                        MoveType::Capture,
                        interface.tileChoice,
                        interface.tileChoice
                        }); 
                ASSERT(interface.board->game->status == GameInstance::Status::PlayerWin);
            } else {
                interface.activePanel = interface.board.get();
                interface.board->validMoves = interface.board->game->getValidMoves(*interface.tileChoice, interface.moveChoice);
            }
            activeOptions.clear();
            break;
    }
}
