cmake_minimum_required(VERSION 3.12)

project("${PROJECT_NAME}-${CMAKE_BUILD_TYPE}")

set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -Wall -Wno-pragmas -Wpsabi -msse2 -lm -lpthread -pthread"
)

if(WIN32)
    list(APPEND CMAKE_PREFIX_PATH "../library/vendor/SDL2-2.26.5")
endif()
find_package(SDL2 REQUIRED)

add_executable(${PROJECT_NAME}
    src/main.cpp

    # add other project sources here
)

target_include_directories(${PROJECT_NAME} PRIVATE ${MUSTARD_DIR}/src/main)
target_include_directories(${PROJECT_NAME} PRIVATE ${MUSTARD_DIR}/src/vendor)

target_link_directories(${PROJECT_NAME} PRIVATE ${MUSTARD_DIR}/bin)

if(WIN32)
    include_directories(${PROJECT_NAME} ${SDL2_INCLUDE_DIRS})

    if (TARGET SDL2::SDL2main)
        target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2main)
    endif()
    
    # library must come before SDL
    target_link_libraries(${PROJECT_NAME} PRIVATE libMustard-${CMAKE_BUILD_TYPE}.a SDL2::SDL2)
else()
    # tells GCC to link as an executable rather than shared object
    target_link_options(${PROJECT_NAME} PRIVATE -no-pie)

    include_directories(${PROJECT_NAME} ${SDL2_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} libMustard-${CMAKE_BUILD_TYPE}.a ${SDL2_LIBRARIES} dl)
endif()

