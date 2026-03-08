add_library(Game_Instance STATIC
    src/boards.cpp
    src/gameinstance.cpp
    src/tiles.cpp
    src/enemy.cpp
)

target_include_directories(Game_Instance
    PRIVATE src/
    PUBLIC include/
)

target_link_libraries(Game_Instance PRIVATE shared)
