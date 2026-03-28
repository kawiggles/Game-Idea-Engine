#pragma once

enum class RuneType {
    Quantifier,
    Subject,
    Predicate
};

enum class QuantifierType {
    Forall,
    Exists
};

enum class SubjectType {
    // Self Rune only targets self, might be useful
    Self,
    // All piece types
    Light,
    Shield,
    Elite,
    Archer,
    LCavalry,
    MCavalry,
    HCavalry,
    Commander,
    Wizard,
    // All piece classes
    Infantry,
    Cavalry,
    Special,
    // All materials
    Wood,
    Iron,
    Silver,
    Gold,
    Diamond,
    // Universal, for every piece owned by player
    Universal
};

enum class PredicateType {
    // Movement Buffs
    Plus1Movement,
    // Strength Buffs
    Plus1Strength,
    // Toughness Buffs
    Plus1Toughness,
    // Other Rules
    CanMoveThroughPieces
};

struct RuneSentence {
    QuantifierType quantifier = QuantifierType::Exists;
    SubjectType subject = SubjectType::Self;
    PredicateType predicate = PredicateType::Plus1Movement;
};
