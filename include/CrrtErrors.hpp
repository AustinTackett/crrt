#pragma once

#include <bitset>
#include <string>

namespace CRRT {

constexpr int maxNumErrorCodes = 64;
enum class ErrorCode
{
    glSuccess = 0,
    glTextureAlreadyCreated,
    glTextureInvalidDimensions,
    glTextureUnsupportedFormat,
    glTextureCreationFailed,
    glTextureConfigurationFailed,
    glShaderVertexFileReadFailed,
    glShaderFragmentFileReadFailed,
    glShaderCreationFailed,
    glShaderVertexCompilationFailed,
    glShaderFragmentCompilationFailed,
    glShaderProgramCreationFailed,
    glShaderProgramLinkFailed,
    glShaderAlreadyCreated,
    glVaoCreationFailed,
    glVaoAlreadyCreated,
    cudaDeviceQueryFailed,
    cudaNoCompatibleDevice,
    cudaSetDeviceFailed,
    cudaTextureAlreadyRegistered,
    cudaTextureNotRegistered,
    cudaInvalidGlTexture,
    cudaTextureAlreadyMapped,
    cudaTextureRegistrationFailed,
    cudaTextureUnregistrationFailed,
    cudaTextureMapFailed,
    cudaMappedArrayQueryFailed,
    cudaSurfaceCreationFailed,
    cudaDeviceSynchronizationFailed,
    cudaSurfaceDestructionFailed,
    cudaTextureUnmapFailed,
    windowsSuccess,
    windowsAlreadyCreated,
    windowsInvalidWindow,
    windowsGlfwInitializationFailed,
    windowsCreationFailed,
    windowsContextActivationFailed,
    windowsGlewInitializationFailed,
    windowsFrameOperationFailed,
    windowsFrameCleanupFailed,
    windowDrawCallbackException,
    userUncaughtException,
    frameAlreadyCreated,
    frameConfigReadFailed,
    frameConfigInvalid,

    // Integer storing how many errors enums have been added
    ErrorCodeCount
};
static_assert(static_cast<size_t>(ErrorCode::ErrorCodeCount) <= maxNumErrorCodes);

struct Errors {
    std::bitset<maxNumErrorCodes> errorCodes;

    // Operator overloading to use | to set flags
    Errors& operator|=(const Errors& other)
    {
        errorCodes |= other.errorCodes;
        return *this;
    }
    Errors& operator|=(ErrorCode errorCode)
    {
        errorCodes.set(static_cast<size_t>(errorCode));
        return *this;
    }
    Errors operator|(const Errors& other) const
    {
        Errors result = *this;
        result |= other;
        return result;
    }
    Errors operator|(ErrorCode errorCode) const
    {
        Errors result = *this;
        result |= errorCode;
        return result;
    }
    
