#include "gameinstance.hpp"
#include "pieces.hpp"
#include "tiles.hpp"
#include "types.hpp"
#include "PerlinNoise.hpp"
#include "enemy.hpp"

#include <cstdlib>
#include <ncurses.h>
#include <vector>
#include <random>
#include <memory>

/* 
 * This file details the methods of the GameInstance object, which handles the logic for running a Game Instance.
 * A Game Instance is first initialized by a bunch of properties determined by its parent Run Instance.
 * The properties are the following:
 *     - The seed, which is used to generate the random elements of the board, such as:
 *         + Board dimensions
 *         + The Perlin Noise map
 *         + Mission objective location
 *         + The enemy piece vector composition
 *     - The biome, which is used to determine the tile makeup of a board
 *     - The mission type, which defines the objectives of the game instance
 *     - The perlin noise octave value
 *     - Whether the map will have a road
 * The Game Instance is initialized when a Run Instance is created.
 * The properties are used to display information about a Game Instance before it is selected.
 * When a Game Instance is selected it uses the makeGame function to actually generate the game board and copy the player's run pieces.
 * From there, setupGame (external function) is used to prompt the player for where to addPiece(s).
 * Then, takePlayerTurn and takeEnemyTurn handle turn by turn logic, which are repeated instances of movePiece, shootPiece, or capturePiece.
 * These functions are likewise controlled by an external function, runGame, which calls the game instance methods using user input.
 * getValidTiles is the main game logic function. It handles rune logic, movement logic (terrain and piece capture), and action logic.
 */

GameInstance::GameInstance(unsigned long seed, BiomeType biome, MissionType mission, int octave, bool hasRoad) {
    this->biome = biome;
    this->mission = mission;
    this->octave = octave;
    this->seed = seed;
    this->hasRoad = hasRoad;

    std::mt19937 gen(seed);
    std::uniform_int_distribution<int> dis(6, 10);
    boardHeight = dis(gen);
    boardWidth = dis(gen);
}

// Remember to get rid of the window parameter when we move from ncurses
void GameInstance::makeGame(std::vector<std::unique_ptr<Piece>>&& runPieces, std::vector<std::unique_ptr<Piece>>&& enemyPieces, WINDOW * window) {
    wprintw(window, "Generating game instance...\n");
    wprintw(window, "Generating board of dimensions %d by %d.\n", boardWidth, boardHeight);
    board.reserve(boardWidth * boardHeight); 

    std::mt19937 gen(seed);
    
    const siv::PerlinNoise noiseMap{ seed }; // Generate perlin noise map from seed

    for (int y = 0; y < boardHeight; y++) {
        for (int x = 0; x < boardWidth; x++) {
            // (x, y) coordinates are normalized in order to account for perlin noise frequency
            float nx = (float)x / boardWidth;
            float ny = (float)y / boardHeight;
            float noiseValue = noiseMap.octave2D_01((nx * 4.0f), (ny * 4.0f), octave);  

            board.push_back(Tile{x, y, getRandomTerrain(noiseValue, biome), nullptr});
            // (x, y), terrain type, and null pointer representing no occupying piece
        }
    }
    wprintw(window, "Base board generated\n");

    if (hasRoad) {
        wprintw(window, "Generating road.\n");
        Tile * startRoad;
        Tile * endRoad;
        std::uniform_int_distribution<int> roadDis(0, boardWidth-1);

        do {
            int roadStartX = roadDis(gen);
            startRoad = &board[roadStartX];
        } while (startRoad->terrain != TerrainType::Field && startRoad->terrain != TerrainType::Forest);
        
        do {
            int roadStartX = roadDis(gen);
            endRoad = &board[boardWidth * (boardHeight-1) + roadStartX];
        } while (endRoad->terrain != TerrainType::Field && endRoad->terrain != TerrainType::Forest);

        std::vector<Tile *> road;
        if (startRoad != nullptr && endRoad != nullptr) road = generateRoad(startRoad, endRoad, board, boardWidth, boardHeight, window);
        for (Tile * tile : road) {
            if (tile != nullptr) {
                tile->terrain = TerrainType::Road;
            }
        }
        wprintw(window, "Road generated\n");
    }

    switch (mission) {
        case MissionType::HoldThePoint: {
            wprintw(window, "Mission type is Hold the Point\n");
            std::uniform_int_distribution<int> xDist(0, boardWidth-1);
            int margin = boardHeight/4;
            std::uniform_int_distribution<int> yDist(margin, boardHeight-margin-1);
            int objX = xDist(gen);
            int objY = yDist(gen);
            board[objY * boardWidth + objX].terrain = TerrainType::Objective;
            break;
        }
        default:
            wprintw(window, "Error, no mission type for the Game Instance\n");
    }

    wprintw(window, "Copying board and pieces to game instance...\n");
    this->playerPieces = std::move(runPieces); 
    this->enemyPieces = std::move(enemyPieces);
}

