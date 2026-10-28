# Use as optional subdirectory for cmake in root

add_executable(debug-colors
    main.cpp
    DebugKernel.cu
)

set_property(TARGET debug-colors PROPERTY EXPORT_COMPILE_COMMANDS ON)

target_link_libraries(debug-colors PRIVATE
    SceneRayTracer
)

target_compile_features(debug-colors PRIVATE
    cxx_std_17
    cuda_std_17
)
set_target_properties(debug-colors PROPERTIES
    CUDA_SEPARABLE_COMPILATION ON
    CUDA_ARCHITECTURES native
)