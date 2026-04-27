#pragma once

#include "panels.hpp"
#include "types.hpp"
#include <memory>

struct GameInterface {
    GameInterface(GameInstance * game);
    bool setupComplete;

    Panel * activePanel; 
    MoveType moveChoice;
    Tile * tileChoice;

    std::unique_ptr<BoardPanel> board;
    std::unique_ptr<InfoPanel> info;
    std::unique_ptr<InputPanel> input;

    void initColors();
    void setup();
    void run();
    void getInput();
    void renderPanels();
};
