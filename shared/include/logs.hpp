#pragma once

#include "pieces.hpp"
#include "types.hpp"

#include <string>
#include <cstdio>
#include <ctime>
#include <cstdarg>

inline FILE * LOG_FILE = nullptr;

inline void initLogger() {
    LOG_FILE = fopen("logs/game_log.txt", "w");
    if (!LOG_FILE) {
        perror("Failed to open log file");
        return;
    }
    time_t currentTime = time(nullptr);
    fprintf(LOG_FILE, "=== Session Log Starting At: %s ===\n", ctime(&currentTime));
    fflush(LOG_FILE);
}

inline void closeLogger() {
    if (LOG_FILE) {
        fprintf(LOG_FILE, "=== End Session Log ===\n");
        fclose(LOG_FILE);
        LOG_FILE = nullptr;
    }
}

inline void log(const char * fmt, ...) {
    if (!LOG_FILE) return;
    va_list args;
    va_start(args, fmt);
    vfprintf(LOG_FILE, fmt, args);
    va_end(args);
    fprintf(LOG_FILE, "\n");
    fflush(LOG_FILE);
}

std::string getPieceType(const Piece * piece);
std::string getBiomeType(const BiomeType biome);
std::string getTerrainType(const TerrainType terrain);
std::string getMoveType(const MoveType move);
