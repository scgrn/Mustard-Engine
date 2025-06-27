cmake_minimum_required(VERSION 3.12)

set(BASE_PROJECT_NAME ${PROJECT_NAME})

if (DEFINED EMSCRIPTEN)
    project("${PROJECT_NAME}-Web-${CMAKE_BUILD_TYPE}")
else()
    if (CMAKE_BUILD_TYPE STREQUAL "Dist")
        project("${PROJECT_NAME}")
    else()
        project("${PROJECT_NAME}-${CMAKE_BUILD_TYPE}")
    endif()
endif()

set(CMAKE_VERBOSE_MAKEFILE OFF)

if (CMAKE_BUILD_TYPE STREQUAL "Dist")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/dist)
else()
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
endif()

if (DEFINED EMSCRIPTEN)
    set(CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS} -Wall -Wno-pragmas -Wpsabi -msse2 -msimd128 -sUSE_SDL=2 -sUSE_ZLIB=1 -sNO_DISABLE_EXCEPTION_CATCHING"
    )
    # set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "index")
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
else()
    set(CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS} -Wall -Wno-pragmas -Wpsabi -msse2 -lm -lpthread -pthread"
    )
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG)
    if (NOT MINGW)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
    endif ()
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "Dist")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
    add_compile_definitions(RELEASE)
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Dist")
    add_compile_definitions(DIST)
endif()

find_package(SDL2 REQUIRED)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS src/*.cpp)
list(REMOVE_ITEM SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/addArchive.cpp")
add_executable(${PROJECT_NAME} ${SRC_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${MUSTARD_DIR}/src/main
    ${MUSTARD_DIR}/src/vendor
    ${SDL2_INCLUDE_DIRS}
)

target_link_directories(${PROJECT_NAME} PRIVATE ${MUSTARD_DIR}/bin)

# https://sam.hooke.me/note/2022/04/porting-a-simple-sdl2-game-to-emscripten/
if (${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "--preload-file=../../dist/${BASE_PROJECT_NAME}.dat@./${BASE_PROJECT_NAME}.dat")
endif()

if (WIN32)
    if (TARGET SDL2::SDL2main)
        target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2main)
    endif()
    
    if (CMAKE_BUILD_TYPE STREQUAL "Dist")    
        target_link_libraries(${PROJECT_NAME} PRIVATE libMustard-${CMAKE_HOST_SYSTEM_NAME}-Release.a SDL2::SDL2)
    else()
        # library must come before SDL
        target_link_libraries(${PROJECT_NAME} PRIVATE libMustard-${CMAKE_HOST_SYSTEM_NAME}-${CMAKE_BUILD_TYPE}.a SDL2::SDL2)
    endif()
else()
    # tells GCC to link as an executable rather than shared object
    target_link_options(${PROJECT_NAME} PRIVATE -no-pie)

    if (DEFINED EMSCRIPTEN)
        target_link_libraries(${PROJECT_NAME} libMustard-Web-${CMAKE_BUILD_TYPE}.a)
        #target_link_options(${PROJECT_NAME} PRIVATE -sFULL_ES3 -sALLOW_MEMORY_GROWTH)
        target_link_options(${PROJECT_NAME} PRIVATE
            -static-libgcc
            -static-libstdc++
            -sUSE_SDL=2
            -sUSE_WEBGL2=1
            -sFULL_ES3=1
            -sMIN_WEBGL_VERSION=2
            -sMAX_WEBGL_VERSION=2
            -sALLOW_MEMORY_GROWTH=1
            -sOFFSCREEN_FRAMEBUFFER=1
            -sNO_DISABLE_EXCEPTION_CATCHING=1
            "-sEXPORTED_RUNTIME_METHODS=['HEAPF32','ccall','cwrap']"
            "-sSTACK_SIZE=1024KB"
        )

    else()
        if (CMAKE_BUILD_TYPE STREQUAL "Dist")    
            target_link_libraries(${PROJECT_NAME} libMustard-${CMAKE_HOST_SYSTEM_NAME}-Release.a ${SDL2_LIBRARIES} dl)
        else()
            target_link_libraries(${PROJECT_NAME} libMustard-${CMAKE_HOST_SYSTEM_NAME}-${CMAKE_BUILD_TYPE}.a ${SDL2_LIBRARIES} dl)
        endif()
    endif()
endif()
