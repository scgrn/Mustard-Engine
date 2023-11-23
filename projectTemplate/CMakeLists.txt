cmake_minimum_required(VERSION 3.12)

if (DEFINED EMSCRIPTEN)
    project("${PROJECT_NAME}-Web-${CMAKE_BUILD_TYPE}")
else()
    project("${PROJECT_NAME}-${CMAKE_BUILD_TYPE}")
endif()

set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

if (DEFINED EMSCRIPTEN)
    set(CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS} -Wall -Wno-pragmas -Wpsabi -msse2 -msimd128 -sUSE_SDL=2 -sUSE_ZLIB=1"
    )
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
else()
    set(CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS} -Wall -Wno-pragmas -Wpsabi -msse2 -lm -lpthread -pthread"
    )
endif()

if(WIN32)
    list(APPEND CMAKE_PREFIX_PATH "../library/vendor/SDL2-2.26.5")
endif()
find_package(SDL2 REQUIRED)

add_executable(${PROJECT_NAME}
    src/main.cpp

    # add other project sources here
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG)
endif ()

target_include_directories(${PROJECT_NAME} PRIVATE
    ${MUSTARD_DIR}/src/main
    ${MUSTARD_DIR}/src/vendor
    ${SDL2_INCLUDE_DIRS}
)

target_link_directories(${PROJECT_NAME} PRIVATE ${MUSTARD_DIR}/bin)

# https://sam.hooke.me/note/2022/04/porting-a-simple-sdl2-game-to-emscripten/
#if (${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
#   set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "--preload-file=../BORKULA.dat")
#endif()


if(WIN32)
    if (TARGET SDL2::SDL2main)
        target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2main)
    endif()
    
    # library must come before SDL
    target_link_libraries(${PROJECT_NAME} PRIVATE libMustard-${CMAKE_HOST_SYSTEM_NAME}-${CMAKE_BUILD_TYPE}.a SDL2::SDL2)
else()
    # tells GCC to link as an executable rather than shared object
    target_link_options(${PROJECT_NAME} PRIVATE -no-pie)

    if (DEFINED EMSCRIPTEN)
        target_link_libraries(${PROJECT_NAME} libMustard-Web-${CMAKE_BUILD_TYPE}.a)
    else()
        target_link_libraries(${PROJECT_NAME} libMustard-${CMAKE_HOST_SYSTEM_NAME}-${CMAKE_BUILD_TYPE}.a ${SDL2_LIBRARIES} dl)
    endif()
endif()

