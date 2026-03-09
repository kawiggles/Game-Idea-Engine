add_library(tests STATIC
    src/main.cpp
    src/printboards.cpp
)

target_include_directories(tests
    PRIVATE 
        src/
        ${CMAKE_SOURCE_DIR}/Game_Instance/include
    PUBLIC include/
)

target_link_libraries(tests PRIVATE shared)