Tile * GameInstance::getTile(int x, int y) {
    if (x < 0 || x >= boardWidth || y < 0 || y >= boardHeight) {
        return nullptr;
    }
    return &board[(y * boardWidth) + x];
}

int GameInstance::getTileId(const Tile &tile) {
    return tile.y * boardWidth + tile.x;
}

bool GameInstance::pieceExists(Piece * piece) {
    if (piece->ownedByPlayer) {
        for (const std::unique_ptr<Piece> &p : playerPieces) if (p.get() == piece) return true;
    } else {
        for (const std::unique_ptr<Piece> &p : enemyPieces) if (p.get() == piece) return true;
    }
    return false;
}

Tile * GameInstance::getPieceTile(const Piece &piece) {
    for (Tile &t : board) if (t.occupyingPiece == &piece) return &t;
    return nullptr;
}

std::vector<Move> GameInstance::getValidMovement(const Piece &piece, Tile * currentTile, int relativeStrengthMod) {
    std::vector<Move> validTiles;
    int vectors[8][2] = {
        { 1, 0}, // Right
        {-1, 0}, // Left
        { 0, 1}, // Up
        { 0,-1}, // Down
        { 1, 1}, // Right and Up
        {-1, 1}, // Left and Up
        { 1,-1}, // Right and Down
        {-1,-1}  // Left and Down
    };

    for (int i = 0; i < 8; i++) {
        bool stopMove = false;
        int cardinalEval = piece.maxCardinal, diagonalEval = piece.maxDiagonal;
        // j iterates through tiles on that vector. The ternary operator in the middle of this loop determines if the number of tiles iterated through is limited by the cardinalEval or diagonalEval value, corresponding with the array of vectors. 
        for (int j = 1; (i < 4) ? j <= cardinalEval : j <= diagonalEval; j++) { 
            Tile * checkTile = getTile(currentTile->x + (vectors[i][0]*j), currentTile->y + (vectors[i][1]*j));
            if (!checkTile) break; // Fail case if tile doesn't exist
            
            int relativeToughnessMod = 0;

            switch (checkTile->terrain) {
                case TerrainType::Field: break;
                case TerrainType::Forest:
                    if ((piece.category == PieceCategory::Cavalry && 
                         piece.type != PieceType::LCavalry) || 
                         piece.category == PieceCategory::Siege) (i < 4) ? cardinalEval-- : diagonalEval--;
                    relativeToughnessMod++;
                    break;
                case TerrainType::Water:
                    stopMove = true;
                    break;
                case TerrainType::Mountain:
                    if (piece.category == PieceCategory::Cavalry || 
                        piece.category == PieceCategory::Siege) {
                        stopMove = true;
                    } else {
                        (i < 4) ? cardinalEval-- : diagonalEval--;
                    }
                    relativeToughnessMod++;
                    break;
                case TerrainType::Road: {
                    Tile * nextTile = getTile(checkTile->x + vectors[i][0], checkTile->y + vectors[i][1]);
                    if (nextTile != nullptr &&
                        nextTile->terrain != TerrainType::Water &&
                        nextTile->terrain == TerrainType::Road) (i < 4) ? cardinalEval++ : diagonalEval++; // Beautiful
                    break; }
                case TerrainType::Desert:
                    relativeToughnessMod--;
                    (i < 4) ? cardinalEval-- : diagonalEval--;
                    break;
                case TerrainType::Jungle:
                    if (piece.category == PieceCategory::Siege) {
                        stopMove = true;
                        break;
                    }
                    if (!(piece.type == PieceType::Light)) (i < 4) ? cardinalEval-- : diagonalEval--;
                    if (piece.category == PieceCategory::Cavalry) (i < 4) ? cardinalEval-- : diagonalEval--;
                    relativeToughnessMod++;
                    break;
                case TerrainType::Peak:
                    stopMove = true;
                    break;
                // Arctic rules are simple here because effects will be determined later on if these tiles are selected.
                case TerrainType::IceField:
                    break;
                case TerrainType::SnowField:
                    (i < 4) ? cardinalEval-- : diagonalEval--;
                    break;
                case TerrainType::Tundra:
                    break;
                case TerrainType::Objective:
                    relativeToughnessMod++;
                    break;
            }

            if ((i < 4) ? j > cardinalEval : j > diagonalEval) break;
            if (stopMove == true) break;

            // Piece capture logic Here
            if (checkTile->occupyingPiece) { 
                if (checkTile->occupyingPiece->ownedByPlayer == piece.ownedByPlayer) { 
                    if (piece.canMoveThroughPieces) {
                        continue;
                    } else {
                        break;
                    }
                } else {
                    int relativeToughness = checkTile->occupyingPiece->toughness + relativeToughnessMod;
                    if (piece.strength + relativeStrengthMod >= relativeToughness) {
                        validTiles.push_back(Move{MoveType::Move, currentTile, checkTile});
                        break;
                    }
                    break;
                }
            } else {
                validTiles.push_back(Move{MoveType::Move, currentTile, checkTile});
            }
        }
    }
    return validTiles;
}

