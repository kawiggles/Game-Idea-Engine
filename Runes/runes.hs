data PieceContext = PieceContext
    { pieceType     :: PieceType
    , pieceCategory :: PieceCategory
    , pieceMaterial :: PieceMaterial
    , isWizard      :: Bool
    , isDruid       :: Bool
    , isCommander   :: Bool
    }

data Sentence = IfThen Condition Effect

data Condition
    = StateCondition ConditionSubject [StatePredicate]
    | AndCondition Condition Condition
    | OrCondition Condition Condition
    | NotCondition Condition

data Effect
    = ModifierEffect EffectSubject [StatePredicate] [RuleEffect]
    | AndEffect Effect Effect
    | SpecialMoveEffect SpecialMove

data ConditionSubject
    = ConditionSelf 
    | ConditionAll Identifier 
    | ExistsCheck Identifier

data EffectSubject
    = EffectSelf
    | EffectAll Identifier
    | ExistsSelect Identifier

data StatePredicate
    = OnTerrain TerrainType
    | IsMaterial PieceMaterial
    | IsType PieceType
    | IsCategory PieceCategory

data Identifier
    = ByCategory PieceCategory
    | ByType PieceType
    | ByMaterial PieceMaterial

data RuleEffect
    = CardinalMod Int
    | DiagonalMod Int
    | StrengthMod Int
    | ToughnessMod Int
    | RangedStrenghtMod Int
    | RangeMaxMod Int
    | RangeMinMod Int
    | CanMoveThroughPieces Bool
    | CanShootThroughPieces Bool


data SpecialMove
    = WizardMove WizardAbility
    | DruidMove DruidAbility

data WizardAbility

data DruidAbility

