cmake_minimum_required(VERSION 3.21)

project(Grimar LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)

# ============================================================
# Dependencies
# ============================================================

# Support both vcpkg CONFIG packages and native distro installations.
find_package(PkgConfig QUIET)

find_package(SDL2 CONFIG REQUIRED)

find_package(SDL2_image CONFIG QUIET)

if (TARGET SDL2_image::SDL2_image)
    set(GRIMAR_SDL2_IMAGE_TARGET SDL2_image::SDL2_image)
elseif (TARGET SDL2_image::SDL2_image-static)
    set(GRIMAR_SDL2_IMAGE_TARGET SDL2_image::SDL2_image-static)
elseif (PkgConfig_FOUND)
    pkg_check_modules(SDL2_IMAGE QUIET IMPORTED_TARGET SDL2_image)

    if (TARGET PkgConfig::SDL2_IMAGE)
        set(GRIMAR_SDL2_IMAGE_TARGET PkgConfig::SDL2_IMAGE)
    endif()
endif()

if (NOT GRIMAR_SDL2_IMAGE_TARGET)
    message(FATAL_ERROR
        "SDL2_image was not found. "
        "Install it through vcpkg or your platform package manager."
    )
endif()


find_package(nlohmann_json CONFIG QUIET)

if (TARGET nlohmann_json::nlohmann_json)

    set(GRIMAR_NLOHMANN_JSON_TARGET
        nlohmann_json::nlohmann_json
    )

else()

    find_path(
        GRIMAR_NLOHMANN_JSON_INCLUDE_DIR
        nlohmann/json.hpp
    )

    if (NOT GRIMAR_NLOHMANN_JSON_INCLUDE_DIR)
        message(FATAL_ERROR
            "nlohmann_json was not found. "
            "Install it through vcpkg or your platform package manager."
        )
    endif()

    add_library(grimar_nlohmann_json INTERFACE)

    target_include_directories(
        grimar_nlohmann_json
        INTERFACE
            ${GRIMAR_NLOHMANN_JSON_INCLUDE_DIR}
    )

    set(GRIMAR_NLOHMANN_JSON_TARGET
        grimar_nlohmann_json
    )

endif()


# ============================================================
# Dear ImGui
# ============================================================

add_library(dear_imgui STATIC
    third_party/imgui/imgui.cpp
    third_party/imgui/imgui_draw.cpp
    third_party/imgui/imgui_tables.cpp
    third_party/imgui/imgui_widgets.cpp
    third_party/imgui/backends/imgui_impl_sdl2.cpp
    third_party/imgui/backends/imgui_impl_sdlrenderer2.cpp
)

target_include_directories(
    dear_imgui
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui/backends
)

target_link_libraries(
    dear_imgui
    PUBLIC
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
)


# ============================================================
# Engine Library
# ============================================================

