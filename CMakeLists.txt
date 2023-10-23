cmake_minimum_required(VERSION 3.12)

project(Mustard VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set compile flags
set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -Wall -Wno-pragmas -Wpsabi -msse2 -ldl -lm -lpthread -pthread"
)

# Set the output directory for static libraries
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

add_compile_definitions(
    DEBUG
    WITH_MINIAUDIO

    # for emscripten
    # https://uncovergame.com/2015/01/21/porting-a-complete-c-game-engine-to-html5-through-emscripten/
    LUA_COMPAT_ALL
)

# Find SDL2 package
set(SDL2_DIR "/libs/SDL2-2.26.5/cmake")
find_package(SDL2 REQUIRED)

set(VENDOR_DIR "${PROJECT_SOURCE_DIR}/src/vendor")
set(PLATFORM_DIR "${PROJECT_SOURCE_DIR}/src/platform/desktop")

# Define your project sources
set(MAIN_SOURCE
    ${PROJECT_SOURCE_DIR}/src/main/core/memory.cpp

    ${PROJECT_SOURCE_DIR}/src/main/mustard.cpp
    ${PROJECT_SOURCE_DIR}/src/main/pch.cpp
    
    ${PROJECT_SOURCE_DIR}/src/main/audio/audio.cpp

    ${PROJECT_SOURCE_DIR}/src/main/core/fileSystem.cpp
    ${PROJECT_SOURCE_DIR}/src/main/core/localization.cpp
    ${PROJECT_SOURCE_DIR}/src/main/core/log.cpp
    ${PROJECT_SOURCE_DIR}/src/main/core/window.cpp

    ${PROJECT_SOURCE_DIR}/src/main/input/input.cpp

    ${PROJECT_SOURCE_DIR}/src/main/math/matrix.cpp
    ${PROJECT_SOURCE_DIR}/src/main/math/perlin.cpp
    ${PROJECT_SOURCE_DIR}/src/main/math/random.cpp
    ${PROJECT_SOURCE_DIR}/src/main/math/vector.cpp

    ${PROJECT_SOURCE_DIR}/src/main/misc/capture.cpp
    ${PROJECT_SOURCE_DIR}/src/main/misc/console.cpp
    ${PROJECT_SOURCE_DIR}/src/main/misc/profiler.cpp

    ${PROJECT_SOURCE_DIR}/src/main/renderer/camera.cpp
    ${PROJECT_SOURCE_DIR}/src/main/renderer/font.cpp
    ${PROJECT_SOURCE_DIR}/src/main/renderer/image.cpp
    ${PROJECT_SOURCE_DIR}/src/main/renderer/model.cpp
    ${PROJECT_SOURCE_DIR}/src/main/renderer/palette.cpp
    ${PROJECT_SOURCE_DIR}/src/main/renderer/renderer.cpp
    ${PROJECT_SOURCE_DIR}/src/main/renderer/renderLayer.cpp
    ${PROJECT_SOURCE_DIR}/src/main/renderer/renderTarget.cpp
    ${PROJECT_SOURCE_DIR}/src/main/renderer/shader.cpp
    ${PROJECT_SOURCE_DIR}/src/main/renderer/skybox.cpp
    ${PROJECT_SOURCE_DIR}/src/main/renderer/sprite.cpp
    ${PROJECT_SOURCE_DIR}/src/main/renderer/spriteAtlas.cpp
    ${PROJECT_SOURCE_DIR}/src/main/renderer/texture.cpp
    ${PROJECT_SOURCE_DIR}/src/main/renderer/textureCache.cpp
    ${PROJECT_SOURCE_DIR}/src/main/renderer/tga.cpp

    ${PROJECT_SOURCE_DIR}/src/main/script/audio.cpp
    ${PROJECT_SOURCE_DIR}/src/main/script/collision.cpp
    ${PROJECT_SOURCE_DIR}/src/main/script/font.cpp
    ${PROJECT_SOURCE_DIR}/src/main/script/graphics.cpp
    ${PROJECT_SOURCE_DIR}/src/main/script/input.cpp
    ${PROJECT_SOURCE_DIR}/src/main/script/localization.cpp
    ${PROJECT_SOURCE_DIR}/src/main/script/math.cpp
    ${PROJECT_SOURCE_DIR}/src/main/script/script.cpp
    ${PROJECT_SOURCE_DIR}/src/main/script/system.cpp
)

set(PLATFORM_SOURCE
    ${PLATFORM_DIR}/desktop.cpp
)

# Collect third-party sources
aux_source_directory("${VENDOR_DIR}/glad" LUA_SOURCE)
aux_source_directory("${VENDOR_DIR}/lua-5.3.5/src" LUA_SOURCE)
aux_source_directory("${VENDOR_DIR}/tinyxml" TINYXML_SOURCE)
aux_source_directory("${VENDOR_DIR}/zlib-1.2.11" ZLIB_SOURCE)
aux_source_directory("${VENDOR_DIR}/soloud20200207/src/core" SOLOUD_CORE_SOURCE)
aux_source_directory("${VENDOR_DIR}/soloud20200207/src/audiosource/wav" SOLOUD_AUDIOSOURCE_SOURCE)
aux_source_directory("${VENDOR_DIR}/soloud20200207/src/filter" SOLOUD_FILTER_SOURCE)
aux_source_directory("${VENDOR_DIR}/soloud20200207/src/backend/miniaudio" SOLOUD_BACKEND_SOURCE)

# Remove unused source files
list(REMOVE_ITEM LUA_SOURCE "${VENDOR_DIR}/lua-5.3.5/src/lua.c")
list(REMOVE_ITEM LUA_SOURCE "${VENDOR_DIR}/lua-5.3.5/src/luac.c")
list(REMOVE_ITEM TINYXML_SOURCE "${VENDOR_DIR}/tinyxml/xmltest.cpp")

# Link directories (if needed)
# link_directories(${SDL2_LIB_DIR})

add_library(${PROJECT_NAME} STATIC
    ${MAIN_SOURCE}
    ${PLATFORM_SOURCE}
    ${LUA_SOURCE}
    ${TINYXML_SOURCE}
    ${ZLIB_SOURCE}
    ${SOLOUD_CORE_SOURCE}
    ${SOLOUD_AUDIOSOURCE_SOURCE}
    ${SOLOUD_FILTER_SOURCE}
    ${SOLOUD_BACKEND_SOURCE}
)

target_include_directories(${PROJECT_NAME}
    PUBLIC "${PROJECT_SOURCE_DIR}/src/main"
    PUBLIC "${VENDOR_DIR}"
    PUBLIC "${VENDOR_DIR}/lua-5.3.5/src"
    PUBLIC "${VENDOR_DIR}/soloud20200207/include"
    PUBLIC "${VENDOR_DIR}/tinyxml"
    PUBLIC "${VENDOR_DIR}/zlib-1.2.11"
    PUBLIC "${PLATFORM_DIR}/win64"
    PUBLIC "${PLATFORM_DIR}/win64/glad"
    PUBLIC ${SDL2_INCLUDE_DIRS}
)

# Link libraries and options
target_link_options(${PROJECT_NAME} PRIVATE -static-libgcc -static-libstdc++)

if(WIN32)
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
    target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
endif()

