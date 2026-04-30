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
    info = std::make_unique<InfoPanel>(7, COLS / 4 * 3, ((LINES - bH) / 3) + bH, (COLS - bW) / 4);
    input = std::make_unique<InputPanel>(LINES / 4, COLS, LINES * 3 / 4, 0);

    activePanel = board.get();
    setupComplete = false;
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

void GameInterface::setup() {
    log("Running game setup through user interface");

    for (int i = 0; i < board->game->playerPieces.size();) {
        renderPanels();
        int ch = getch();
        switch (ch) {
            case KEY_UP: case 119:
                if (board->cursorY < board->game->boardHeight - 1) board->cursorY++; break;
            case KEY_DOWN: case 115:
                if (board->cursorY > 0) board->cursorY--; break;
            case KEY_LEFT: case 97:
                if (board->cursorX > 0) board->cursorX--; break;
            case KEY_RIGHT: case 100:
                if (board->cursorX < board->game->boardWidth - 1) board->cursorX++; break;
            case '\n': case KEY_ENTER:
                if (!board->game->board.at(board->cursorY * board->game->boardWidth + board->cursorX)->occupyingPiece) {
                    board->game->addPiece(board->game->playerPieces[i].get(),
                        board->cursorY * board->game->boardWidth + board->cursorX);
                    i++;
                }
                break;
        }
    }
    log("Player setup complete");
    board->game->setupEnemy();
    setupComplete = true;
}

void GameInterface::run() {
    while (board->game->status == GameInstance::Status::Redo || board->game->status == GameInstance::Status::Next) {
        renderPanels();
        getInput();
    }
};

void GameInterface::getInput() {
    int ch = getch();
    if (ch == 'q') board->game->status = GameInstance::Status::Quit;
    activePanel->handleInput(ch, *this);
}

void GameInterface::renderPanels() {
    board->draw(*this);
    info->draw(*this);
    input->draw(*this);
};
