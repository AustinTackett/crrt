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
    Errors& operator|=(const Errors& other);
    Errors& operator|=(ErrorCode errorCode);
    Errors operator|(const Errors& other) const;
    Errors operator|(ErrorCode errorCode) const;
    
    // if (errors) { execute when at least one error is set }
    explicit operator bool() const noexcept;
};

void SetError(Errors& errors, ErrorCode errorCode);

Errors JoinErrors(const Errors& errors1, const Errors& errors2);

void ClearError(Errors& errors, ErrorCode errorCode);

bool HasError(const Errors& errors, ErrorCode errorCode);

bool HasErrors(const Errors& errors);

std::string GetErrorCodeString(ErrorCode errorCode);

std::string GetErrorsString(const Errors& errors);

}

