#include "Window.hpp"

namespace CRRT {

FrameView GetFrameView(const Frame& frame)
{
    return FrameView{frame.cudaTexture.surface, frame.glTexture.width, frame.glTexture.height};
}

}
