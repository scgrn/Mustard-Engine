cmake_minimum_required(VERSION 3.12)

project(Mustard-${CMAKE_HOST_SYSTEM_NAME}-${CMAKE_BUILD_TYPE} VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set compile flags
set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -Wall -Wno-pragmas -Wpsabi -msse2 -ldl -lm -lpthread -pthread"
)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
    add_compile_definitions(RELEASE)
endif()

add_compile_definitions(
    LUA_COMPAT_ALL
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG)
    if (NOT MINGW)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
    endif ()
endif()

# Find SDL2 package
set(SDL2_DIR "/libs/SDL2-2.30.3/cmake")
find_package(SDL2 REQUIRED)

set(VENDOR_DIR "../../vendor")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ../../bin)

# Define project sources
set(MAIN_SOURCE
    #../../main/core/memory.cpp

    ../../main/mustard.cpp
    ../../main/pch.cpp

    ../../main/audio/audio.cpp

    ../../main/core/fileSystem.cpp
    ../../main/core/localization.cpp
    ../../main/core/log.cpp
    ../../main/core/window.cpp

    ../../main/input/input.cpp

    ../../main/math/math.cpp
    ../../main/math/matrix.cpp
    ../../main/math/perlin.cpp
    ../../main/math/random.cpp
    ../../main/math/vector.cpp
    ../../main/math/plane.cpp
    ../../main/math/aabb.cpp
    ../../main/math/frustum.cpp

    ../../main/misc/poissonDiscSampling.cpp

    ../../main/renderer/camera.cpp
    ../../main/renderer/font.cpp
    ../../main/renderer/image.cpp
    ../../main/renderer/model.cpp
    ../../main/renderer/palette.cpp
    ../../main/renderer/quadRenderer.cpp
    ../../main/renderer/renderer.cpp
    ../../main/renderer/renderLayer.cpp
    ../../main/renderer/renderTarget.cpp
    ../../main/renderer/shader.cpp
    ../../main/renderer/skybox.cpp
    ../../main/renderer/sprite.cpp
    ../../main/renderer/spriteAtlas.cpp
    ../../main/renderer/texture.cpp
    ../../main/renderer/textureCache.cpp
    ../../main/renderer/tga.cpp

    ../../main/script/audio.cpp
    ../../main/script/collision.cpp
    ../../main/script/font.cpp
    ../../main/script/graphics.cpp
    ../../main/script/input.cpp
    ../../main/script/localization.cpp
    ../../main/script/math.cpp
    ../../main/script/script.cpp
    ../../main/script/system.cpp

    desktop.cpp
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(MAIN_SOURCE
        ${MAIN_SOURCE}
        capture.cpp
        console.cpp
        profiler.cpp
    )
endif()

# Collect third-party sources
aux_source_directory("${VENDOR_DIR}/glad" GLAD_SOURCE)
aux_source_directory("${VENDOR_DIR}/lua-5.3.5/src" LUA_SOURCE)
aux_source_directory("${VENDOR_DIR}/zlib-1.3.1" ZLIB_SOURCE)

# Remove unused source files
list(REMOVE_ITEM LUA_SOURCE "${VENDOR_DIR}/lua-5.3.5/src/lua.c")
list(REMOVE_ITEM LUA_SOURCE "${VENDOR_DIR}/lua-5.3.5/src/luac.c")

# Add compile definition for zlib to include necessary headers
add_compile_definitions(_GNU_SOURCE)

add_library(${PROJECT_NAME} STATIC
    ${MAIN_SOURCE}
    ${GLAD_SOURCE}
    ${LUA_SOURCE}
    ${ZLIB_SOURCE}
)

target_include_directories(${PROJECT_NAME}
    PUBLIC "../../main"
    PUBLIC "."
    PUBLIC "${VENDOR_DIR}"
    PUBLIC "${VENDOR_DIR}/lua-5.3.5/src"
    PUBLIC "${VENDOR_DIR}/zlib-1.3.1"
    PUBLIC "${SDL2_INCLUDE_DIRS}"
)

# Link libraries and options
target_link_options(${PROJECT_NAME} PRIVATE -static-libgcc -static-libstdc++)

if (WIN32)
    add_compile_definitions(
        WIN32
    )
    target_link_libraries(${PROJECT_NAME}
        mingw32
        opengl32
        SDL2::SDL2
        SDL2::SDL2main
    )
else()
    target_link_libraries(${PROJECT_NAME}
        PUBLIC
            ${SDL2_LIBRARIES}
            vorbis
            vorbisfile
            vorbisenc
            ogg
    )
endif()
