cmake_minimum_required(VERSION 3.28)
project(DebugColors LANGUAGES C CXX CUDA)

add_subdirectory(
    "${CMAKE_CURRENT_SOURCE_DIR}/../.."
    "${CMAKE_CURRENT_BINARY_DIR}/SceneRayTracer"
)

add_executable(debug-colors main.cpp DebugKernel.cu)

target_link_libraries(debug-colors PRIVATE SceneRayTracer)
target_compile_features(debug-colors PRIVATE cxx_std_17 cuda_std_17)

set_target_properties(debug-colors PROPERTIES
    CUDA_SEPARABLE_COMPILATION ON
    CUDA_ARCHITECTURES native
)
