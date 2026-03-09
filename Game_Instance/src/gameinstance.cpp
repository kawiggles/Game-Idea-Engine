#include "gameinstance.hpp"
#include "pieces.hpp"
#include "tiles.hpp"
#include "types.hpp"
#include "PerlinNoise.hpp"
#include "enemy.hpp"

#include <iostream>
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
    std::uniform_int_distribution<int> dis(8, 20);
    boardHeight = dis(gen);
    boardWidth = dis(gen);
}

void GameInstance::makeGame(std::vector<Piece *> runPieces, std::vector<Piece *> enemyPieces) {
    std::cout << "Generating game instance..." <<std::endl;
    std::cout << "Generating board of dimensions " << boardWidth << " by " << boardHeight << "." <<std::endl;
    std::vector<Tile> board;
    board.reserve(boardWidth * boardHeight); 

    std::mt19937 gen(seed);
    
    const siv::PerlinNoise noiseMap{ seed }; // Generate perlin noise map from seed

    for (int y = 0; y < boardHeight; y++) {
        for (int x = 0; x < boardWidth; x++) {
            // (x, y) coordinates are normalized in order to account for perlin noise frequency
            float nx = (float)x / boardWidth;
            float ny = (float)y / boardHeight;
            float noiseValue = noiseMap.octave2D_01((nx * 4.0f), (ny * 4.0f), octave); // Get noise value depending on normalized (x, y) and octave 

            board.push_back(Tile{x, y, getRandomTerrain(noiseValue, biome), nullptr});
            // (x, y), terrain type, and null pointer representing no occupying piece
        }
    }
    std::cout << "Base board generated" << std::endl;

    if (hasRoad) {
        std::cout << "Generating road." << std::endl;
        Tile * startRoad;
        Tile * endRoad;
        for (int i = 0; i < boardWidth; i++) {
            Tile * currentTile = &board[i];
            if (currentTile->terrain == TerrainType::Field || currentTile->terrain == TerrainType::Forest) {
                startRoad = currentTile;
                break;
            }
        }
        
        for (int i = boardWidth; i >= 0; i--) {
            Tile * currentTile = &board[i * (boardHeight-1) + (boardWidth-1)];
            if (currentTile->terrain == TerrainType::Field || currentTile->terrain == TerrainType::Forest) {
                endRoad = currentTile;
                break;
            }
        }

        std::vector<Tile *> road;
        if (startRoad != nullptr && endRoad != nullptr) road = generateRoad(startRoad, endRoad, board, boardWidth, boardHeight);
        for (Tile * tile : road) {
            if (tile != nullptr) {
                tile->terrain = TerrainType::Road;
            }
        }
        std::cout << "Road generated." << std::endl;
    }

    switch (mission) {
        case MissionType::HoldThePoint: {
            std::cout << "Mission type is Hold the Point";
            std::uniform_int_distribution<int> xDist(0, boardWidth);
            std::uniform_int_distribution<int> yDist(0, boardHeight);
            int objX = xDist(gen);
            int objY = yDist(gen);
            board[objX * boardWidth + objY].terrain = TerrainType::Objective;
            break;
        }
        default:
            std::cout << "Error, no mission type for the Game Instance" << std::endl;
    }

    std::cout << "Copying board and pieces to game instance." << std::endl;
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
        std::cout << "Fail, coordinates out of bounds" << std::endl;
        return false; // Fail condition 1, out of bounds
    }
    
    if (!pieceExists(piece)) {
        std::cout << "Fail, piece not in game instance" << std::endl;
        return false; // Fail condition 2, piece not in game
    }

    Tile * tile = getTile(x, y);
    if (tile->occupyingPiece != nullptr) {
        std::cout << "Fail, tile already occupied" << std::endl;
        return false;
    } // Fail condition 3, tile occupied
    if (tile->terrain == TerrainType::Water) {
        std::cout << "Fail, can't place pieces on water tiles" << std::endl;
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
// It's going to be a particularly complicated piece of code, so prepare for a lot of comments
std::vector<Move> GameInstance::getValidMoves(Piece * piece) {
    Tile * currentTile = getPieceTile(piece);    

    int cardinalMax = piece->maxCardinal;
    int diagonalMax = piece->maxDiagonal; 
    int relativeStrength = piece->strength; 

    std::vector<Move> validTiles; // validTiles is the vector we'll return
    validTiles.reserve((cardinalMax * 4) + (diagonalMax * 4)); 

// An array of vectors representing each possible direction of movement, used to code invariant below  
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

// We know there's only 8 vectors to loop through, so we can set the number of iterations manually 
    for (int i = 0; i < 8; i++) { // i iterates through vectors
            bool stopMove = false;
            int cardinalEval = cardinalMax;
            int diagonalEval = diagonalMax;
        for (int j = 1; (i < 4) ? j <= cardinalEval : j <= diagonalEval; j++) { 
        // j iterates through tiles on that vector. The ternary operator in the middle of this loop determines if the number of tiles iterated through is limited by the cardinalEval or diagonalEval value, corresponding with the array of vectors. 

            Tile * checkTile = getTile(currentTile->x + (vectors[i][0]*j), currentTile->y + (vectors[i][1]*j)); // The tile being checked 
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
                         piece->category == PieceCategory::Siege) (i < 4) ? cardinalEval-- : diagonalEval--; // Decrease movement by one
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
            }

            if ((i < 4) ? j > cardinalEval : j > diagonalEval) break; // If terrain rules caused j to exceed the move limit, break 
            if (stopMove == true) break; // Had to use this bool because breaks don't work in switches and I don't like massive if statements

            // Now logic for if the tile is occupied
            if (checkTile->occupyingPiece) { 
                // If it's friendly, check if we can move through it.
                if (checkTile->occupyingPiece->ownedByPlayer == piece->ownedByPlayer) { 
                    if (piece->canMoveThroughPieces) {
                        continue;
                    } else {
                        break;
                    }
                } else { // If it's not, we do a strength/toughness comparison
                    int relativeToughness = checkTile->occupyingPiece->toughness + relativeToughnessMod; // Like strength, will be altered by terrain and runes later

                    if (relativeStrength + relativeStrengthMod >= relativeToughness) {
                        validTiles.push_back(Move{MoveType::Move, currentTile, checkTile});
                        break;
                    }
                    break;
                }
            } else {
                validTiles.push_back(Move{MoveType::Move, currentTile, checkTile}); // If the tile isn't occupied, simply add it to the vector
            }
        }
    }

    return validTiles;

}

