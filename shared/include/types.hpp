#ifndef TYPES_HPP
#define TYPES_HPP

/*
 * This header defines all the enums which are used across the code.
 * The idea is that runes will operate over these enums.
 *     - PieceType defines the basic stats of pieces and some terrian interactions
 *     - PieceCategory defines terrain interactions primarily
 *     - PieceMaterial determines the kinds of runes that can be applied to a piece
 *     - TerrainType defines terrian rules
 *     - BiomeType defines which TerrainTypes can exist on a board
 *     - MissionType determines the objective of a GameInstance
 *     - LeaderType determines the initial pieces of a RunInstance and the Leader Rule
 */
enum class PieceType {
    Light,
    Shield,
    Elite,
    Archer,
    LCavalry,
    MCavalry,
    HCavalry,
    Catapult,
    Ballista,
    Chariot,
    Commander,
    Wizard,
    Assassin,
    Druid
};

enum class PieceCategory {
    Infantry, //Light, Shield, Elite, and Archer
    Cavalry,  // Light, Medium, and Heavy Cavalry
    Siege,    // Catapult, Ballista, and Chairot
    Special   // Commander, Wizard, Assasin, and Druid
};

enum class PieceMaterial {
    Wood,
    Stone,
    Iron,
    Gold,
    Diamond
};

enum class MoveType {
    Move,
    Shoot,
    Capture,
    Any
};

enum class TerrainType {
    Field,
    Forest,
    Water,
    Mountain,
    Road,
    Desert,
    Jungle,
    Peak,
    IceField,
    SnowField,
    Tundra,
    Objective
};

enum class BiomeType {
    Temperate,
    Grassy,
    Arid,
    Tropical,
    Alpine,
    Arctic
};

enum class MissionType {
    HoldThePoint
};

enum class LeaderType {
    Scout
};

namespace MoveResult {
    constexpr int EnemyWin = 3;
    constexpr int PlayerWin = 2;
    constexpr int Success = 1;
    constexpr int InvalidMove = 0;
    constexpr int TargetOutOfBounds = -1;
    constexpr int PieceNotInGame = -2;
    constexpr int PieceNotOnBoard = -3;
    constexpr int TargetNotOnBoard = -4;
}

struct Direction {
    int dx, dy;
};

static constexpr Direction vectors[8] = {
    { 1, 0}, // Right
    {-1, 0}, // Left
    { 0, 1}, // Up
    { 0,-1}, // Down
    { 1, 1}, // Right and Up
    {-1, 1}, // Left and Up
    { 1,-1}, // Right and Down
    {-1,-1}  // Left and Down
};

#endif
