### DEPRECATED. WE MOVED TO AMBUILD. SEE MAKEFILE FOR DETAILS
### THIS FILE IS IMPORTANT TO MAKE CLION RESOLVE HEADER FILES
### IDUNNO HOW TO MAKE IT BETTER :(
cmake_minimum_required(VERSION 3.24)
project(resourcemod)

set (CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/lib)

add_definitions(-DCOMPILER_MSVC64 -DCOMPILER_MSVC -Dprotobuf_BUILD_TESTS=OFF)

include_directories(third_party third_party/google/v8 alliedmodders/hl2sdk/game/shared alliedmodders/hl2sdk/public alliedmodders/metamod-source/core alliedmodders/hl2sdk/game/server alliedmodders/hl2sdk/public/entity2)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")

add_library(resourcemod SHARED
        game/GameDLL.cpp
        game/hooks/EventManager.cpp
        engine/Engine.cpp
        engine/EventLoop.cpp
        engine/Utils.cpp
        engine/Plugin.cpp
        engine/ExternalRuntime.cpp
        game/ResourceMod.cpp)

target_link_libraries(resourcemod
        ${CMAKE_SOURCE_DIR}/third_party/sourcehook/sourcehook.lib
        )
target_compile_definitions(resourcemod PUBLIC IS_INTERNAL_RUNTIME)
### THIS PART IS ACTUALLY IN-USE
### IT RELATED TO EXTERNAL RUNTIME
### IT WILL COMPILE YOUR EXTERNAL RUNTIME DLL FILE
project(external_runtime)

set (CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/lib)
add_definitions(-DCOMPILER_MSVC64 -DCOMPILER_MSVC -Dprotobuf_BUILD_TESTS=OFF)
include_directories(third_party)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")
add_library(external_runtime SHARED external_runtime/ExternalRuntime.cpp)

target_link_libraries(external_runtime
        ${CMAKE_SOURCE_DIR}/lib/libzmq.lib
        )