std::vector<Move> GameInstance::getValidRangedAttacks(const Piece &piece, Tile * currentTile, int relativeRangedStrengthMod, int relativeRangeMax) {
    std::vector<Move> validTiles;
    int evalMaxRange = piece.rangedAttack.maxRange + relativeRangeMax;
    int evalRangedStrength = piece.rangedAttack.strength + relativeRangedStrengthMod;

    for (int dx = -evalMaxRange; dx <= evalMaxRange; dx++) { 
        for (int dy = -evalMaxRange; dy <= evalMaxRange; dy++) {
            int nx = currentTile->x + dx;
            int ny = currentTile->y + dy;
            int dist = std::abs(dx) + std::abs(dy);

            if (piece.rangedAttack.minRange <= dist && dist <= evalMaxRange) {
                if (nx >= 0 && nx < boardWidth && ny >=0 && ny < boardHeight) {
                    Tile * searchTile = getTile(nx, ny);
                    if (searchTile->occupyingPiece && searchTile->occupyingPiece->ownedByPlayer != piece.ownedByPlayer) {
                        if (evalRangedStrength >= searchTile->occupyingPiece->toughness) {
                            validTiles.push_back(Move{MoveType::Shoot, currentTile, searchTile});
                        }
                    }
                }
            }
        }
    }
    
    return validTiles;
}

// This function creates an array of valid move structs for a passed piece object 
std::vector<Move> GameInstance::getValidMoves(const Piece &piece, MoveType type) {
    Tile * currentTile = getPieceTile(piece);    
    int relativeStrengthMod = 0;
    int relativeRangedStrengthMod = 0;
    int relativeRangeMax = 0;
    bool canCapture = false;
    switch (currentTile->terrain) {
        case TerrainType::Field: break;
        case TerrainType::Forest:
            if (piece.category != PieceCategory::Infantry) relativeStrengthMod++;
            relativeStrengthMod++;
            relativeRangedStrengthMod--;
            break;
        case TerrainType::Water: break;
        case TerrainType::Mountain:
            if (piece.category == PieceCategory::Siege) relativeRangedStrengthMod++;
            relativeRangeMax++;
            break;
        case TerrainType::Road:
            if (piece.category == PieceCategory::Cavalry) relativeStrengthMod++;
            break; 
        case TerrainType::Desert: break;
        case TerrainType::Jungle:
            if (!(piece.type == PieceType::Light ||
                piece.type == PieceType::LCavalry)) relativeStrengthMod--;
            relativeRangedStrengthMod--;
            relativeRangeMax--;
            break;
        case TerrainType::Peak:
            relativeRangeMax++;
            relativeRangedStrengthMod++;
            break;
        case TerrainType::IceField:
            break;
        case TerrainType::SnowField:
            break;
        case TerrainType::Tundra:
            break;
        case TerrainType::Objective:
            canCapture = true;
            break;
    }

    // Rune Logic Happens Here
    std::vector<Move> validTiles;
    validTiles.reserve((piece.maxCardinal+3 * 4) + (piece.maxDiagonal+3 * 4) + (2 * piece.rangedAttack.maxRange+2 * piece.rangedAttack.maxRange+2 + 2 * piece.rangedAttack.maxRange+2 + 1)+1); // This is essentially and arbitrarily large number to reserve. 

    if (type == MoveType::Any) {
        for (Move move : getValidMovement(piece, currentTile, relativeStrengthMod)) validTiles.push_back(move);
        for (Move move : getValidRangedAttacks(piece, currentTile, relativeRangedStrengthMod, relativeRangeMax)) validTiles.push_back(move);
    } else if (type == MoveType::Move) {
        for (Move move : getValidMovement(piece, currentTile, relativeStrengthMod)) validTiles.push_back(move);
    } else if (type == MoveType::Shoot) {
        for (Move move : getValidRangedAttacks(piece, currentTile, relativeRangedStrengthMod, relativeRangeMax)) validTiles.push_back(move);
    } else if (type == MoveType::Capture && canCapture) {
        validTiles.push_back(Move{MoveType::Capture, currentTile, currentTile});
    }

    return validTiles;

}

