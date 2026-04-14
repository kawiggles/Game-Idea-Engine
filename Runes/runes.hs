data PieceType
    = Light
    | Shield
    | Elite
    | Archer
    | LCavalry
    | MCavalry
    | HCavalry
    | Catapult
    | Ballista
    | Chariot
    | Commander
    | Wizard
    | Assassin
    | Druid
    deriving (Show, Eq)

data PieceCategory
    = Infantry
    | Cavalry
    | Siege
    | Special
    deriving (Show)

pieceCategory :: PieceType -> PieceCategory
pieceCategory p
    | p `elem` [Light, Shield, Elite, Archer]       = Infantry
    | p `elem` [HCavalry, MCavalry, LCavalry]       = Cavalry
    | p `elem` [Catapult, Ballista, Chariot]        = Siege
    | p `elem` [Commander, Wizard, Assassin, Druid] = Special
    | otherwise                                     = Infantry

data PieceMaterial
    = Wood
    | Stone
    | Iron
    | Gold
    | Diamond
    deriving (Show)

data TerrainType
    = Field
    | Forest
    | Water
    | Mountain
    | Road
    | Desert
    | Jungle
    | Peak
    | IceField
    | SnowField
    | Tundra
    | Objective
    deriving (Show)

data BiomeType
    = Temperate
    | Grassy
    | Arid
    | Tropical
    | Alpine
    | Arctic

data Sentence = Sentence If Has

data If
    = Condition Subject Identifier
    | AndCondition If If
    | OrCondition If If
    | NotCondition If

data Has
    = Effect Subject Identifier RuleEffect
    | AndEffect Has Has
    | SpecialMoveEffect SpecialMove

data Subject
    = ConditionSelf 
    | ConditionAll Identifier 
    | ExistsCheck Identifier

data Identifier
    = AndIdentifier Identifier Identifier
    | OrIdentifier Identifier Identifier
    | NotIdentifier Identifier
    | OnTerrain TerrainType
    | InBiome BiomeType
    | IsMaterial PieceMaterial
    | IsType PieceType
    | IsCategory PieceCategory
    | IsInProximity Int

data RuleEffect
    = AndRuleEffect RuleEffect RuleEffect
    | CardinalMod Int
    | DiagonalMod Int
    | StrengthMod Int
    | ToughnessMod Int
    | RangedStrengthMod Int
    | RangeMaxMod Int
    | RangeMinMod Int
    | CanMoveThroughPieces Bool
    | CanShootThroughPieces Bool

data SpecialMove
    = WizardMove WizardAbility
    | DruidMove DruidAbility

data WizardAbility
    = Fireball

data DruidAbility
    = Summon

data RuleType
    = Generating
    | Limiting 
    | GenLim
    | Paradox
    deriving(Show, Eq, Ord, Enum, Bounded)
