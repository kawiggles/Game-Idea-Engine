add_library(ncurses STATIC
    src/panels.cpp
    src/interface.cpp
)

target_include_directories(ncurses
    PRIVATE 
        src/
        ${CMAKE_SOURCE_DIR}/Game_Instance/include
    PUBLIC include/
)

target_link_libraries(ncurses PRIVATE shared ${CURSES_LIBRARIES})