int GameInstance::addPiece(Piece * piece, int tileIndex) {
    if (tileIndex >= board.size()) return -1; // Fail condition 1, out of bounds
    
    if (!pieceExists(piece)) return -2; // Fail condition 2, piece not in game

    Tile * tile = &board[tileIndex];
    if (tile->occupyingPiece != nullptr) return -3; // Fail condition 3, tile occupied

    if (tile->terrain == TerrainType::Water) return -4; // Fail condition 4, tile invalid

    tile->occupyingPiece = piece;
    piece->onBoard = true;
    return 1;
}

int GameInstance::movePiece(Piece * piece, Tile * target) {
    if (!target) return -1; // Fail condition 1, out of bounds
    
    if (!pieceExists(piece)) return -2; // Fail condition 2, piece not in game

    Tile * currentTile = getPieceTile(*piece);
    if (!currentTile->occupyingPiece->onBoard) return -3; // Fail condition 3, piece not on board

    std::vector<Move> validTiles = getValidMoves(*piece, MoveType::Move);

    for (Move move : validTiles) {
        if (target == move.to) {
            if (target->occupyingPiece) target->occupyingPiece->onBoard = false;
            currentTile->occupyingPiece = nullptr;
            target->occupyingPiece = piece;
            return 1;
        }
    }

    return -4; // Fail condition 4, rare exception
}

int GameInstance::shootPiece(Piece * piece, Tile * target) {
    if (!target) return -1; // Fail condition 1, out of bounds

    if (!pieceExists(piece)) return -2; // Fail condition 2, piece not in game

    Tile * currentTile = getPieceTile(*piece);
    if (!currentTile->occupyingPiece->onBoard) return -3; // Fail condition 3, piece not on board

    if (!target->occupyingPiece) return -4; // Fail condition 4, target not on board

    std::vector<Move> validTiles = getValidMoves(*piece, MoveType::Shoot);
    for (Move move : validTiles) {
        if (target == move.to) {
            move.to->occupyingPiece->onBoard = false;
            move.to->occupyingPiece = nullptr;
            return true;
        }
    }
    return -5; // Fail condition 5, other error
}

// Currently doesn't do anything
int GameInstance::isMissionComplete() {
    switch (mission) {
        case MissionType::HoldThePoint: {
            Tile * objective = nullptr;
            for (Tile &tile : board) {
                if (tile.terrain == TerrainType::Objective) objective = &tile;
            }

            if (objective->occupyingPiece == nullptr) {
                playerHoldsObjective = false;
                enemyHoldsObjective = false;
            } else if (objective->occupyingPiece->ownedByPlayer) {
                if (playerHoldsObjective) return 2;
                playerHoldsObjective = true;
                enemyHoldsObjective = false;
            } else {
                if (enemyHoldsObjective) return 3;
                playerHoldsObjective = false;
                enemyHoldsObjective = true;
            }
            return 1;
        }
        default: return 1;
    }
}

int GameInstance::takePlayerTurn(Move move) {
    switch (move.type) {
        case MoveType::Move: 
            movePiece(move.from->occupyingPiece, move.to);
            return 1;
        case MoveType::Shoot:
            shootPiece(move.from->occupyingPiece, move.to);
            return 1;
        case MoveType::Capture:
            playerHoldsObjective = true;
            return 2;
        default:
            return 0;
    }
}

int GameInstance::setupEnemy() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution disX(0, boardWidth - 1);

    for (int i = 0; i < enemyPieces.size(); i++) {
        if (addPiece(enemyPieces[i].get(), disX(gen))) continue;
        i--;
    }
    return 1;
}

int GameInstance::takeEnemyTurn() {
    std::vector<Move> allEnemyMoves;
    for (const std::unique_ptr<Piece> &piece : enemyPieces) {
        if (piece->onBoard) {
            std::vector<Move> pieceValidMoves = getValidMoves(*piece, MoveType::Any);
            allEnemyMoves.insert(allEnemyMoves.end(), pieceValidMoves.begin(), pieceValidMoves.end());
        }
    }
    
    if (allEnemyMoves.empty()) return 2;

    Move move = enemyAlgoRandom(allEnemyMoves, board);

    switch (move.type) {
        case MoveType::Move: 
            movePiece(move.from->occupyingPiece, move.to);
            return 1;
        case MoveType::Shoot:
            shootPiece(move.from->occupyingPiece, move.to);
            return 1;
        case MoveType::Capture:
            enemyHoldsObjective = true;
            return 3;
        default:
            return 0;
    }
}
