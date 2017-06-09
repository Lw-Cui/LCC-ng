cmake_minimum_required(VERSION 3.2)
project(LCC-ng)

set(CMAKE_C_STANDARD 11)

set(LCC_SOURCE_FILES
        main.c
        lex.yy.c
        y.tab.h y.tab.c
        )

add_executable(LCC ${LCC_SOURCE_FILES})