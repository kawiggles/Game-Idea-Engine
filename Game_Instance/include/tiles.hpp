#pragma once

#include "types.hpp"

#include <vector>
#include <unordered_map>
#include <memory>
#include <ncurses.h>

// See tiles.cpp for how tiles are constructed on a board for a Game Instance

// Structures
struct Piece; // Forward declaration of Piece struct, which is in pieces.hpp

struct Tile {
    int x, y;
    Piece * occupyingPiece = nullptr; // Pointer to the piece currently on this tile, if any
    TerrainType terrain = TerrainType::Field;
    virtual ~Tile() = default;

    virtual int getMovementMod(const Piece &piece) { return 0; }
    virtual int getRoadMovementMod(const Piece &piece, Tile * nextTile) { return 0; }
    virtual int getToughnessMod() { return 0; }
    virtual int getStrengthMod(const Piece &piece) { return 0; }
    virtual int getRangedStrengthMod(const Piece &piece) { return 0; }
    virtual int getRangeMaxMod() { return 0; }
};

struct Water : Tile {
    int x, y;
    Water(int x, int y) : x(x), y(y) {};
    ~Water() override;
    TerrainType terrain = TerrainType::Water;
    int getMovementMod(const Piece &piece) override;
};

struct Field : Tile { 
    int x, y;
    Field(int x, int y) : x(x), y(y) {};
    ~Field() override;
    TerrainType terrain = TerrainType::Field;
    int getStrengthMod(const Piece &piece) override;
};

struct Forest : Tile {
    int x, y;
    Forest(int x, int y) : x(x), y(y) {};
    ~Forest() override;
    TerrainType terrain = TerrainType::Forest;
    int getMovementMod(const Piece &piece) override;
    int getToughnessMod() override;
    int getStrengthMod(const Piece &piece) override;
    int getRangeMaxMod() override;
};

struct Mountain : Tile {
    int x, y;
    Mountain(int x, int y) : x(x), y(y) {};
    ~Mountain() override;
    TerrainType terrain = TerrainType::Mountain;
    int getMovementMod(const Piece &piece) override;
    int getToughnessMod() override;
    int getRangedStrengthMod(const Piece &piece) override;
    int getRangeMaxMod() override;
};

struct Road : Tile {
    int x, y;
    Road(int x, int y) : x(x), y(y) {};
    ~Road() override;
    TerrainType terrain = TerrainType::Road;
    int getRoadMovementMod(const Piece &piece, Tile * nextTile) override;
    int getStrengthMod(const Piece &piece) override;
};

struct Desert : Tile {
    int x, y;
    Desert(int x, int y) : x(x), y(y) {};
    ~Desert() override;
    TerrainType terrain = TerrainType::Desert;
    int getMovementMod(const Piece &piece) override;
    int getToughnessMod() override;
    int getStrengthMod(const Piece &piece) override;
};

struct Jungle : Tile {
    int x, y;
    Jungle(int x, int y) : x(x), y(y) {};
    ~Jungle() override;
    TerrainType terrain = TerrainType::Jungle;
    int getMovementMod(const Piece &piece) override;
    int getToughnessMod() override;
    int getStrengthMod(const Piece &piece) override;
    int getRangedStrengthMod(const Piece &piece) override;
    int getRangeMaxMod() override;
};

struct Peak : Tile {
    int x, y;
    Peak(int x, int y) : x(x), y(y) {};
    ~Peak() override;
    TerrainType terrain = TerrainType::Peak;
    int getMovementMod(const Piece &piece) override;
    int getToughnessMod() override;
    int getRangedStrengthMod(const Piece &piece) override;
    int getRangeMaxMod() override;
};

struct IceField : Tile {
    int x, y;
    IceField(int x, int y) : x(x), y(y) {};
    ~IceField() override;
    TerrainType terrain = TerrainType::IceField;
};

struct SnowField : Tile {
    int x, y;
    SnowField(int x, int y) : x(x), y(y) {};
    ~SnowField() override;
    TerrainType terrain = TerrainType::SnowField;
    int getMovementMod(const Piece &piece) override;
};

struct Tundra : Tile {
    int x, y;
    Tundra(int x, int y) : x(x), y(y) {};
    ~Tundra() override;
    TerrainType terrain = TerrainType::Tundra;
};

struct Objective : Tile {
    int x, y;
    Objective(int x, int y) : x(x), y(y) {};
    ~Objective() override;
    TerrainType terrain = TerrainType::Objective;
    int getToughnessMod() override;
};


// Functions:

// Function to randomly generate a terrain type for a tile, will be updated later for perlin noise
std::unique_ptr<Tile> makeRandomTerrain(float noise, BiomeType biome, int x, int y);

// Function to generate a road on a board.
std::vector<int> generateRoad(Tile * startTile, Tile * endTile, std::unordered_map<int, std::unique_ptr<Tile>> &board, int width, int height, WINDOW * window);
