cmake_minimum_required(VERSION 3.20)
project(LR1-Remake)

set(CMAKE_CXX_STANDARD 20)

# Dependencies
find_package(SDL3 REQUIRED)
find_package(SDL3_mixer REQUIRED)
find_package(glm REQUIRED)
find_package(Vulkan REQUIRED)
