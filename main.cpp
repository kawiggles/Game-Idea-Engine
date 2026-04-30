#include "panels.hpp"
#include "pieces.hpp"
#include "tiles.hpp"
#include "gameinstance.hpp"
#include "types.hpp"
#include "logs.hpp"
#include "interface.hpp"

#include <assert.h>
#include <random>
#include <ncurses.h>
#include <memory>

int main() {
    initLogger();
    initscr();
    keypad(stdscr, TRUE);
    noecho();
    cbreak();

    // TODO: change how game seeds are collected
    int terminalHeight = LINES/2;
    int terminalStartY = LINES - terminalHeight;
    WINDOW * terminalBorder = newwin(terminalHeight, COLS, terminalStartY, 0);
    mvwhline(terminalBorder, 0, 0, ACS_HLINE, COLS);
    WINDOW * terminalWindow = derwin(terminalBorder, terminalHeight-1, COLS, 1, 0);
    keypad(terminalWindow, TRUE);
    scrollok(terminalWindow, TRUE);
    wrefresh(terminalBorder);
    wrefresh(terminalWindow);

    wmove(terminalWindow, 1, 0);
    wprintw(terminalWindow, "Starting Chess 2...\n");
    unsigned int testSeed;
    std::string seedInput = "";
    wprintw(terminalWindow, "Enter seed (leave blank for random): ");
    wrefresh(terminalWindow);
    int ch;
    while ((ch = wgetch(terminalWindow)) != '\n') {
        if (ch == KEY_BACKSPACE || ch == 263) {
            if (!seedInput.empty()) {
                seedInput.pop_back();
                int y, x;
                getyx(terminalWindow, y, x);
                mvwprintw(terminalWindow, y, x-1, " ");
                wmove(terminalWindow, y, x-1);
                wrefresh(terminalWindow);
            }
        } else if (ch >= '0' && ch <= '9') {
            seedInput += (char)ch;
            waddch(terminalWindow, ch);
            wrefresh(terminalWindow);
        }
    }

    if (seedInput.empty()) {
        std::random_device rd;
        testSeed = rd();
        wprintw(terminalWindow, "\nSeed: %u\n", testSeed);
        log("Seed: %u", testSeed);
    } else {
        try {
            testSeed = stoul(seedInput);
            wprintw(terminalWindow, "\nSeed: %u\n", testSeed);
            log("Seed: %u", testSeed);
        } catch (...) {
            std::random_device rd;
            testSeed = rd();
            wprintw(terminalWindow, "\nSeed: %u\n", testSeed);
            log("Seed: %u", testSeed);
        }
    }

    std::mt19937 gen(testSeed);
    std::uniform_int_distribution<unsigned long> distribution;
    unsigned long gameSeed = distribution(gen);
    wprintw(terminalWindow, "Game seed: %lu\n", gameSeed);
    log("Game seed: %lu\n", gameSeed);

    // Create arrays of test of pieces
    std::vector<std::unique_ptr<Piece>> testPlayerPieces;
    testPlayerPieces.push_back(std::make_unique<Piece>(PieceMaterial::Wood, PieceType::Light, Player::Human));
    testPlayerPieces.push_back(std::make_unique<Piece>(PieceMaterial::Wood, PieceType::Catapult, Player::Human));
    testPlayerPieces.push_back(std::make_unique<Piece>(PieceMaterial::Wood, PieceType::Elite, Player::Human));
    testPlayerPieces.push_back(std::make_unique<Piece>(PieceMaterial::Stone, PieceType::HCavalry, Player::Human));
    testPlayerPieces.push_back(std::make_unique<Piece>(PieceMaterial::Stone, PieceType::LCavalry, Player::Human));
    log("Player pieces created...");
    
    std::vector<std::unique_ptr<Piece>> testEnemyPieces;
    testEnemyPieces.push_back(std::make_unique<Piece>(PieceMaterial::Wood, PieceType::Light, Player::CPU));
    testEnemyPieces.push_back(std::make_unique<Piece>(PieceMaterial::Wood, PieceType::Light, Player::CPU));
    testEnemyPieces.push_back(std::make_unique<Piece>(PieceMaterial::Wood, PieceType::Archer, Player::CPU));
    testEnemyPieces.push_back(std::make_unique<Piece>(PieceMaterial::Wood, PieceType::Light, Player::CPU));
    testEnemyPieces.push_back(std::make_unique<Piece>(PieceMaterial::Wood, PieceType::Archer, Player::CPU));
    log("Enemy pieces created...");
    
    log("Defining game instance attributes...");
    std::uniform_int_distribution<int> biomeDist(0, 4);
    BiomeType randomBiome = static_cast<BiomeType>(biomeDist(gen));
    log("\tBiome: %s", getBiomeType(randomBiome).c_str());

    std::uniform_int_distribution<int> octaveDist(1, 3);
    int randomOctave = octaveDist(gen);
    log("\tPerlin noise octave: %d", randomOctave);

    std::uniform_int_distribution<int> roadDist(0, 1);
    bool roadBool = roadDist(gen);
    if (roadBool) log("\tBoard has road");

    wprintw(terminalWindow, "Press Enter to generate game instance...\n");
    wgetch(terminalWindow);
    wclear(terminalBorder);
    wrefresh(terminalBorder);
    delwin(terminalBorder);
    wclear(terminalWindow);
    wrefresh(terminalWindow);
    delwin(terminalWindow);

    log("Game Instance made, setting up game");
    GameInstance testGame(gameSeed, randomBiome, MissionType::HoldThePoint, randomOctave, roadBool);
    testGame.makeGame(std::move(testPlayerPieces), std::move(testEnemyPieces));
    
    GameInterface testInterface(&testGame);
    testInterface.initColors();
    testInterface.setup();
    testInterface.run();

    endwin();
    
    switch (testGame.status) {
        case GameInstance::Status::PlayerWin:
            printf("Player wins!");
            break;
        case GameInstance::Status::EnemyWin:
            printf("Enemy wins");
            break;
        case GameInstance::Status::Quit:
            printf("Quitting...");
            break;
        default:
            printf("Something has gone wrong Kawika, fix it");
    }

    closeLogger();
    return 0;
}
