#include "printboards.hpp"
#include "pieces.hpp"
#include "tiles.hpp"
#include "gameinstance.hpp"
#include "types.hpp"

#include <assert.h>
#include <random>
#include <ncurses.h>

int main() {
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    initColors();

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
    } else {
        try {
            testSeed = stoul(seedInput);
            wprintw(terminalWindow, "\nSeed: %u\n", testSeed);
        } catch (...) {
            std::random_device rd;
            testSeed = rd();
            wprintw(terminalWindow, "\nSeed: %u\n", testSeed);
        }
    }

    std::mt19937 gen(testSeed);
    std::uniform_int_distribution<unsigned long> distribution;
    unsigned long gameSeed = distribution(gen);
    wprintw(terminalWindow, "Game seed: %lu\n", gameSeed);

    // Create arrays of test of pieces
    std::vector<Piece *> testPlayerPieces;
    testPlayerPieces.push_back(new Piece(PieceMaterial::Wood, PieceType::Light, true));
    testPlayerPieces.push_back(new Piece(PieceMaterial::Wood, PieceType::Light, true));
    testPlayerPieces.push_back(new Piece(PieceMaterial::Wood, PieceType::Elite, true));
    testPlayerPieces.push_back(new Piece(PieceMaterial::Stone, PieceType::LCavalry, true));
    testPlayerPieces.push_back(new Piece(PieceMaterial::Stone, PieceType::LCavalry, true));
    
    
    std::vector<Piece *> testEnemyPieces;
    testEnemyPieces.push_back(new Piece(PieceMaterial::Wood, PieceType::Light, false));
    testEnemyPieces.push_back(new Piece(PieceMaterial::Wood, PieceType::Shield, false));
    testEnemyPieces.push_back(new Piece(PieceMaterial::Wood, PieceType::Elite, false));
    testEnemyPieces.push_back(new Piece(PieceMaterial::Wood, PieceType::Light, false));
    testEnemyPieces.push_back(new Piece(PieceMaterial::Wood, PieceType::Shield, false));
    
    // Make a new game instance
    std::uniform_int_distribution<int> biomeDist(0, 4);
    BiomeType randomBiome = static_cast<BiomeType>(biomeDist(gen));
    wprintw(terminalWindow, "Biome: %s\n", getBiomeType(randomBiome).c_str());

    std::uniform_int_distribution<int> octaveDist(1, 3);
    int randomOctave = octaveDist(gen);
    wprintw(terminalWindow, "Perlin noise octave: %d\n", randomOctave);

    std::uniform_int_distribution<int> roadDist(0, 1);
    bool roadBool = roadDist(gen);
    if (roadBool) wprintw(terminalWindow, "Board has road\n");

    wprintw(terminalWindow, "Press Enter to generate game instance...\n");
    wgetch(terminalWindow);
    wrefresh(terminalWindow);

    GameInstance testGame(gameSeed, randomBiome, MissionType::HoldThePoint, randomOctave, roadBool);
    wprintw(terminalWindow, "Game Instance intitalized\n");
    wrefresh(terminalWindow);
    testGame.makeGame(testPlayerPieces, testEnemyPieces, terminalWindow);
    wprintw(terminalWindow, "Game Instance made, setting up game\n");
    wrefresh(terminalWindow);
    setupGame(testGame, terminalWindow);
    runGame(testGame, true, terminalWindow);

    wprintw(terminalWindow, "\nGame exiting...");
    wgetch(terminalWindow);

    destroyWindow(terminalWindow);
    destroyWindow(terminalBorder);
    endwin();
    return 0;
}
