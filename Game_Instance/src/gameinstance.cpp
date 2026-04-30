#include "gameinstance.hpp"
#include "pieces.hpp"
#include "tiles.hpp"
#include "types.hpp"
#include "PerlinNoise.hpp"
#include "enemy.hpp"
#include "logs.hpp"

#include <cstdlib>
#include <ncurses.h>
#include <vector>
#include <unordered_set>
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

    log("Game instance initialized in biome %s\n", getBiomeType(this->biome).c_str());
}

void GameInstance::makeGame(std::vector<std::unique_ptr<Piece>> &&runPieces, std::vector<std::unique_ptr<Piece>> &&enemyPieces) {
    log("Running makeGame instance...");
    board.reserve(boardWidth * boardHeight); 
    log("Board of dimensions %d by %d generated", boardWidth, boardHeight);

    std::mt19937 gen(seed);
    const siv::PerlinNoise noiseMap{ seed }; // Generate perlin noise map from seed

    for (int y = 0; y < boardHeight; y++) {
        for (int x = 0; x < boardWidth; x++) {
            // (x, y) coordinates are normalized in order to account for perlin noise frequency
            float nx = (float) x / boardWidth;
            float ny = (float) y / boardHeight;
            float noiseValue = noiseMap.octave2D_01((nx * 4.0f), (ny * 4.0f), octave);  

            board.emplace(y * boardWidth + x, makeRandomTerrain(noiseValue, biome, x , y));
            log("Tile at (%d, %d) of terrain type %s", x+1, y+1, getTerrainType(board[y * boardWidth + x]->terrain).c_str());
        }
    }
    log("Base board generated\n");

    if (hasRoad) {
        log("Generating road");
        Tile  * startRoad;
        Tile  * endRoad;
        std::uniform_int_distribution<int> roadDis(0, boardWidth-1);

        // TODO: add guards for potential infinite loop
        do {
            int roadStartX = roadDis(gen);
            startRoad = board[roadStartX].get();
        } while (startRoad->terrain != TerrainType::Field && startRoad->terrain != TerrainType::Forest);
        log("\tStart of road is (%d, %d)", startRoad->x+1, startRoad->y+1);
        
        do {
            int roadStartX = roadDis(gen);
            endRoad = board[boardWidth * (boardHeight-1) + roadStartX].get();
        } while (endRoad->terrain != TerrainType::Field && endRoad->terrain != TerrainType::Forest);
        log("\tEnd of road is (%d, %d)", endRoad->x+1, endRoad->y+1);

        std::vector<int> road;
        road = generateRoad(startRoad, endRoad, board, boardWidth, boardHeight);
        for (int tile : road) {
            int nx = board[tile]->x;
            int ny = board[tile]->y;
            board[tile] = std::make_unique<Road>(nx, ny);
            log("Road tile emplaced at (%d, %d)", nx, ny);
        }
        log("Road generated\n");
    }

    switch (mission) {
        case MissionType::HoldThePoint: {
            log("Mission type is Hold the Point");
            std::uniform_int_distribution<int> xDist(0, boardWidth-1);
            int margin = boardHeight/4;
            std::uniform_int_distribution<int> yDist(margin, boardHeight-margin-1);
            int objX = xDist(gen);
            int objY = yDist(gen);
            
            board[objY * boardWidth + objX] = std::make_unique<Objective>(objX, objY);
            objectives.reserve(1);
            Objective * newObj = dynamic_cast<Objective *>(board.at(objY * boardWidth + objX).get());
            objectives.push_back(newObj);
            log("\tObjective set at (%d, %d)\n", objX, objY);
            break;
        }
        default:
            log("Error, no mission type for the Game Instance\n");
    }

    log("Copying pieces to game instance...\n");
    this->playerPieces = std::move(runPieces); 
    this->enemyPieces = std::move(enemyPieces);
}

// Essentially unnecessary, but helps with clarity when searching through adjacent x,y coordinates
Tile * GameInstance::getTile(int x, int y) {
    if (x < 0 || x >= boardWidth || y < 0 || y >= boardHeight)
        return nullptr;
    return board[y * boardWidth + x].get();
}

// Using this in place of contains because I was dumb and used raw pointers everywhere, and both are O(n) efficiency
// Might consider making the vector of player pieces an unordered set to make efficiency O(log n) instead of O(n)
bool GameInstance::pieceExists(Piece * piece) {
    ASSERT(piece);
    if (piece->owner == Player::Human) {
        for (const std::unique_ptr<Piece> &p : playerPieces) if (p.get() == piece) return true;
    } else {
        for (const std::unique_ptr<Piece> &p : enemyPieces) if (p.get() == piece) return true;
    }
    return false;
}

