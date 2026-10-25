# Project Properties
cmake_minimum_required(VERSION 3.28)
include(FetchContent)

project(SceneRayTracer LANGUAGES C CXX CUDA)

# Define Internal Build File Directories
set(INCLUDE_DIRS
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/cuda"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/openGL")
set(SOURCE_DIRS
    "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Collect All Header Files Recursively
set(ALL_INCLUDE)
foreach(dir IN LISTS INCLUDE_DIRS)
    file(GLOB_RECURSE files CONFIGURE_DEPENDS
        "${dir}/*.h" 
        "${dir}/*.hpp" 
        "${dir}/*.cuh")
    list(FILTER files EXCLUDE REGEX "/[Bb][Aa][Cc][Kk][Uu][Pp][Ss]?/")
    list(APPEND ALL_INCLUDE ${files})
endforeach()
list(REMOVE_DUPLICATES INCLUDE_DIRS)
list(REMOVE_DUPLICATES ALL_INCLUDE)

# Collect Source Files Recursively
set(ALL_SOURCES)
foreach(dir IN LISTS SOURCE_DIRS)
    file(GLOB_RECURSE files CONFIGURE_DEPENDS
        "${dir}/*.cpp" 
        "${dir}/*.cc" 
        "${dir}/*.cu")
    list(FILTER files EXCLUDE REGEX "/[Bb][Aa][Cc][Kk][Uu][Pp][Ss]?/")
    list(APPEND ALL_SOURCES ${files})
endforeach()

# Add Source and Header Files into Build
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_library(SceneRayTracer OBJECT
    ${ALL_INCLUDE} 
    ${ALL_SOURCES})
target_include_directories(SceneRayTracer PUBLIC
    ${INCLUDE_DIRS})

# Add Depedencies Already on System into Build
find_package(OpenGL REQUIRED)
find_package(CUDAToolkit REQUIRED)
set_target_properties(SceneRayTracer PROPERTIES CUDA_SEPARABLE_COMPILATION ON)

target_link_libraries(SceneRayTracer PUBLIC
    OpenGL::GL
    CUDA::cudart
    CUDA::cuda_driver)
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
FetchContent_Declare(assimp
    GIT_REPOSITORY https://github.com/assimp/assimp.git
    GIT_TAG v6.0.5
    GIT_SHALLOW TRUE)
FetchContent_Declare(glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
    GIT_SHALLOW TRUE)
FetchContent_Declare(glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.3
    GIT_SHALLOW TRUE)
FetchContent_Declare(stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG master
    GIT_SHALLOW TRUE)

# External Depedency Build Options
set(CMAKE_EXPORT_COMPILE_COMMANDS OFF)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "" FORCE)
set(ONLY_LIBS ON CACHE BOOL "" FORCE)
set(glew-cmake_BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(glew-cmake_BUILD_STATIC ON CACHE BOOL "" FORCE)

# Download Dependencies
FetchContent_MakeAvailable(glew assimp glfw glm stb)

target_link_libraries(SceneRayTracer PUBLIC
    libglew_static 
    assimp::assimp 
    glfw 
    glm::glm)
target_include_directories(SceneRayTracer PUBLIC "${stb_SOURCE_DIR}")
