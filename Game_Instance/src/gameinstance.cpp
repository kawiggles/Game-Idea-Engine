#include "gameinstance.hpp"
#include "pieces.hpp"
#include "tiles.hpp"
#include "types.hpp"
#include "PerlinNoise.hpp"
#include "enemy.hpp"

#include <ncurses.h>
#include <vector>
#include <random>

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
 * Then, takeTurn handles the turn by turn logic, which are repeated instances of movePiece.
 */

GameInstance::GameInstance(unsigned int seed, BiomeType biome, MissionType mission, int octave, bool hasRoad) {
    this->biome = biome;
    this->mission = mission;
    this->octave = octave;
    this->seed = seed;
    this->hasRoad = hasRoad;

    std::mt19937 gen(seed); // Random width and height
    std::uniform_int_distribution<int> dis(5, 10);
    boardHeight = dis(gen);
    boardWidth = dis(gen);
}

void GameInstance::makeGame(std::vector<Piece *> runPieces, std::vector<Piece *> enemyPieces) {
    printw("Generating game instance...\n");
    printw("Generating board of dimensions %d by %d.\n", boardWidth, boardHeight);
    std::vector<Tile> board;
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
    printw("Base board generated\n");

    if (hasRoad) {
        printw("Generating road.\n");
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
        } while (endRoad->terrain != TerrainType::Field && startRoad->terrain != TerrainType::Forest);

        std::vector<Tile *> road;
        if (startRoad != nullptr && endRoad != nullptr) road = generateRoad(startRoad, endRoad, board, boardWidth, boardHeight);
        for (Tile * tile : road) {
            if (tile != nullptr) {
                tile->terrain = TerrainType::Road;
            }
        }
        printw("Road generated\n");
    }

    switch (mission) {
        case MissionType::HoldThePoint: {
            printw("Mission type is Hold the Point\n");
            std::uniform_int_distribution<int> xDist(0, boardWidth);
            std::uniform_int_distribution<int> yDist(4, boardHeight-4);
            int objX = xDist(gen);
            int objY = yDist(gen);
            board[objY * boardWidth + objX].terrain = TerrainType::Objective;
            break;
        }
        default:
            printw("Error, no mission type for the Game Instance\n");
    }

    printw("Copying board and pieces to game instance...\n");
    this->board = std::move(board);
    this->playerPieces = runPieces; 
    this->enemyPieces = enemyPieces;
}

Tile * GameInstance::getTile(int x, int y) {
    if (x < 0 || x >= boardWidth || y < 0 || y >= boardHeight) {
        return nullptr;
    }
    return &board[(y * boardWidth) + x];
}

bool GameInstance::pieceExists(Piece * piece) {
    if (piece->ownedByPlayer) {
        for (Piece * p : playerPieces) {
            if (p == piece) {
                return true;
            }
        }
    } else {
        for (Piece * p : enemyPieces) {
            if (p == piece) {
                return true;
            }
        }
    }
    return false;
}

bool GameInstance::addPiece(Piece * piece, int x, int y) {
    if (x < 0 || y < 0 || x >= boardWidth || y >= boardHeight) {
        printw("Fail, coordinates out of bounds\n");
        return false; // Fail condition 1, out of bounds
    }
    
    if (!pieceExists(piece)) {
        printw("Fail, piece not in game instance\n");
        return false; // Fail condition 2, piece not in game
    }

    Tile * tile = getTile(x, y);
    if (tile->occupyingPiece != nullptr) {
        printw("Fail, tile already occupied\n");
        return false;
    } // Fail condition 3, tile occupied
    if (tile->terrain == TerrainType::Water) {
        printw("Fail, can't place pieces on water tiles\n");
        return false;
    } // Fail condition 4, tile invalid

    tile->occupyingPiece = piece;
    return true;
}

Tile * GameInstance::getPieceTile(Piece * piece) {
    for (Tile &t : board) {
        if (t.occupyingPiece == piece) {
            return &t;
        }
    }
    return nullptr;
}

