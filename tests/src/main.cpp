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
    start_color();

    printw("Starting Chess 2...\n");
    unsigned int testSeed;
    std::string seedInput = "";
    printw("Enter seed (leave blank for random): ");
    refresh();
    int ch;
    while ((ch = getch()) != '\n') {
        if (ch == KEY_BACKSPACE || ch == 263) {
            if (!seedInput.empty()) {
                seedInput.pop_back();
                int y, x;
                getyx(stdscr, y, x);
                mvprintw(y, x-1, " ");
                move(y, x-1);
                refresh();
            }
        } else if (ch >= '0' && ch <= '9') {
            seedInput += (char)ch;
            addch(ch);
            refresh();
        }
    }

    if (seedInput.empty()) {
        std::random_device rd;
        testSeed = rd();
        printw("Seed: %u\n", testSeed);
    } else {
        try {
            testSeed = stoul(seedInput);
        } catch (...) {
            std::random_device rd;
            testSeed = rd();
            printw("Seed: %u\n", testSeed);
        }
    }

    std::mt19937 gen(testSeed);
    std::uniform_int_distribution<unsigned int> distribution;
    unsigned int gameSeed = distribution(gen);
    printw("Game seed: %u\n", gameSeed);

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
    printw("Biome: %s\n", getBiomeType(randomBiome).c_str());

    std::uniform_int_distribution<int> octaveDist(1, 3);
    int randomOctave = octaveDist(gen);
    printw("Perlin noise octave: %d\n", randomOctave);

    std::uniform_int_distribution<int> roadDist(0, 1);
    bool roadBool = roadDist(gen);
    if (roadBool) printw("Board has road\n");

    printw("Press Enter to generate game instance...\n");
    getch();
    refresh();

    GameInstance testGame(gameSeed, randomBiome, MissionType::HoldThePoint, randomOctave, roadBool);
    testGame.makeGame(testPlayerPieces, testEnemyPieces);
    setupGame(testGame);
    runGame(testGame, true);

    endwin();
    return 0;
}
