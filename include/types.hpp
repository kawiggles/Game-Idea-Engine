#ifndef TYPES_HPP
#define TYPES_HPP

// All the different types of pieces, defines movement and base stats for a piece
enum class PieceType {
    Light,
    Shield,
    Elite,
    Archer,
    LCavalry,
    MCavalry,
    HCavalry,
    Commander,
    Wizard
};

enum class PieceCategory {
    Infantry, //Light, Shield, Elite, Archer
    Cavalry, // Light, Medium, and Heavy Cavalry
    Special // Commander and Wizard
};

// The materials each piece can be made of, works as a class for the rune logic system
enum class PieceMaterial {
    Wood,
    Iron,
    Silver,
    Gold,
    Diamond
};

// Names for different kinds of terrain, not using ints because the logic for each terrain piece will be complicated
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

};

#endif
