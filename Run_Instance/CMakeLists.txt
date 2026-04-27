add_library(Run_Instance STATIC
    src/runinstance.cpp
)

target_include_directories(Run_Instance
    PRIVATE 
        src/
        ${CMAKE_SOURCE_DIR}/Game_Instance/include/
    PUBLIC include/
)

target_link_libraries(Run_Instance PRIVATE shared)
