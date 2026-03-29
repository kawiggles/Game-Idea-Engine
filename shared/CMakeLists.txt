add_library(shared STATIC
    src/pieces.cpp
    src/logs.cpp
)

target_include_directories(shared
    PUBLIC include/
)
