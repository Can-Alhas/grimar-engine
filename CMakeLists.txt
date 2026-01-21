cmake_minimum_required(VERSION 3.21)

project(Grimar LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---- Engine Library ----
add_library(grimar_engine STATIC
        src/core/dummy.cpp
        include/grimar/core/Assert.hpp
)

target_include_directories(grimar_engine PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_sources(grimar_engine PRIVATE
        src/core/dummy.cpp
)


find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)
find_package(SDL2_ttf CONFIG REQUIRED)
find_package(SDL2_mixer CONFIG REQUIRED)

target_link_libraries(grimar_engine PUBLIC
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
        SDL2::SDL2main

        $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
        $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>
        $<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,SDL2_mixer::SDL2_mixer,SDL2_mixer::SDL2_mixer-static>
)

# ---- Sandbox executable (boot) -----
add_executable(grimar_sandbox
        src/main.cpp
)
target_link_libraries(grimar_sandbox PRIVATE grimar_engine)

# ---- Copy runtime DLLs next to exe (Windows) ----
# Windows'ta gerekli DLL'leri exe yan?na otomatik kopyala
if (WIN32)
        add_custom_command(TARGET grimar_sandbox POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_RUNTIME_DLLS:grimar_sandbox>
                $<TARGET_FILE_DIR:grimar_sandbox>
                COMMAND_EXPAND_LISTS
        )
endif()

