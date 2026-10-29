#include "CudaContext.hpp"

// GLEW must precede the OpenGL headers included by CUDA interop.
#include <GL/glew.h>
#include <cuda_gl_interop.h>

namespace CRRT::CUDA {

[[nodiscard]] Errors FetchCudaContext(CudaContext& context) noexcept
{
    context = CudaContext{};
    CudaContext candidate;
    Errors errors;
    
    if (cudaGLGetDevices(
        &candidate.cudaDeviceCount,
        &candidate.cudaDevice,
        1,
        cudaGLDeviceListAll))
    {
        errors |= ErrorCode::cudaDeviceQueryFailed;
        return errors;
    }
    if (candidate.cudaDeviceCount == 0) {
        errors |= ErrorCode::cudaNoCompatibleDevice;
        return errors;
    }

    context = candidate;
    return errors;
}

}