std::unordered_set<Move, MoveHash> GameInstance::getValidMovement(Tile &currentTile, int relativeStrengthMod) {
    const Piece piece = *currentTile.occupyingPiece;
    log("\tgetValidMovement running for %s at tile (%d, %d)", getPieceType(piece).c_str(), currentTile.x, currentTile.y);
    std::unordered_set<Move, MoveHash> validTiles;

    for (int i = 0; i < 8; i++) {
        log("\t\tChecking vector [%d, %d]", vectors[i].dx, vectors[i].dy);
        int maxMoveEval = (i < 4) ? piece.maxCardinal : piece.maxDiagonal;
        // j iterates through tiles on a vector. The ternary operator in this loop determines if the iteration limit is cardinalEval or diagonalEval. 
        for (int j = 1; j <= maxMoveEval; j++) { 
            Tile * checkTile = getTile(currentTile.x + (vectors[i].dx*j), currentTile.y + (vectors[i].dy*j));
            if (!checkTile) break; 
            log("\t\t\tChecking tile (%d, %d)", checkTile->x+1, checkTile->y+1);
            
            maxMoveEval += (checkTile->terrain == TerrainType::Road) ?
                checkTile->getRoadMovementMod(piece, getTile(checkTile->x + vectors[i].dx, checkTile->y + vectors[i].dy)) :
                checkTile->getMovementMod(piece); // Disgusting
            if (j > maxMoveEval) break;

            // Piece capture logic here
            if (checkTile->occupyingPiece) { 
                Piece * checkPiece = checkTile->occupyingPiece;
                if (checkPiece->owner == piece.owner) { 
                    if (piece.canMoveThroughPieces) continue;
                } else {
                    log("\t\t\t\tValid occupied tile found at (%d, %d)", checkTile->x+1, checkTile->y+1);
                    if (piece.strength + relativeStrengthMod >= checkPiece->toughness + checkTile->getToughnessMod())
                        validTiles.insert(Move{MoveType::Move, &currentTile, checkTile});
                }
                break;
            } else {
                log("\t\t\t\tValid tile found at (%d, %d)", checkTile->x+1, checkTile->y+1);
                validTiles.insert(Move{MoveType::Move, &currentTile, checkTile});
            }
        }
    }
    log("\tEnding getValidMovement\n");
    return validTiles;
}

std::unordered_set<Move, MoveHash> GameInstance::getValidRangedAttacks(Tile &currentTile, int relativeRangedStrengthMod, int relativeRangeMax) {
    const Piece piece = *currentTile.occupyingPiece;
    log("\tgetValidRangedAttacks starting for %s at tile (%d, %d)", getPieceType(piece).c_str(), currentTile.x+1, currentTile.y+1);
    std::unordered_set<Move, MoveHash> validTiles;
    int evalMaxRange = piece.rangedAttack.maxRange + relativeRangeMax;
    int evalRangedStrength = piece.rangedAttack.strength + relativeRangedStrengthMod;

    for (int dx = -evalMaxRange; dx <= evalMaxRange; dx++) { 
        for (int dy = -evalMaxRange; dy <= evalMaxRange; dy++) {
            int nx = currentTile.x + dx;
            int ny = currentTile.y + dy;
            int dist = std::abs(dx) + std::abs(dy);

            if (piece.rangedAttack.minRange <= dist && dist <= evalMaxRange) {
                if (nx >= 0 && nx < boardWidth && ny >=0 && ny < boardHeight) {
                    Tile * searchTile = getTile(nx, ny);
                    log("\t\tChecking tile (%d, %d)", nx+1, ny+1);
                    if (searchTile->occupyingPiece && searchTile->occupyingPiece->owner != piece.owner) {
                        if (evalRangedStrength >= searchTile->occupyingPiece->toughness + searchTile->getToughnessMod()) {
                            validTiles.insert(Move{MoveType::Shoot, &currentTile, searchTile});
                            log("\t\t\tValid ranged attack target found at (%d, %d)", nx+1, ny+1);
                        }
                    }
                }
            }
        }
    }
    log("\tEnding getValidRangedAttacks\n"); 
    return validTiles;
}

// This function creates an array of valid move structs for a passed piece object 
/* Need to consider changing the input of this function to const Tile &tile. Might be able to remove piecePositions as a result. */ 
std::unordered_set<Move, MoveHash> GameInstance::getValidMoves(Tile &tile, MoveType type) { 
    std::unordered_set<Move, MoveHash> validTiles;

    Piece piece = *tile.occupyingPiece;    
    log("Running getValidMoves for %s at tile (%d, %d) looking for movetype %s\n", getPieceType(piece).c_str(), tile.x+1, tile.y+1, getMoveType(type).c_str());
    int relativeStrengthMod = tile.getStrengthMod(piece);
    log("\tPiece strength: %d", piece.strength + relativeStrengthMod);
    int relativeRangedStrengthMod = tile.getRangedStrengthMod(piece);
    log("\tPiece ranged strength: %d", piece.rangedAttack.strength + relativeRangedStrengthMod);
    int relativeRangeMax = tile.getRangeMaxMod();
    log("\tPiece ranged attack range: %d\n", piece.rangedAttack.maxRange + relativeRangeMax);

    // Rune Logic Happens Here

    if (type == MoveType::Any) {
        log("\tFinding all moves...");
        validTiles.merge(getValidMovement(tile, relativeStrengthMod));
        validTiles.merge(getValidRangedAttacks(tile, relativeRangedStrengthMod, relativeRangeMax));
        if (tile.terrain == TerrainType::Objective) validTiles.insert(Move{MoveType::Capture, &tile, &tile});
        return validTiles;
    } else if (type == MoveType::Move) {
        log("\tFinding moves...");
        return getValidMovement(tile, relativeStrengthMod);
    } else if (type == MoveType::Shoot) {
        log("\tFinding ranged attacks...");
        return getValidRangedAttacks(tile, relativeRangedStrengthMod, relativeRangeMax);
    } else if (type == MoveType::Capture && tile.terrain == TerrainType::Objective) {
        log("\tFinding objective captures...");
        validTiles.insert(Move{MoveType::Capture, &tile, &tile});
    }
    return validTiles;
}

