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
    Tundra
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
    FillerMission
};

enum class LeaderType {
    Scout
};

#endif
