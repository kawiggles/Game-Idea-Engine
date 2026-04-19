#pragma once

#include "pieces.hpp"
#include "types.hpp"

#include <string>
#include <cstdio>
#include <ctime>
#include <cstdarg>

#define DEBUG

#ifndef DEBUG
#define ASSERT(n)
#define LOG
#else
#define ASSERT(n) \
	if(!(n)) { \
	debug("%s - Failed\n", #n); \
	debug("At %s ", __TIME__); \
	debug("in file %s ", __FILE__); \
	debug("at line %d\n", __LINE__); \
    closeLogger(); \
	exit(1); }
#define LOG log
#endif

inline FILE * LOG_FILE = nullptr;
inline FILE * DEBUG_FILE = nullptr;

inline void initLogger() {
    LOG_FILE = fopen("logs/game_log.txt", "w");
    if (!LOG_FILE) {
        perror("Failed to open log file");
        return;
    }
    DEBUG_FILE = fopen("logs/debug.txt", "w");
    if (!DEBUG_FILE) {
        perror("Failed to open debug file");
        return;
    }
    time_t currentTime = time(nullptr);
    fprintf(LOG_FILE, "=== Session Log Starting At: %s ===\n", ctime(&currentTime));
    fprintf(DEBUG_FILE, "=== Debug Log Starting At: %s ===\n", ctime(&currentTime));
    fflush(LOG_FILE);
    fflush(DEBUG_FILE);
}

inline void closeLogger() {
    if (LOG_FILE) {
        fprintf(LOG_FILE, "=== End Session Log ===\n");
        fclose(LOG_FILE);
        LOG_FILE = nullptr;
    }
    if (DEBUG_FILE) {
        fprintf(DEBUG_FILE, "=== End Debug Log ===\n");
        fclose(DEBUG_FILE);
        DEBUG_FILE = nullptr;
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

inline void debug(const char * fmt, ...) {
    if (!DEBUG_FILE) return;
    va_list args;
    va_start(args, fmt);
    vfprintf(DEBUG_FILE, fmt, args);
    va_end(args);
    fprintf(DEBUG_FILE, "\n");
    fflush(DEBUG_FILE);
}

std::string getPieceType(const Piece * piece);
std::string getBiomeType(const BiomeType biome);
std::string getTerrainType(const TerrainType terrain);
std::string getMoveType(const MoveType move);
