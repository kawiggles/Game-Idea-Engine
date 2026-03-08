#include "gameinstance.hpp"
#include "boards.hpp"
#include "pieces.hpp"
#include "tiles.hpp"
#include "types.hpp"

#include <iostream>
#include <vector>
#include <random>

// Game Instance constructor, used to initialize a game instance
GameInstance::GameInstance(unsigned int seed, BiomeType biome, MissionType mission, int octave, bool hasRoad) {
    this->biome = biome;
    this->mission = mission;
    this->octave = octave;
    this->seed = seed;
    road = hasRoad;

    // Random width and height
    std::mt19937 gen(seed);
    std::uniform_int_distribution<int> dis(8, 20);
    boardHeight = dis(gen);
    boardWidth = dis(gen);
}

// This function actually initalizes the game instance, taking values from the constructor to generate a board
void GameInstance::makeGame(std::vector<Piece> runPieces, std::vector<Piece> enemyPieces) {
    board = makeBoard(GameInstance::boardWidth, GameInstance::boardHeight, GameInstance::seed, GameInstance::octave, GameInstance::biome, GameInstance::mission, GameInstance::road);
    this->playerPieces = runPieces; 
    this->enemyPieces = enemyPieces;
}

// Widely used function to get a tile from an (x, y) coordinate
Tile * GameInstance::getTile(int x, int y) {
    // Return nullptr if no tile is found at the given coordinates
    if (x < 0 || x >= board.width || y < 0 || y >= board.height) {
        return nullptr;
    }
    return &board.tiles[(y * board.width) + x];
}

// Widely used function to check if a piece exists in a game instance
bool GameInstance::pieceExists(Piece * piece) {
    if (piece->ownedByPlayer) {
        for (Piece &p : playerPieces) {
            if (&p == piece) {
                return true;
            }
        }
    } else {
        for (Piece &p : enemyPieces) {
            if (&p == piece) {
                return true;
            }
        }
    }
    return false;
}

// Function to add a piece from the game instance to a tile on the board by setting that pieces pointer to the 
// occupyingPiece value of the tile. Used at beginning of game instance.
bool GameInstance::addPiece(Piece * piece, int x, int y) {
    if (x < 0 || y < 0 || x >= board.width || y >= board.height) {
        std::cout << "Fail, coordinates out of bounds" << std::endl;
        return false; // Fail condition 1, out of bounds
    }
    
    if (!pieceExists(piece)) {
        std::cout << "Fail, piece not in game instance" << std::endl;
        return false; // Fail condition 2, piece not in game
    }

    Tile * tile = getTile(x, y);
    if (tile->occupyingPiece != nullptr) {
        return false;
    } // Fail condition 3, tile occupied
    tile->occupyingPiece = piece;
    return true;
}

// Quick function to get the tile a piece is on, as pieces don't belong to tiles.
Tile * GameInstance::getPieceTile(Piece * piece) {
    for (Tile &t : board.tiles) {
        if (t.occupyingPiece == piece) {
            return &t;
        }
    }
    return nullptr;
}

// This function creates an array of valid move targets for a passed piece object It's going to be a particularly complicated piece of code, so prepare for a lot of comments
std::vector<Tile *> GameInstance::getValidMoves(Piece * piece) {
    Tile * currentTile = getPieceTile(piece);    

    int cardinalMax = piece->maxCardinal;
    int diagonalMax = piece->maxDiagonal; 
    int relativeStrength = piece->strength; // These will be modified by runes and terrain, but are the base value for now

    std::vector<Tile *> validTiles; // validTiles is the vector we'll return
    validTiles.reserve((cardinalMax * 4) + (diagonalMax * 4)); // I could write a ton of logic to get this to work with the inifinite movement pieces, but fuck that shit, it can be massive in those instances

/*  // Test code, loop to return the enire board as valid tiles
    validTiles.reserve(board.tiles.size());
    for (Tile &t : board.tiles) {
        validTiles.push_back(&t);
    }
*/

// We're doing linear algebra, we need an array of arrays. This is some C shit, baby! 
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

            Tile * checkTile = getTile(currentTile->x + (vectors[i][0]*j), currentTile->y + (vectors[i][1]*j)); // The tile being checked. Because linear algebra, works in all directions

            if (!checkTile) break; // Breaks loop if tile doesn't exist
            
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
            if (stopMove == true) break; //If any terrain rules trigger this, break the loop

            if (checkTile->occupyingPiece) { // Now logic for if the tile is occupied
                if (checkTile->occupyingPiece->ownedByPlayer == piece->ownedByPlayer) { // If it's friendly, check if we can move through it.
                    if (piece->canMoveThroughPieces) {
                        continue;
                    } else {
                        break;
                    }
                } else { // If it's not, we do a strength/toughness comparison
                    int relativeToughness = checkTile->occupyingPiece->toughness + relativeToughnessMod; // Like strength, will be altered by terrain and runes later

                    if (relativeStrength + relativeStrengthMod >= relativeToughness) {
                        validTiles.push_back(checkTile);
                        break;
                    }
                    break;
                }
            } else {
                validTiles.push_back(checkTile); // If the tile isn't occupied, simply add it to the array
            }
        }
    }

    return validTiles;

}

// Function to move a piece from one tile to another, provided the checks of getValidMoves and pieceExists are passed. Also allows for the "capture" of pieces 
bool GameInstance::movePiece(Piece * piece, int x, int y) {
    if (x < 0 || y < 0 || x >= board.width || y >= board.height) {
        std::cout << "Fail, coordinates out of bounds. Error in movePiece." << std::endl;
        return false; // Fail condition 1, out of bounds
    }
    
    if (!pieceExists(piece)) {
        std::cout << "Fail, piece not in game instance. Error in movePiece." << std::endl;
        return false; // Fail condition 2, piece not in game
    }

    Tile * currentTile = getPieceTile(piece);
    Tile * targetTile = getTile(x, y);
    std::vector<Tile *> validTiles = getValidMoves(piece);
    
    /* Debug Code
    std::cout << "DEBUG: Valid moves count = " << validTiles.size() << std::endl;
    std::cout << "DEBUG: Valid moves for piece: " << std::endl;
    for (Tile* vt : validTiles) {
        std::cout << "  (" << vt->x << ", " << vt->y << ") ptr=" << vt << std::endl;
    }
    */

    if (!currentTile) {
        std::cout << "Fail, piece not on board. Error in movePiece." << std::endl;
        return false; // Fail condition 3, piece not on board
    }

    for (int i = 0; i < validTiles.size(); i++) {
        if (targetTile == validTiles[i]) {
            /* Debug Code
            std::cout << "DEBUG: Attempting to move piece to (" << x << ", " << y << ")" << std::endl;
            std::cout << "DEBUG: Piece currently at (" << currentTile->x << ", " << currentTile->y << ")" << std::endl;
            */
            currentTile->occupyingPiece = nullptr;
            targetTile->occupyingPiece = piece;
            return true;
        }
    }

    std::cout << "Fail, invalid move." << std::endl;
    std::cout << "Target was: " << targetTile << " at (" << x << ", " << y << ")" << std::endl; 
    return false; // Fail condition 4, rare exception
}