add_library(grimar_engine STATIC

    src/core/dummy.cpp

    include/grimar/core/Assert.hpp
    src/core/Assert.cpp

    include/grimar/core/Log.hpp
    src/core/Log.cpp

    include/grimar/core/Time.hpp
    src/core/Time.cpp

    include/grimar/core/Math2D.hpp

    include/grimar/debug/ImGuiDebugLayer.hpp
    src/debug/ImGuiDebugLayer.cpp

    include/grimar/engine/EngineConfig.hpp
    include/grimar/engine/Scene.hpp
    include/grimar/engine/EngineApp.hpp
    src/engine/EngineApp.cpp

    include/grimar/engine/Entity.hpp
    include/grimar/engine/ComponentStorage.hpp

    include/grimar/engine/World.hpp
    src/engine/World.cpp

    include/grimar/engine/components/Transform2D.hpp
    include/grimar/engine/components/SpriteRenderer.hpp
    include/grimar/engine/components/Animator2D.hpp
    include/grimar/engine/components/CharacterController2D.hpp
    include/grimar/engine/components/RigidBody2D.hpp
    include/grimar/engine/components/BoxCollider2D.hpp

    include/grimar/engine/systems/AnimationSystem.hpp
    src/engine/systems/AnimationSystem.cpp

    include/grimar/engine/systems/CharacterControllerSystem.hpp
    src/engine/systems/CharacterControllerSystem.cpp

    include/grimar/engine/systems/RenderSystem.hpp
    src/engine/systems/RenderSystem.cpp

    include/grimar/engine/systems/PhysicsSystem.hpp
    src/engine/systems/PhysicsSystem.cpp

    include/grimar/engine/systems/DebugDrawSystem.hpp
    src/engine/systems/DebugDrawSystem.cpp

    include/grimar/engine/systems/TileMapSystem.hpp
    src/engine/systems/TileMapSystem.cpp

    include/grimar/platform/Window.hpp
    src/platform/Window.cpp

    include/grimar/platform/FileSystem.hpp
    src/platform/FileSystem.cpp

    include/grimar/platform/Input.hpp
    src/platform/Input.cpp

    include/grimar/render/Color.hpp
    include/grimar/render/Rect.hpp
    include/grimar/render/Renderer2D.hpp
    include/grimar/render/SdlRenderer2D.hpp
    src/render/SdlRenderer2D.cpp
    include/grimar/render/Camera2D.hpp

    include/grimar/assets/Texture2D.hpp
    src/assets/Texture2D.cpp

    include/grimar/assets/AssetManager.hpp
    src/assets/AssetManager.cpp

    include/grimar/assets/SpriteSheet.hpp
    src/assets/SpriteSheet.cpp

    include/grimar/assets/Animation2D.hpp

    include/grimar/assets/TileMap.hpp
    src/assets/TileMap.cpp
)

target_include_directories(
    grimar_engine
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(
    grimar_engine
    PUBLIC
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
        ${GRIMAR_SDL2_IMAGE_TARGET}
)

target_link_libraries(
    grimar_engine
    PRIVATE
        ${GRIMAR_NLOHMANN_JSON_TARGET}
        dear_imgui
)


# ============================================================
# Sandbox
# ============================================================

add_executable(grimar_sandbox
    src/main.cpp
    src/sandbox/SandboxScene.hpp
    src/sandbox/SandboxScene.cpp
)

target_include_directories(
    grimar_sandbox
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(
    grimar_sandbox
    PRIVATE
        grimar_engine
)


# ============================================================
# Editor
# ============================================================

add_executable(grimar_editor
    src/editor/main.cpp
    src/editor/EditorScene.hpp
    src/editor/EditorScene.cpp
)

target_include_directories(
    grimar_editor
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui/backends
)

target_link_libraries(
    grimar_editor
    PRIVATE
        grimar_engine
        dear_imgui
)


# ============================================================
# Tests
# ============================================================

add_executable(grimar_tests
    tests/main.cpp
    tests/smoke/EngineSmokeTests.cpp
    tests/smoke/EngineSmokeTests.hpp
)

target_include_directories(
    grimar_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/tests
)

target_link_libraries(
    grimar_tests
    PRIVATE
        grimar_engine
)


# ============================================================
# Windows Runtime DLLs
# ============================================================

if (WIN32)

    foreach (grimar_target
            IN ITEMS
                grimar_sandbox
                grimar_editor
                grimar_tests)

        add_custom_command(
            TARGET ${grimar_target}
            POST_BUILD

            COMMAND
                ${CMAKE_COMMAND}
                -E
                copy_if_different
                $<TARGET_RUNTIME_DLLS:${grimar_target}>
                $<TARGET_FILE_DIR:${grimar_target}>

            COMMAND_EXPAND_LISTS
        )

    endforeach()

endif()


# ============================================================
# Assets
# ============================================================

foreach (grimar_target
        IN ITEMS
            grimar_sandbox
            grimar_editor
            grimar_tests)

    add_custom_command(
        TARGET ${grimar_target}
        POST_BUILD

        COMMAND
            ${CMAKE_COMMAND}
            -E
            copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/assets
            $<TARGET_FILE_DIR:${grimar_target}>/assets
    )

endforeach()


# ============================================================
# CTest
# ============================================================

if (BUILD_TESTING)
    add_test(
        NAME grimar_smoke
        COMMAND grimar_tests
    )
endif()
