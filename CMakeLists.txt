cmake_minimum_required(VERSION 3.20)
project(LR1-Remake)

set(CMAKE_CXX_STANDARD 20)

# Dependencies
find_package(SDL3 REQUIRED)
find_package(SDL3_mixer REQUIRED)
find_package(glm REQUIRED)
find_package(Vulkan REQUIRED COMPONENTS glslc)

#TODO: Implement stb as a dependency (maybe via FetchContent)

include_directories(include)

add_executable(lr1-remake
        src/main.cpp
        src/app/app.cpp
        src/app/loop.cpp
        src/app/render.cpp
        src/app/event.cpp
        src/util/log.cpp
        src/util/res.cpp
        src/util/stbImpl.cpp
        include/LR1-remake/renderer/vulkan.hpp
        src/renderer/vulkan/vulkan.cpp
        src/renderer/vulkan/instance.cpp
        src/renderer/vulkan/debug.cpp
        src/renderer/vulkan/physicalDevice.cpp
        src/renderer/vulkan/logicalDevice.cpp
        src/renderer/vulkan/swapchain.cpp
        src/renderer/vulkan/graphicsPipeline.cpp
        src/renderer/vulkan/drawing.cpp
        src/renderer/vulkan/buffer.cpp
        src/renderer/vulkan/texture.cpp
        src/renderer/vulkan/depthResources.cpp
        include/LR1-remake/renderer/data.hpp
        src/renderer/data/vertexData.cpp
)
target_link_libraries(lr1-remake SDL3::SDL3 SDL3_mixer::SDL3_mixer glm::glm Vulkan::Vulkan)
target_compile_definitions(lr1-remake PUBLIC
        GLM_FORCE_RADIANS
        GLM_FORCE_DEPTH_ZERO_TO_ONE
)
if (CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    message(STATUS "Debug Build")
    target_compile_definitions(lr1-remake PUBLIC LR1_DEBUG)
else ()
    message(STATUS "Release Build")
    set_target_properties(lr1-remake PROPERTIES WIN32_EXECUTABLE ON)
endif ()

# Compile shader files into SPIR-V
file(GLOB_RECURSE shaderFiles RELATIVE ${CMAKE_SOURCE_DIR} "res/shaders/*.vert" "res/shaders/*.frag")
foreach (file ${shaderFiles})
    message(STATUS "Found shader file ${file}, compiling ")
    add_custom_command(
        OUTPUT "${file}.spv"
        COMMAND glslc ${CMAKE_SOURCE_DIR}/${file} -o ${CMAKE_BINARY_DIR}/${file}.spv
        DEPENDS ${file}
    )
    list(APPEND compiledShaderFiles ${CMAKE_BINARY_DIR}/${file}.spv)
endforeach ()

add_custom_target(compiledShaders DEPENDS ${compiledShaderFiles})
add_dependencies(lr1-remake compiledShaders)

#Copy resources
add_custom_command(
        TARGET lr1-remake POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/res/textures/ $<TARGET_FILE_DIR:lr1-remake>/res/textures
)