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
    TerrainType terrain;

    Tile(int x, int y, TerrainType t) : x(x), y(y), terrain(t) {}
    virtual ~Tile() = default;

    virtual int getMovementMod(const Piece &piece) const { return 0; }
    virtual int getRoadMovementMod(const Piece &piece, Tile * nextTile) const { return 0; }
    virtual int getToughnessMod() const { return 0; }
    virtual int getStrengthMod(const Piece &piece) const { return 0; }
    virtual int getRangedStrengthMod(const Piece &piece) const { return 0; }
    virtual int getRangeMaxMod() const { return 0; }
};

struct Water : Tile {
    Water(int x, int y) : Tile(x, y, TerrainType::Water) {}
    ~Water() override = default;
    int getMovementMod(const Piece &piece) const override;
};

struct Field : Tile { 
    int x, y;
    Field(int x, int y) : Tile(x, y, TerrainType::Field) {}
    ~Field() override = default;
    TerrainType terrain = TerrainType::Field;
    int getStrengthMod(const Piece &piece) const override;
};

struct Forest : Tile {
    int x, y;
    Forest(int x, int y) : Tile(x, y, TerrainType::Forest) {}
    ~Forest() override = default;
    TerrainType terrain = TerrainType::Forest;
    int getMovementMod(const Piece &piece) const override;
    int getToughnessMod() const override;
    int getStrengthMod(const Piece &piece) const override;
    int getRangeMaxMod() const override;
};

struct Mountain : Tile {
    int x, y;
    Mountain(int x, int y) : Tile(x, y, TerrainType::Mountain) {}
    ~Mountain() override = default;
    TerrainType terrain = TerrainType::Mountain;
    int getMovementMod(const Piece &piece) const override;
    int getToughnessMod() const override;
    int getRangedStrengthMod(const Piece &piece) const override;
    int getRangeMaxMod() const override;
};

struct Road : Tile {
    int x, y;
    Road(int x, int y) : Tile(x, y, TerrainType::Road) {}
    ~Road() override = default;
    TerrainType terrain = TerrainType::Road;
    int getRoadMovementMod(const Piece &piece, Tile * nextTile) const override;
    int getStrengthMod(const Piece &piece) const override;
};

struct Desert : Tile {
    int x, y;
    Desert(int x, int y) : Tile(x, y, TerrainType::Desert) {}
    ~Desert() override = default;
    TerrainType terrain = TerrainType::Desert;
    int getMovementMod(const Piece &piece) const override;
    int getToughnessMod() const override;
    int getStrengthMod(const Piece &piece) const override;
};

struct Jungle : Tile {
    int x, y;
    Jungle(int x, int y) : Tile(x, y, TerrainType::Jungle) {}
    ~Jungle() override = default;
    TerrainType terrain = TerrainType::Jungle;
    int getMovementMod(const Piece &piece) const override;
    int getToughnessMod() const override;
    int getStrengthMod(const Piece &piece) const override;
    int getRangedStrengthMod(const Piece &piece) const override;
    int getRangeMaxMod() const override;
};

struct Peak : Tile {
    int x, y;
    Peak(int x, int y) : Tile(x, y, TerrainType::Peak) {}
    ~Peak() override = default;
    TerrainType terrain = TerrainType::Peak;
    int getMovementMod(const Piece &piece) const override;
    int getToughnessMod() const override;
    int getRangedStrengthMod(const Piece &piece) const override;
    int getRangeMaxMod() const override;
};

struct IceField : Tile {
    int x, y;
    IceField(int x, int y) : Tile(x, y, TerrainType::IceField) {}
    ~IceField() override = default;
    TerrainType terrain = TerrainType::IceField;
};

struct SnowField : Tile {
    int x, y;
    SnowField(int x, int y) : Tile(x, y, TerrainType::SnowField) {}
    ~SnowField() override = default;
    TerrainType terrain = TerrainType::SnowField;
    int getMovementMod(const Piece &piece) const override;
};

struct Tundra : Tile {
    int x, y;
    Tundra(int x, int y) : Tile(x, y, TerrainType::Tundra) {}
    ~Tundra() override = default;
    TerrainType terrain = TerrainType::Tundra;
};

struct Objective : Tile {
    int x, y;
    Objective(int x, int y) : Tile(x, y, TerrainType::Objective) {}
    ~Objective() override = default;
    TerrainType terrain = TerrainType::Objective;
    int getToughnessMod() const override;
    Player isCapturedBy = Player::None;
};


// Functions:

// Function to randomly generate a terrain type for a tile, will be updated later for perlin noise
std::unique_ptr<Tile> makeRandomTerrain(float noise, BiomeType biome, int x, int y);

// Function to generate a road on a board.
std::vector<int> generateRoad(Tile * startTile, Tile * endTile, std::unordered_map<int, std::unique_ptr<Tile>> &board, int width, int height);