// This function creates an array of valid move structs for a passed piece object 
std::vector<Move> GameInstance::getValidMoves(Piece * piece) {
    Tile * currentTile = getPieceTile(piece);    

    int cardinalMax = piece->maxCardinal;
    int diagonalMax = piece->maxDiagonal; 
    int relativeStrength = piece->strength; 

    std::vector<Move> validTiles;
    validTiles.reserve((cardinalMax * 4) + (diagonalMax * 4)); 

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
            int cardinalEval = cardinalMax;
            int diagonalEval = diagonalMax;
        for (int j = 1; (i < 4) ? j <= cardinalEval : j <= diagonalEval; j++) { 
        // j iterates through tiles on that vector. The ternary operator in the middle of this loop determines if the number of tiles iterated through is limited by the cardinalEval or diagonalEval value, corresponding with the array of vectors. 

            Tile * checkTile = getTile(currentTile->x + (vectors[i][0]*j), currentTile->y + (vectors[i][1]*j));
            if (!checkTile) break; // Fail case if tile doesn't exist
            
            // Terrain Logic Here
            int relativeToughnessMod = 0;
            int relativeStrengthMod = 0;

            switch (checkTile->terrain) {
                case TerrainType::Field:
                    break;
                case TerrainType::Forest:
                    if ((piece->category == PieceCategory::Cavalry && 
                         piece->type != PieceType::LCavalry) || 
                         piece->category == PieceCategory::Siege) (i < 4) ? cardinalEval-- : diagonalEval--;
                    relativeToughnessMod++;
                    break;
                case TerrainType::Water:
                    stopMove = true;
                    break;
                case TerrainType::Mountain:
                    if (piece->category == PieceCategory::Cavalry || 
                        piece->category == PieceCategory::Siege) {
                        stopMove = true;
                    } else {
                        (i < 4) ? cardinalEval-- : diagonalEval--;
                    }
                    relativeStrengthMod++;
                    break;
                case TerrainType::Road: {
                    Tile * nextTile = getTile(checkTile->x + vectors[i][0], checkTile->y + vectors[i][0]);
                    if (nextTile != nullptr &&
                        nextTile->terrain != TerrainType::Water &&
                        nextTile->terrain == TerrainType::Road) (i < 4) ? cardinalEval++ : diagonalEval++; // Beautiful
                    break;
                    }
                case TerrainType::Desert:
                    relativeToughnessMod--;
                    (i < 4) ? cardinalEval-- : diagonalEval--;
                    break;
                case TerrainType::Jungle:
                    if (piece->category == PieceCategory::Siege) {
                        stopMove = true;
                        break;
                    }
                    if (!(piece->type == PieceType::Light)) (i < 4) ? cardinalEval-- : diagonalEval--;
                    if (piece->category == PieceCategory::Cavalry) (i < 4) ? cardinalEval-- : diagonalEval--;
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
                    if (relativeStrength < 2) stopMove = true;
                    relativeToughnessMod++;
                    break;
            }

            // Capture Logic Here
            if ((i < 4) ? j > cardinalEval : j > diagonalEval) break;
            if (stopMove == true) break;

            if (checkTile->occupyingPiece) { 
                if (checkTile->occupyingPiece->ownedByPlayer == piece->ownedByPlayer) { 
                    if (piece->canMoveThroughPieces) {
                        continue;
                    } else {
                        break;
                    }
                } else {
                    int relativeToughness = checkTile->occupyingPiece->toughness + relativeToughnessMod;

                    if (relativeStrength + relativeStrengthMod >= relativeToughness) {
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

// Function to move a piece from one tile to another, provided the checks of getValidMoves and pieceExists are passed. Also allows for the "capture" of pieces 
bool GameInstance::movePiece(Piece * piece, Tile * target) {
    if (!target) {
        printw("Fail, target tile out of bounds\n");
        return false; // Fail condition 1, out of bounds
    }
    
    if (!pieceExists(piece)) {
        printw("Fail, piece not in game instance\n");
        return false; // Fail condition 2, piece not in game
    }

    Tile * currentTile = getPieceTile(piece);
    std::vector<Move> validTiles = getValidMoves(piece);

    if (!currentTile->occupyingPiece) {
        printw("Fail, piece not on board\n");
        return false; // Fail condition 3, piece not on board
    }

    for (int i = 0; i < validTiles.size(); i++) {
        if (target == validTiles[i].to) {
            int index = 0;
            if (target->occupyingPiece) {
                auto &pieces = target->occupyingPiece->ownedByPlayer ? playerPieces : enemyPieces;
                auto it = std::find(pieces.begin(), pieces.end(), target->occupyingPiece);
                if (it != pieces.end()) pieces.erase(it);
            }
            currentTile->occupyingPiece = nullptr;
            target->occupyingPiece = piece;
            printw("Piece moved from (%d, %d) to (%d ,%d)\n", currentTile->x+1, currentTile->y+1, target->x+1, target->y+1);
            return true;
        }
    }

    printw("Fail, invalid move.\n");
    printw("Target was: (%d, %d)\n", target->x+1, target->y+1); 
    return false; // Fail condition 4, rare exception
}

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
    int turnStatus = 0;
    bool moveComplete = false;
    switch (move.type) {
        case MoveType::Move: 
            printw("Attempting move\n");
            moveComplete = movePiece(move.from->occupyingPiece, move.to);
            break;
        case MoveType::Shoot:
            printw("Attempting to shoot\n");
            break;
        default:
            printw("Error, move type not recognized\n");
    }

    if (!moveComplete) return 0;
    return isMissionComplete();
}

int GameInstance::takeEnemyTurn() {
    std::vector<Move> allEnemyMoves;
    for (Piece * piece : enemyPieces) {
        std::vector<Move> pieceValidMoves = getValidMoves(piece);
        allEnemyMoves.insert(allEnemyMoves.end(), pieceValidMoves.begin(), pieceValidMoves.end());
    }
    
    Move move = enemyAlgoRandom(allEnemyMoves, board);

    int turnStatus = 0;
    bool moveComplete = false;
    switch (move.type) {
        case MoveType::Move: 
            printw("Enemy attempting move from (%d, %d) to (%d, %d)\n", move.from->x+1, move.from->y+1, move.to->x+1, move.to->y+1);
            moveComplete = movePiece(move.from->occupyingPiece, move.to);
            break;
        case MoveType::Shoot:
            printw("Enemy attempting to shoot\n");
            break;
        default:
            printw("Error, enemy move type not recognized\n");
    }

    if (!moveComplete) return 0;
    return isMissionComplete();
}
