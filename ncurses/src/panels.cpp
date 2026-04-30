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

BoardPanel::Symbol BoardPanel::getSymbol(const Tile &tile, bool v) {
    Symbol symbol;

    switch (tile.terrain) {
        case TerrainType::Water:    symbol.terrainColor = v ? 11 : 1; break;
        case TerrainType::Field:    symbol.terrainColor = v ? 12 : 2; break;
        case TerrainType::Forest:   symbol.terrainColor = v ? 13 : 3; break;
        case TerrainType::Mountain: symbol.terrainColor = v ? 14 : 4; break;
        case TerrainType::Road:     symbol.terrainColor = v ? 15 : 5; break;
        case TerrainType::Desert:   symbol.terrainColor = v ? 16 : 6; break;
        case TerrainType::Jungle:   symbol.terrainColor = v ? 17 : 7; break;
        case TerrainType::Peak:     symbol.terrainColor = v ? 18 : 8; break;
        // Arctic Tiles TODO: Actually write inverse colors for arctic tiles
        case TerrainType::IceField: symbol.terrainColor = 31; break;
        case TerrainType::SnowField:symbol.terrainColor = 32; break;
        case TerrainType::Tundra:   symbol.terrainColor = 33; break;
        // Mission Tiles
        case TerrainType::Objective:symbol.terrainColor = v ? 19 : 9; break;
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

        if (tile.occupyingPiece->owner == Player::Human)
            symbol.pieceColor = v ? 23 : 21;
        else
            symbol.pieceColor = v ? 24 : 22;
    } else {
        symbol.pieceColor = symbol.terrainColor;
    }

    return symbol;
}

void BoardPanel::draw(const GameInterface &interface) {
    log("Drawing board panel");
    // TODO: find a better way to check if a tile is valid than this
    auto tileInMoves = [this] (Tile * tile) {
        for (Move m : validMoves)
            if (m.to == tile)
                return true;
        return false;
    };

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
            Symbol symbol = getSymbol(*tile, !validMoves.empty() && tileInMoves(tile));
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
    log("Running handleInput with BoardPanel");
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
                game->status = game->executeMove(Move{
                        interface.moveChoice,
                        interface.tileChoice,
                        toChoice
                        }); // ugh
                validMoves.clear();
                interface.moveChoice = MoveType::Null;
                interface.tileChoice = nullptr;
                ASSERT(game->status == GameInstance::Status::Next);
                log("Taking enemy turn");
                game->status = game->takeEnemyTurn();
            }
            break;
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

    mvwprintw(window, 1, 1, interface.setupComplete ? "Turn: %d" : "Setup:", (interface.board->game->turnCount / 2) + 1);
    wmove(window, 2, 1);
    for (int i = 0; i < w - 2; i++) wprintw(window, "-");

    mvwprintw(window, 3, 1, "Terrain: ");
    mvwprintw(window, 4, 1, "\tType: %s", getTerrainType(tile->terrain).c_str());
    mvwprintw(window, 5, 1, "\tCoordinates: (%d, %d)", tile->x+1, tile->y+1);
    if (tile->occupyingPiece) {
        const Piece piece = *tile->occupyingPiece;
        mvwprintw(window, 3, w/2, "|Piece: ");
        mvwprintw(window, 4, w/2, "|\tOwner: ");
        wprintw(window, "%s", (piece.owner == Player::Human) ? "Player" : "Enemy");
        mvwprintw(window, 5, w/2, "|\tType: ");
        wprintw(window, "%s", getPieceType(piece).c_str());
    } 
    
    wrefresh(window);
};

void InfoPanel::handleInput(int ch, GameInterface &interface) { return; }

void InputPanel::draw(const GameInterface &interface) {
    log("Drawing InputPanel");
    wclear(window);
    box(window, 0, 0);
    int w = getmaxx(window);
    std::string prompt;

    if (interface.activePanel == this) {
        prompt = "Select a move type:";
        mvwprintw(window, 1, (getmaxx(window) - prompt.length()) / 2, "%s", prompt.c_str());
        const Piece piece = *interface.tileChoice->occupyingPiece;
        int s = activeOptions.size();
        ASSERT(s > 0);
        for (int i = 0; i < s; i++) {
            if (selected == i + 1) wattron(window, A_REVERSE);
            mvwprintw(window, (getmaxy(window) / 2) - 1, (getmaxx(window) * i / (s + 1)) + (getmaxx(window) / (s + 1)), "%s", getMoveType(activeOptions[i]).c_str());
            if (selected == i + 1) wattroff(window, A_REVERSE);
        }

    } else if (!interface.setupComplete) {
        prompt = "Place next piece:";
        mvwprintw(window, 1, (getmaxx(window) - prompt.length()) / 2, "%s", prompt.c_str());
    } else if (interface.moveChoice == MoveType::Null) {
       prompt = "Select a piece to move:"; 
       mvwprintw(window, 1, (getmaxx(window) - prompt.length()) / 2, "%s", prompt.c_str());
    } else {
        prompt = "Select a destination tile:";
        mvwprintw(window, 1, (getmaxx(window) - prompt.length()) / 2, "%s", prompt.c_str());
    }

    wrefresh(window);
}

void InputPanel::handleInput(int ch, GameInterface &interface) {
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
                interface.board->validMoves.clear();
                interface.moveChoice = MoveType::Null;
                interface.tileChoice = nullptr;
                ASSERT(interface.board->game->status == GameInstance::Status::PlayerWin);
                log("Taking enemy turn");
                interface.board->game->status = interface.board->game->takeEnemyTurn();
            } else {
                interface.activePanel = interface.board.get();
                interface.board->validMoves = interface.board->game->getValidMoves(*interface.tileChoice, interface.moveChoice);
            }
            activeOptions.clear();
            break;
    }
}
