cmake_minimum_required(VERSION 3.2)
project(LCC-ng)

set(CMAKE_C_STANDARD 11)

set(LCC_SOURCE_FILES
        main.c
        lcc.c lcc.h lcc.internal.h
        ADT.h ADT.c)

add_executable(LCC ${LCC_SOURCE_FILES})