cmake_minimum_required(VERSION 3.28)
include(FetchContent)

project(CRRT LANGUAGES C CXX CUDA)

add_library(CRRT STATIC
    src/CrrtErrors.cpp
    src/Input.cpp
    src/Time.cpp
    src/Window.cpp
    src/cuda/CudaContext.cpp
    src/cuda/CudaTexture2D.cpp
    src/openGL/GlShader.cpp
    src/openGL/GlTexture2D.cpp
    src/openGL/GlVao.cpp
    kernels/FrameOperations.cu
)
target_include_directories(CRRT
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/cuda"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/openGL"
)
set_property(TARGET CRRT PROPERTY EXPORT_COMPILE_COMMANDS ON)

# CPP and CUDA Properties
set_target_properties(CRRT PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF

    CUDA_STANDARD 17
    CUDA_STANDARD_REQUIRED ON
    CUDA_ARCHITECTURES native
)
set_target_properties(CRRT PROPERTIES
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

target_link_libraries(CRRT PUBLIC
    OpenGL::GL
    CUDA::cudart
    libglew_static
    glfw
)
