cmake_minimum_required(VERSION 3.15)
project(Dama)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(Dama
        play.cpp
        player.cpp
        player.hpp
        )
