cmake_minimum_required(VERSION 3.21)

project(Grimar LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---- Engine Library ----
add_library(grimar_engine STATIC
        src/core/dummy.cpp
        include/grimar/core/Assert.hpp
        src/core/Assert.cpp
        include/grimar/core/Log.hpp
        src/core/Log.cpp
        include/grimar/core/Time.hpp
        src/core/Time.cpp
        include/grimar/engine/EngineApp.hpp
        src/engine/EngineApp.cpp
        include/grimar/platform/Window.hpp
        src/platform/Window.cpp
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
	    src/assets/Animation2D.cpp
		include/grimar/assets/TileMap.hpp
		src/assets/TileMap.cpp
		include/grimar/engine/Entity.hpp

		include/grimar/engine/World.hpp
		src/engine/World.cpp
		include/grimar/engine/components/Transform2D.hpp
		include/grimar/core/Math2D.hpp
		include/grimar/engine/components/SpriteRenderer.hpp
		include/grimar/engine/systems/RenderSystem.hpp
		src/engine/systems/RenderSystem.cpp
		include/grimar/engine/systems/PhysicsSystem.hpp
		src/engine/systems/PhysicsSystem.cpp
		include/grimar/engine/systems/DebugDrawSystem.hpp
		src/engine/systems/DebugDrawSystem.cpp
		include/grimar/engine/systems/TileMapSystem.hpp
		src/engine/systems/TileMapSystem.cpp
		include/grimar/engine/components/RigidBody2D.hpp
		include/grimar/engine/components/BoxCollider2D.hpp


)


add_executable(grimar_tests
		tests/smoke/EngineSmokeTests.cpp
		tests/smoke/EngineSmokeTests.hpp
		tests/main.cpp
)

target_include_directories(grimar_tests PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/tests
)

target_link_libraries(grimar_tests PRIVATE grimar_engine)

target_include_directories(grimar_engine PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_sources(grimar_engine PRIVATE
        src/core/dummy.cpp
        src/core/Assert.cpp
)


find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)
find_package(SDL2_ttf CONFIG REQUIRED)
find_package(SDL2_mixer CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

target_link_libraries(grimar_engine PUBLIC
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
        SDL2::SDL2main

        $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
        $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>
        $<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,SDL2_mixer::SDL2_mixer,SDL2_mixer::SDL2_mixer-static>
)

target_link_libraries(grimar_engine PRIVATE
        nlohmann_json::nlohmann_json
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

