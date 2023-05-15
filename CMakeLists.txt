
cmake_minimum_required(VERSION 3.23)
project(PecheauCanard C)

set(CMAKE_C_STANDARD 23)

add_executable(PecheauCanard main.c)
target_link_libraries(PecheauCanard -lmingw32 -lalleg44 -llibpng16 -llibloadpng -llibjpgalleg)