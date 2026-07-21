cmake_minimum_required(VERSION 3.10)
project(SpriteForgeC)

set(CMAKE_C_STANDARD 99)

find_package(SDL2 CONFIG REQUIRED)

add_executable(spriteforge src/main.c)

target_link_libraries(spriteforge
    PRIVATE
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
)

target_include_directories(spriteforge PRIVATE src)