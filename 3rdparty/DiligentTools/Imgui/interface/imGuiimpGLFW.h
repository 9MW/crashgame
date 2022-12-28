#pragma once
#include "ImGuiImplDiligent.hpp"
struct GLFWwindow;
namespace Diligent
{
class ImGuiimpGLFW final : public ImGuiImplDiligent
{
public:
    ImGuiimpGLFW(GLFWwindow*    hWnd,
                 IRenderDevice* pDevice,
                 TEXTURE_FORMAT BackBufferFmt,
                 TEXTURE_FORMAT DepthBufferFmt,
                 Uint32         InitialVertexBufferSize = ImGuiImplDiligent::DefaultInitialVBSize,
                 Uint32         InitialIndexBufferSize  = ImGuiImplDiligent::DefaultInitialIBSize);
    ~ImGuiimpGLFW();
    virtual void NewFrame(Uint32 RenderSurfaceWidth, Uint32 RenderSurfaceHeight, SURFACE_TRANSFORM SurfacePreTransform) override final;
    ImGuiimpGLFW(const ImGuiimpGLFW&) = delete;
    ImGuiimpGLFW(ImGuiimpGLFW&&)      = delete;
    ImGuiimpGLFW& operator=(const ImGuiimpGLFW&) = delete;
    ImGuiimpGLFW& operator=(ImGuiimpGLFW&&) = delete;
};

} // namespace Diligent