int GameInstance::addPiece(Piece * piece, int tileIndex) {
    log("Attempting to add %s at tile (%d, %d)", getPieceType(*piece).c_str(), board.at(tileIndex)->x+1, board.at(tileIndex)->y+1);
    ASSERT(tileIndex < board.size());
    ASSERT(pieceExists(piece));

    Tile * tile = board.at(tileIndex).get();
    if (tile->occupyingPiece != nullptr) return -1; // Fail condition 1, tile occupied
    if (tile->terrain == TerrainType::Water) return -2; // Fail condition 2, tile invalid

    tile->occupyingPiece = piece;
    piecePositions[piece] = tile;
    log("\tSuccess");
    return 1;
}

int GameInstance::setupEnemy() {
    log("Running setupEnemy...\n");
    std::mt19937 gen(seed);
    std::uniform_int_distribution disX(0, boardWidth - 1);

    for (int i = 0; i < enemyPieces.size(); i++) {
        if (addPiece(enemyPieces[i].get(), disX(gen)) == 1) continue;
        i--;
    }
    return 1;
}

GameInstance::Status GameInstance::executeMove(const Move &move) {
    ASSERT(getValidMoves(*move.from, move.type).count(move));
    ASSERT(move.to);
    Piece * piece = move.from->occupyingPiece;
    ASSERT(piecePositions.count(move.from->occupyingPiece));
    ASSERT(pieceExists(piece));

    switch (move.type) {
        case MoveType::Move: 
            log("Moving %s at (%d, %d) to (%d, %d)", getPieceType(*piece).c_str(), move.from->x+1, move.from->y+1, move.to->x+1, move.to->y+1);
            if (move.to->occupyingPiece) piecePositions.at(move.to->occupyingPiece) = nullptr;
            move.from->occupyingPiece = nullptr;
            move.to->occupyingPiece = piece;
            piecePositions[piece] = move.to;
            turnCount++;
            return getWinStatus();
        case MoveType::Shoot:
            ASSERT(pieceExists(move.to->occupyingPiece));
            log("Piece %s at (%d, %d) shooting piece at (%d, %d)",
                    getPieceType(*piece).c_str(), move.from->x+1, move.from->y+1, move.to->x+1, move.to->y+1);
            piecePositions.at(move.to->occupyingPiece) = nullptr;
            move.to->occupyingPiece = nullptr;
            turnCount++;
            return getWinStatus();
        case MoveType::Capture: {
            log("Piece %s attempting to capture objective at (%d, %d)",
                    getPieceType(*piece).c_str(), move.from->x+1, move.from->y+1);
            ASSERT(move.from->terrain == TerrainType::Objective);
            Objective * obj = dynamic_cast<Objective *>(move.from);
            obj->isCapturedBy = Player::Human;
            turnCount++;
            return getWinStatus();
        }
        default:
            return Status::Redo;
    }
}

GameInstance::Status GameInstance::takeEnemyTurn() {
    log("\nTaking enemy turn... \n\n");
    // The construction of this data structure will change as I figure out how the algo works
    std::unordered_set<Move, MoveHash> allEnemyMoves;
    for (const std::unique_ptr<Piece> &piece : enemyPieces)
        if (piecePositions.count(piece.get()) && piecePositions.at(piece.get()))
            allEnemyMoves.merge(getValidMoves(*piecePositions.at(piece.get()), MoveType::Any));
    log("\tList of possible enemy moves generated");
    
    if (allEnemyMoves.empty()) return Status::PlayerWin;

    Move move = enemyAlgoBasic(allEnemyMoves, board);
    log("\tEnemy move selected, executing move...");

    return executeMove(move);
}

GameInstance::Status GameInstance::getWinStatus() {
    switch (mission) {
        case MissionType::HoldThePoint:
            if (objectives[0]->isCapturedBy == Player::Human) {
                log("getWinStatus is Player Win");
                return Status::PlayerWin;
            } else if (objectives[0]->isCapturedBy == Player::CPU) {
                log("getWinStatus is Enemy Win");
                return Status::EnemyWin;
            } else {
                log("getWinStatus is Next");
                return Status::Next;
            }
        default:
            log("Error: no valid mission type");
            ASSERT(mission == MissionType::HoldThePoint);
    }
    log("getWinStatus is Quit");
    return Status::Quit;
}
