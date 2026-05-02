#pragma once

#include "tiles.hpp"
#include "gameinstance.hpp"

#include <ncurses.h>
#include <unordered_set>

// Functions to control ncurses windows

struct GameInterface;
struct MoveHash;

struct Panel {
    Panel(int height, int width, int startx, int starty);
    virtual ~Panel();

    WINDOW * window;
    int height, width, x, y;

    virtual void handleInput(int ch, GameInterface &interface) = 0;
    virtual void draw(const GameInterface &interface) = 0;
};

struct BoardPanel : Panel {
    BoardPanel(int h, int w, int x, int y) : Panel(h, w, x, y) {}

    GameInstance * game;
    int cursorX, cursorY;
    std::unordered_set<Move, MoveHash> validMoves;

    void handleInput(int ch, GameInterface &interface) override;
    void handleSetup(int ch, GameInterface &interface, Piece * piece);
    void draw(const GameInterface &interface) override;
};

struct InfoPanel : Panel {
    InfoPanel(int h, int w, int x, int y) : Panel(h, w, x, y) {}

    void handleInput(int ch, GameInterface &interface) override;
    void draw(const GameInterface &interface) override;
};

struct InputPanel : Panel {
    InputPanel(int h, int w, int x, int y) : Panel(h, w, x, y) {}

    std::vector<MoveType> activeOptions;
    int selected = 1;

    void handleInput(int ch, GameInterface &interface) override;
    void draw(const GameInterface &interface) override;
    void setupDraw(const GameInterface &interface, Piece * piece);
};

void initColors();