    // if (errors) { execute when at least one error is set }
    explicit operator bool() const noexcept
    {
        return errorCodes.any();
    }
};


void inline SetError(Errors& errors, ErrorCode errorCode) {
    errors |= errorCode;
}

Errors inline JoinErrors(const Errors& errors1, const Errors& errors2) {
    return errors1 | errors2;
}

void inline ClearError(Errors& errors, ErrorCode errorCode) {
    errors.errorCodes[static_cast<int>(errorCode)] = false;
}

bool inline HasError(const Errors& errors, ErrorCode errorCode) {
    return errors.errorCodes.test(static_cast<int>(errorCode));
}

bool inline HasErrors(const Errors& errors) {
    return errors.errorCodes.count() > 0;
}

std::string inline GetErrorCodeString(ErrorCode errorCode) {
    switch (errorCode) {
        case ErrorCode::glSuccess:                                return "glSuccess";
        case ErrorCode::glTextureAlreadyCreated:                  return "glTextureAlreadyCreated";
        case ErrorCode::glTextureInvalidDimensions:               return "glTextureInvalidDimensions";
        case ErrorCode::glTextureUnsupportedFormat:               return "glTextureUnsupportedFormat";
        case ErrorCode::glTextureCreationFailed:                  return "glTextureCreationFailed";
        case ErrorCode::glTextureConfigurationFailed:             return "glTextureConfigurationFailed";
        case ErrorCode::glShaderVertexFileReadFailed:             return "glShaderVertexFileReadFailed";
        case ErrorCode::glShaderFragmentFileReadFailed:           return "glShaderFragmentFileReadFailed";
        case ErrorCode::glShaderCreationFailed:                   return "glShaderCreationFailed";
        case ErrorCode::glShaderVertexCompilationFailed:          return "glShaderVertexCompilationFailed";
        case ErrorCode::glShaderFragmentCompilationFailed:        return "glShaderFragmentCompilationFailed";
        case ErrorCode::glShaderProgramCreationFailed:            return "glShaderProgramCreationFailed";
        case ErrorCode::glShaderProgramLinkFailed:                return "glShaderProgramLinkFailed";
        case ErrorCode::glShaderAlreadyCreated:                   return "glShaderAlreadyCreated";
        case ErrorCode::glVaoCreationFailed:                      return "glVaoCreationFailed";
        case ErrorCode::glVaoAlreadyCreated:                      return "glVaoAlreadyCreated";
        case ErrorCode::cudaDeviceQueryFailed:                    return "cudaDeviceQueryFailed";
        case ErrorCode::cudaNoCompatibleDevice:                   return "cudaNoCompatibleDevice";
        case ErrorCode::cudaSetDeviceFailed:                      return "cudaSetDeviceFailed";
        case ErrorCode::cudaTextureAlreadyRegistered:             return "cudaTextureAlreadyRegistered";
        case ErrorCode::cudaTextureNotRegistered:                 return "cudaTextureNotRegistered";
        case ErrorCode::cudaInvalidGlTexture:                     return "cudaInvalidGlTexture";
        case ErrorCode::cudaTextureAlreadyMapped:                 return "cudaTextureAlreadyMapped";
        case ErrorCode::cudaTextureRegistrationFailed:            return "cudaTextureRegistrationFailed";
        case ErrorCode::cudaTextureUnregistrationFailed:          return "cudaTextureUnregistrationFailed";
        case ErrorCode::cudaTextureMapFailed:                     return "cudaTextureMapFailed";
        case ErrorCode::cudaMappedArrayQueryFailed:               return "cudaMappedArrayQueryFailed";
        case ErrorCode::cudaSurfaceCreationFailed:                return "cudaSurfaceCreationFailed";
        case ErrorCode::cudaDeviceSynchronizationFailed:          return "cudaDeviceSynchronizationFailed";
        case ErrorCode::cudaSurfaceDestructionFailed:             return "cudaSurfaceDestructionFailed";
        case ErrorCode::cudaTextureUnmapFailed:                   return "cudaTextureUnmapFailed";
        case ErrorCode::windowsSuccess:                           return "windowsSuccess";
        case ErrorCode::windowsAlreadyCreated:                    return "windowsAlreadyCreated";
        case ErrorCode::windowsInvalidWindow:                     return "windowsInvalidWindow";
        case ErrorCode::windowsGlfwInitializationFailed:          return "windowsGlfwInitializationFailed";
        case ErrorCode::windowsCreationFailed:                    return "windowsCreationFailed";
        case ErrorCode::windowsContextActivationFailed:           return "windowsContextActivationFailed";
        case ErrorCode::windowsGlewInitializationFailed:          return "windowsGlewInitializationFailed";
        case ErrorCode::windowsFrameOperationFailed:              return "windowsFrameOperationFailed";
        case ErrorCode::windowsFrameCleanupFailed:                return "windowsFrameCleanupFailed";
        case ErrorCode::windowDrawCallbackException:              return "windowDrawCallbackException";
        case ErrorCode::userUncaughtException:                    return "userUncaughtException";
        case ErrorCode::frameAlreadyCreated:                      return "frameAlreadyCreated";
        case ErrorCode::frameConfigReadFailed:                    return "frameConfigReadFailed";
        case ErrorCode::frameConfigInvalid:                       return "frameConfigInvalid";
        default:                                                  return "Unknown error code";
    }
}

std::string inline GetErrorsString(const Errors& errors) {
    std::string result;
    for (size_t code = 0; code < errors.errorCodes.size(); ++code) {
        if (!errors.errorCodes.test(code)) { continue; }
        result += GetErrorCodeString(static_cast<ErrorCode>(code)) + "\n";
    }
    return result;
}

}

