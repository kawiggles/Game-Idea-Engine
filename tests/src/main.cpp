#include "printboards.hpp"
#include "pieces.hpp"
#include "tiles.hpp"
#include "gameinstance.hpp"
#include "types.hpp"

#include <iostream>
#include <assert.h>
#include <random>

int main() {
    std::cout << "Test Begin..." << std::endl;
    int testSeed;
    std::string seedInput;
    std::cout << "Enter seed (leave blank for random): ";
    std::getline(std::cin, seedInput);

    if (seedInput.empty()) {
        std::random_device rd;
        testSeed = rd();
        std::cout << "Seed: " << testSeed << std::endl;
    } else {
        testSeed = stoul(seedInput);
    }
    

    std::mt19937 gen(testSeed);
    std::uniform_int_distribution<unsigned long> distribution;
    unsigned long gameSeed = distribution(gen);
    std::cout << "Game seed: " << gameSeed << std::endl;

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
    std::cout << "Biome: " << getBiomeType(randomBiome) << std::endl;

    std::uniform_int_distribution<int> octaveDist(1, 3);
    int randomOctave = octaveDist(gen);
    std::cout << "Perlin noise octave: " << randomOctave << std::endl;

    std::uniform_int_distribution<int> roadDist(0, 1);
    bool roadBool = roadDist(gen);
    if (roadBool) std::cout << "Board has road" << std::endl;

    GameInstance testGame(gameSeed, randomBiome, MissionType::HoldThePoint, randomOctave, roadBool);
    testGame.makeGame(testPlayerPieces, testEnemyPieces);
    setupGame(testGame);
    runGame(testGame, true);
    // Moving pieces and reprinting board:
    return 0;
}
