cmake_minimum_required(VERSION 3.20)
project(LR1-Remake)

set(CMAKE_CXX_STANDARD 20)

# Dependencies
find_package(SDL3 REQUIRED)
find_package(SDL3_mixer REQUIRED)
find_package(glm REQUIRED)
find_package(Vulkan REQUIRED)

include_directories(include)

add_executable(lr1-remake
        src/main.cpp
        src/app/app.cpp
        src/app/loop.cpp
        src/app/render.cpp
        src/app/event.cpp
        src/util/log.cpp
        src/renderer/vulkan.cpp
)
target_link_libraries(lr1-remake SDL3::SDL3 SDL3_mixer::SDL3_mixer glm::glm Vulkan::Vulkan)
target_compile_definitions(lr1-remake PUBLIC
        GLM_FORCE_RADIANS
        GLM_FORCE_DEPTH_ZERO_TO_ONE
)
if (CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    message(STATUS "Debug Build")
    target_compile_definitions(lr1-remake PUBLIC LR1_DEBUG)
endif ()