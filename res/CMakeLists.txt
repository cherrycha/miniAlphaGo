cmake_minimum_required(VERSION 3.5)
project(reversi)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES reversi.c resource.h chess_board.c MCTS.c resource.rc)
add_executable(reversi ${SOURCE_FILES})