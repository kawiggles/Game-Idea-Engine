cmake_minimum_required(VERSION 4.2.3)

project(Chess2)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
find_package(Curses REQUIRED)
set(CURSES_NEED_NCURSES TRUE)

add_subdirectory(shared)
add_subdirectory(Game_Instance)
add_subdirectory(Run_Instance)
add_subdirectory(tests)

add_executable(Chess2 tests/src/main.cpp)
target_link_libraries(Chess2 PRIVATE Game_Instance Run_Instance shared tests ${CURSES_LIBRARIES})
