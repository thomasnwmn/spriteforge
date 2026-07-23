cmake_minimum_required(VERSION 3.10)
project(SpriteForgeC)

set(CMAKE_C_STANDARD 99)

find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)
find_package(SDL2_ttf CONFIG REQUIRED)
find_package(Lua REQUIRED)

add_executable(spriteforge src/main.c src/input.c src/render.c src/asset_manager.c src/entity.c src/scripting.c src/launcher.c)

target_link_libraries(spriteforge
    PRIVATE
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
    $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
    $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>
    ${LUA_LIBRARIES}
)

target_include_directories(spriteforge PRIVATE src ${LUA_INCLUDE_DIR})

# Automatically copy the game folders to the build directory every time we build!
add_custom_command(TARGET spriteforge POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/example_projects"
        "$<TARGET_FILE_DIR:spriteforge>/example_projects"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/user_projects"
        "$<TARGET_FILE_DIR:spriteforge>/user_projects"
)