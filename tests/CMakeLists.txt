add_executable(main_test test.cpp)

target_link_libraries(main_test PRIVATE Game_Instance Run_Instance shared ncurses ${CURSES_LIBRARIES})

add_test(NAME main_test COMMAND test)
