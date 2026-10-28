cmake_minimum_required(VERSION 3.28)
include(FetchContent)

project(SceneRayTracer LANGUAGES C CXX CUDA)

# Build Options (by default build examples if we call from this cmake)
option(CRRT_BUILD_EXAMPLES
    "Build SceneRayTracer examples"
    ${PROJECT_IS_TOP_LEVEL}
)

add_library(SceneRayTracer STATIC
    src/Window.cpp
    kernels/FrameOperations.cu
)
target_include_directories(SceneRayTracer
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/cuda"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/openGL"
)
set_property(TARGET SceneRayTracer PROPERTY EXPORT_COMPILE_COMMANDS ON)

# CPP and CUDA Properties
set_target_properties(SceneRayTracer PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF

    CUDA_STANDARD 17
    CUDA_STANDARD_REQUIRED ON
    CUDA_ARCHITECTURES native
)
set_target_properties(SceneRayTracer PROPERTIES
    CUDA_SEPARABLE_COMPILATION ON
    CUDA_RESOLVE_DEVICE_SYMBOLS OFF)

# Depedencies on System
find_package(OpenGL REQUIRED)
find_package(CUDAToolkit REQUIRED)

# External Depedencies
set(ONLY_LIBS ON CACHE BOOL "" FORCE)
set(glew-cmake_BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(glew-cmake_BUILD_STATIC ON CACHE BOOL "" FORCE)

set(ONLY_LIBS ON CACHE BOOL "" FORCE)
set(glew-cmake_BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(glew-cmake_BUILD_STATIC ON CACHE BOOL "" FORCE)

FetchContent_Declare(glew
    GIT_REPOSITORY https://github.com/Perlmint/glew-cmake.git
    GIT_TAG glew-cmake-2.3.1
    GIT_SHALLOW TRUE)
FetchContent_Declare(glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
    GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(glew glfw)

target_link_libraries(SceneRayTracer PUBLIC
    OpenGL::GL
    CUDA::cudart
    libglew_static
    glfw
)

# If you want to build examples, add the subdirectory
option(CRRT_BUILD_EXAMPLES "Build SceneRayTracer examples" ${PROJECT_IS_TOP_LEVEL})
if(CRRT_BUILD_EXAMPLES)
    add_subdirectory(examples/debug-colors)
endif()

