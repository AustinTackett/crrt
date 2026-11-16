#include "CudaContext.hpp"

#include <vector>

#include <GL/glew.h>
#include <cuda_gl_interop.h>

namespace CRRT::CUDA {

[[nodiscard]] Errors FetchCudaContext(CudaContext& context) noexcept
{
    context = CudaContext{};
    CudaContext candidate;
    Errors errors;

    int totalDeviceCount = 0;
    if (cudaGetDeviceCount(&totalDeviceCount) != cudaSuccess) {
        errors |= ErrorCode::cudaDeviceQueryFailed;
        return errors;
    }
    if (totalDeviceCount == 0) {
        errors |= ErrorCode::cudaNoCompatibleDevice;
        return errors;
    }

    // Get all cuda devices
    std::vector<int> deviceIds;
    try {
        deviceIds.resize(totalDeviceCount);
    } catch (const std::bad_alloc&) {
        errors |= ErrorCode::cudaHostAllocationFailed;
        return errors;
    }

    unsigned int glDeviceCount = 0;
    cudaError_t cudaDeviceResult = cudaGLGetDevices(
        &glDeviceCount, 
        deviceIds.data(), 
        static_cast<unsigned int>(deviceIds.size()),
        cudaGLDeviceListAll);
    if (cudaDeviceResult != cudaSuccess)
    {
        errors |= ErrorCode::cudaDeviceQueryFailed;
        return errors;
    }

    // Investigate properties for each device and choose one
    int deviceCount = std::min(static_cast<int>(glDeviceCount), totalDeviceCount);
    for (int i = 0; i < deviceCount; ++i) {  
        const int deviceId = deviceIds[i];

        cudaDeviceProp props{};
        if (cudaGetDeviceProperties(&props, deviceId) != cudaSuccess) {
            errors |= ErrorCode::cudaDeviceQueryFailed;
            return errors;
        }

        // Here you could add your device checks and just continue the loop
        // if it doesn't pass. I'll just get the first cuda capable device

        if (cudaSetDevice(deviceId) != cudaSuccess) {
            continue;
        }

        candidate.cudaDevice = deviceId;
        candidate.cudaDeviceCount = glDeviceCount;
        context = candidate;
        return errors;
    }


    errors |= ErrorCode::cudaNoCompatibleDevice;
    return errors;
}

}
