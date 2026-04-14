#pragma once

#include <vector>
#include <unordered_set>
#include <string>

enum class Rune {
    // Values
    One, Two, Three,
    // Quantifiers
    Self, All, Exists,
    // PieceType Identifiers
    Light, Shield, Elite, Archer,
    LCavalry, MCavalry, HCavalry,
    Commander, Wizard, Assassin, Druid,
    // PieceCategory Identifiers
    Infantry, Cavalry, Siege, Special,
    // PieceMaterial Identifiers
    Wood, Stone, Iron, Gold, Diamond,
    // TerrainType Identifiers
    Field, Forest, Water, Mountain,
    Desert, Jungle, Peak,
    IceField, SnowField, Tundra,
    Road, Objective,
    // BiomeType Identifiers
    Temperate, Grassy, Arid, Tropical, Alpine, Arctic,
    // Proximity
    Proximity,
    // Effects
    CardinalMod, DiagonalMod, MovementMod,
    ToughnessMod, StrengthMod, RangedStrengthMod,
    RangeMaxMod, RangeMinMod,
    CanMoveThroughPieces, CanShootThroughPieces,
    // Wizard Runes
    Fireball,
    // Druid Runes
    Summon,
    // Conditional Runes
    And, Or, Not, Is, If, Has
};

struct CraftedRune {
    std::string name;
    std::vector<Rune> runes;
};

struct RuneLibrary {
    std::unordered_set<Rune> basic;
    std::vector<CraftedRune> constructed;
};
