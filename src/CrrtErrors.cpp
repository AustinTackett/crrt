#include "CrrtErrors.hpp"

namespace CRRT {

Errors& Errors::operator|=(const Errors& other)
{
    errorCodes |= other.errorCodes;
    return *this;
}

Errors& Errors::operator|=(ErrorCode errorCode)
{
    errorCodes.set(static_cast<size_t>(errorCode));
    return *this;
}

Errors Errors::operator|(const Errors& other) const
{
    Errors result = *this;
    result |= other;
    return result;
}

Errors Errors::operator|(ErrorCode errorCode) const
{
    Errors result = *this;
    result |= errorCode;
    return result;
}

Errors::operator bool() const noexcept
{
    return errorCodes.any();
}

void SetError(Errors& errors, ErrorCode errorCode)
{
    errors |= errorCode;
}

Errors JoinErrors(const Errors& errors1, const Errors& errors2)
{
    return errors1 | errors2;
}

void ClearError(Errors& errors, ErrorCode errorCode)
{
    errors.errorCodes[static_cast<int>(errorCode)] = false;
}

bool HasError(const Errors& errors, ErrorCode errorCode)
{
    return errors.errorCodes.test(static_cast<int>(errorCode));
}

bool HasErrors(const Errors& errors)
{
    return errors.errorCodes.count() > 0;
}

std::string GetErrorCodeString(ErrorCode errorCode)
{
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

std::string GetErrorsString(const Errors& errors)
{
    std::string result;
    for (size_t code = 0; code < errors.errorCodes.size(); ++code) {
        if (!errors.errorCodes.test(code)) { continue; }
        result += GetErrorCodeString(static_cast<ErrorCode>(code)) + "\n";
    }
    return result;
}

}
