cmake_minimum_required(VERSION 4.2.3)

project(Game_Idea)

add_executable(TestBuild)

target_sources(TestBuild
    PRIVATE
        src/main.cpp
        src/gameinstance.cpp
        src/boards.cpp
        src/tiles.cpp
        src/pieces.cpp
        src/runes.cpp
    
    PUBLIC
        FILE_SET headers
        TYPE HEADERS
        BASE_DIRS
            include
        FILES
            include/gameinstance.hpp
            include/boards.hpp
            include/pieces.hpp
            include/tiles.hpp
            include/runes.hpp
)