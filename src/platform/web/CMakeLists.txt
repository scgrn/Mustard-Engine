cmake_minimum_required(VERSION 3.12)

project(Mustard-Web-${CMAKE_BUILD_TYPE} VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set compile flags
set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -Wall -Wno-pragmas -Wpsabi -msse2 -msimd128 -sUSE_SDL=2 -sUSE_ZLIB=1"
)

add_compile_definitions(
    # for emscripten
    # https://uncovergame.com/2015/01/21/porting-a-complete-c-game-engine-to-html5-through-emscripten/
    LUA_COMPAT_ALL
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG)
endif ()

set(VENDOR_DIR "../../vendor")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ../../bin)

# Define your project sources
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

    web.cpp
)

# Collect third-party sources
aux_source_directory("${VENDOR_DIR}/lua-5.3.5/src" LUA_SOURCE)

# Remove unused source files
list(REMOVE_ITEM LUA_SOURCE "${VENDOR_DIR}/lua-5.3.5/src/lua.c")
list(REMOVE_ITEM LUA_SOURCE "${VENDOR_DIR}/lua-5.3.5/src/luac.c")

add_library(${PROJECT_NAME} STATIC
    ${MAIN_SOURCE}
    ${LUA_SOURCE}
)

target_include_directories(${PROJECT_NAME}
    PUBLIC "../../main"
    PUBLIC "${VENDOR_DIR}"
    PUBLIC "${VENDOR_DIR}/lua-5.3.5/src"
)

# Link libraries and options
target_link_options(${PROJECT_NAME} PRIVATE
    -static-libgcc
    -static-libstdc++
    -s USE_SDL=2
    -s USE_WEBGL2=1
    -s FULL_ES3=1
    -s MIN_WEBGL_VERSION=2
    -s MAX_WEBGL_VERSION=2
    -s ALLOW_MEMORY_GROWTH=1
    -s OFFSCREEN_FRAMEBUFFER=1
    -s NO_DISABLE_EXCEPTION_CATCHING=1
    "-sEXPORTED_RUNTIME_METHODS=['HEAPF32','ccall','cwrap']"
    -sSTACK_SIZE=1024KB
    -sINITIAL_RUN=0
    --no-entry
)

