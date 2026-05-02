#include "panels.hpp"
#include "interface.hpp"
#include "logs.hpp"

#include <ncurses.h>

GameInterface::GameInterface(GameInstance * game) {
    int bH = game->boardHeight + 3;
    int bW = 4 * (game->boardWidth + 1) + 2;
    board = std::make_unique<BoardPanel>(bH, bW, (LINES - bH) / 3, (COLS - bW) / 2);
    board->game = game;
    board->cursorX = 0;
    board->cursorY = game->boardHeight - 1;
    input = std::make_unique<InputPanel>(8, COLS / 2, ((LINES - bH) / 3) + bH, COLS / 4);
    info = std::make_unique<InfoPanel>(LINES / 4, COLS, LINES * 3 / 4, 0);

    activePanel = board.get();
    moveChoice = MoveType::Null;
    tileChoice = nullptr;
    board->validMoves.clear();
}

void GameInterface::initColors() {
    start_color();
    can_change_color();
    init_pair(99, COLOR_WHITE, COLOR_BLACK);
    wbkgd(stdscr, COLOR_PAIR(1));
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

GameInterface::Symbol GameInterface::getSymbol(const Tile &tile, bool v) const {
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

void GameInterface::setup() {
    log("Running game setup through user interface");

    while (setupIterator < board->game->playerPieces.size()) {
        Piece * next = board->game->playerPieces.at(setupIterator).get();
        board->draw(*this);
        info->draw(*this);
        input->setupDraw(*this, next);
        int ch = getch();
        board->handleSetup(ch, *this, next);;
    }

    log("Player setup complete");
    board->game->setupEnemy();
    setupIterator = -1;
}

void GameInterface::run() {
    while (board->game->status == GameInstance::Status::Redo || board->game->status == GameInstance::Status::Next) {
        board->draw(*this);
        info->draw(*this);
        input->draw(*this);
        getInput();
    }
};

void GameInterface::getInput() {
    int ch = getch();
    if (ch == 'q') board->game->status = GameInstance::Status::Quit;
    activePanel->handleInput(ch, *this);
}