// Function to move a piece from one tile to another, provided the checks of getValidMoves and pieceExists are passed. Also allows for the "capture" of pieces 
bool GameInstance::movePiece(Piece * piece, Tile * target) {
    if (!target) {
        std::cout << "Fail, target tile out of bounds" << std::endl;
        return false; // Fail condition 1, out of bounds
    }
    
    if (!pieceExists(piece)) {
        std::cout << "Fail, piece not in game instance" << std::endl;
        return false; // Fail condition 2, piece not in game
    }

    Tile * currentTile = getPieceTile(piece);
    std::vector<Move> validTiles = getValidMoves(piece);
    
    /* Debug Code
    std::cout << "DEBUG: Valid moves count = " << validTiles.size() << std::endl;
    std::cout << "DEBUG: Valid moves for piece: " << std::endl;
    for (Move vt : validTiles) {
        std::cout << "  (" << vt.to->x << ", " << vt.to->y << ") ptr=" << &vt << std::endl;
    }
    */

    if (!currentTile->occupyingPiece) {
        std::cout << "Fail, piece not on board" << std::endl;
        return false; // Fail condition 3, piece not on board
    }

    for (int i = 0; i < validTiles.size(); i++) {
        if (target == validTiles[i].to) {
            /* Debug Code
            std::cout << "DEBUG: Attempting to move piece to (" << x << ", " << y << ")" << std::endl;
            std::cout << "DEBUG: Piece currently at (" << currentTile->x << ", " << currentTile->y << ")" << std::endl;
            */
            int index = 0;
            if (currentTile->occupyingPiece->ownedByPlayer) {
                while (playerPieces[index] != currentTile->occupyingPiece) index++;
                playerPieces.erase(playerPieces.begin() + index);
            } else {
                while (enemyPieces[index] != currentTile->occupyingPiece) index++;
                enemyPieces.erase(enemyPieces.begin() + index);
            }
            currentTile->occupyingPiece = nullptr;
            target->occupyingPiece = piece;
            std::cout << "Piece moved from (" << currentTile->x+1 << ", " << currentTile->y+1 << ") to (" << target->x+1 << ", " << target->y+1 << ")" << std::endl;
            return true;
        }
    }

    std::cout << "Fail, invalid move." << std::endl;
    std::cout << "Target was: " << target << " at (" << target->x << ", " << target->y << ")" << std::endl; 
    return false; // Fail condition 4, rare exception
}

bool GameInstance::isMissionComplete() {
    switch (mission) {
        case MissionType::HoldThePoint:
            break;
        default: return false;
    }
}

int GameInstance::takePlayerTurn(Move move) {
    int turnStatus = 0;
    bool moveComplete = false;
    switch (move.type) {
        case MoveType::Move: 
            std::cout << "Attempting move" << std::endl;
            moveComplete = movePiece(move.from->occupyingPiece, move.to);
            break;
        case MoveType::Shoot:
            std::cout << "Attempting to shoot" << std::endl;
            break;
        default:
            std::cout << "Error, move type not recognized" << std::endl;
    }

    if (!moveComplete) return 0;
    return (isMissionComplete()) ? 2 : 1;
}

int GameInstance::takeEnemyTurn() {
    std::vector<Move> allEnemyMoves;
    for (Piece * piece : enemyPieces) {
        std::vector<Move> pieceValidMoves = getValidMoves(piece);
        allEnemyMoves.insert(allEnemyMoves.end(), pieceValidMoves.begin(), pieceValidMoves.end());
    }
    
    Move move = enemyAlgoBasic(allEnemyMoves, board);

    int turnStatus = 0;
    bool moveComplete = false;
    switch (move.type) {
        case MoveType::Move: 
            std::cout << "Enemy attempting move" << std::endl;
            moveComplete = movePiece(move.from->occupyingPiece, move.to);
            break;
        case MoveType::Shoot:
            std::cout << "Enemy attempting to shoot" << std::endl;
            break;
        default:
            std::cout << "Error, enemy move type not recognized" << std::endl;
    }

    if (!moveComplete) return 0;
    return (isMissionComplete()) ? 2 : 1;
}
