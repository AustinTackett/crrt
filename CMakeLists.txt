cmake_minimum_required(VERSION 3.28)
include(FetchContent)

project(SceneRayTracer LANGUAGES CXX CUDA)

file(GLOB_RECURSE CRRT_INCLUDE CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.cuh"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cuh")
file(GLOB_RECURSE CRRT_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cu")

# Add Source and Header Files into Build
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_library(SceneRayTracer STATIC
    ${CRRT_INCLUDE} 
    ${CRRT_SOURCES})
target_include_directories(SceneRayTracer
    PUBLIC  "${CMAKE_CURRENT_SOURCE_DIR}/include"
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Add Depedencies Already on System into Build
find_package(OpenGL REQUIRED)
find_package(CUDAToolkit REQUIRED)
set_target_properties(SceneRayTracer PROPERTIES
    CUDA_SEPARABLE_COMPILATION ON
    CUDA_RESOLVE_DEVICE_SYMBOLS OFF)

target_link_libraries(SceneRayTracer PUBLIC
    OpenGL::GL
    CUDA::cudart)
target_include_directories(SceneRayTracer PUBLIC
    ${OPENGL_INCLUDE_DIR}
    ${CUDAToolkit_INCLUDE_DIRS})

# CPP and CUDA Properties
set_target_properties(SceneRayTracer PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF

    CUDA_STANDARD 17
    CUDA_STANDARD_REQUIRED ON
    CUDA_ARCHITECTURES native
)

# Define External Dependency File Locations
FetchContent_Declare(glew
    GIT_REPOSITORY https://github.com/Perlmint/glew-cmake.git
    GIT_TAG glew-cmake-2.3.1
    GIT_SHALLOW TRUE)
FetchContent_Declare(glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
    GIT_SHALLOW TRUE)
FetchContent_Declare(glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.3
    GIT_SHALLOW TRUE)

# External Depedency Build Options
set(CMAKE_EXPORT_COMPILE_COMMANDS OFF)
set(ONLY_LIBS ON CACHE BOOL "" FORCE)
set(glew-cmake_BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(glew-cmake_BUILD_STATIC ON CACHE BOOL "" FORCE)
set(GLM_BUILD_LIBRARY OFF CACHE BOOL "" FORCE)

# Download Dependencies
FetchContent_MakeAvailable(glew glfw glm)

target_link_libraries(SceneRayTracer PUBLIC
    libglew_static 
    glfw 
    glm::glm
)
