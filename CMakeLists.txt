cmake_minimum_required(VERSION 3.30)
project(TOi)

set(CMAKE_CXX_STANDARD 17)

add_executable(TOi main.cpp
        CFG.cpp
        CFG.h
        PDA.cpp
        PDA.h)
