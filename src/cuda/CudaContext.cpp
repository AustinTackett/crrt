#include "CudaContext.hpp"

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
    if(cudaSetDevice(candidate.cudaDevice)) {
        errors |= ErrorCode::cudaSetDeviceFailed;
        return errors;
    }

    context = candidate;
    return errors;
}

}
