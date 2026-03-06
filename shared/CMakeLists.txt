add_library(shared STATIC
    src/pieces.cpp
)

target_include_directories(shared
    PUBLIC include/
)
