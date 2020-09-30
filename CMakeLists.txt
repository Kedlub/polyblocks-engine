# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("PolyblocksEngine")

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
# Include sub-projects.
add_subdirectory ("PolyblocksEngine")
add_subdirectory("libs/glfw/")
add_subdirectory("libs/glad/")
add_subdirectory("libs/glm/glm/")
add_definitions(-DGLM_FORCE_RADIANS) # use only modern API in radians
# include_directories(${PROJECT_SOURCE_DIR}/libs/glm)
include_directories("libs/glfw-3.3.2/include")
# add_library(glad libs/glad/include/glad/glad.h libs/glad/src/glad.c)

find_package(OpenGL REQUIRED)