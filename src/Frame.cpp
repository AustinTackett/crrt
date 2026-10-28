#include "Frame.hpp"

namespace CRRT {

[[nodiscard]] Errors CreateFrame(Frame& frame, int width, int height)
{
    Errors errors;
    if (IsFrameAlive(frame)) 
    {
        errors |= ErrorCode::frameAlreadyCreated;
    }
    if (width <= 0 || height <= 0) {
        errors |= ErrorCode::glTextureInvalidDimensions;
    }
    if (errors) { 
        return errors; 
    }
    
    GL::GlTexture2DConfig textureConfig;
    textureConfig.width = width;
    textureConfig.height = height;

    errors |= GL::CreateVao(frame.glVao);
    if (errors) { return errors; }
    errors |= GL::LoadShader(frame.glShader);
    if (errors) { return errors; }
    errors |= GL::CreateEmptyTexture(frame.glTexture, textureConfig);
    if (errors) { return errors; }

    errors |= CUDA::FetchCudaContext(frame.cudaContext);
    if (errors) { return errors; }

    errors |= CUDA::RegisterTextureInterop(
        frame.cudaContext,
        frame.cudaTexture,
        frame.glTexture);

    return errors;
}

[[nodiscard]] Errors ReleaseFrame(Frame& frame)
{
    Errors errors;

    // Destroy surface and unmap resource.
    if (frame.cudaTexture.isMemMapped || frame.cudaTexture.surface)
    {
        errors |= CUDA::UnMemMapTexture(
            frame.cudaContext,
            frame.cudaTexture
        );
        if (errors) { return errors; } 
    }

    // Unregister CUDA from the OpenGL texture.
    if (frame.cudaTexture.textureResource)
    {
        errors |= CUDA::UnregisterTextureInterop(
            frame.cudaContext,
            frame.cudaTexture
        );

        if (errors) { return errors; }
    }

    // Release OpenGL Resources
    GL::ReleaseTexture(frame.glTexture);
    GL::ReleaseShader(frame.glShader);
    GL::ReleaseVao(frame.glVao);

    frame.cudaContext = {};

    return errors;
}

void Draw(Frame& frame)
{
    if (frame.cudaTexture.isMemMapped || frame.cudaTexture.surface) { return; }

    GL::BindShader(frame.glShader);
    GL::BindTexture(frame.glTexture, 0);
    GL::SetUniform1i(frame.glShader, "frameTexture", 0);
    GL::DrawVao(frame.glVao);
}

[[nodiscard]] Errors BeginCudaAccess(Frame& frame)
{
    return CUDA::MemMapTexture(frame.cudaContext, frame.cudaTexture);
}

[[nodiscard]] Errors EndCudaAccess(Frame& frame)
{
    return CUDA::UnMemMapTexture(frame.cudaContext, frame.cudaTexture);
}

[[nodiscard]] bool IsFrameAlive(const Frame& frame)
{
    // Check if frame has any lingering resources
    return (
        frame.cudaTexture.textureResource ||
        frame.cudaTexture.isMemMapped ||
        frame.cudaTexture.surface ||
        frame.glTexture.texture ||
        frame.glShader.shader ||
        frame.glVao.vao
    );
}

FrameView GetFrameView(const Frame& frame)
{
    return FrameView{frame.cudaTexture.surface, frame.glTexture.width, frame.glTexture.height};
}

}
