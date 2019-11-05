#include <vdl/Window.hpp>

#include <vdl/Engine.hpp>
#include <vdl/Window/IWindow.hpp>
#include <vdl/SwapChain/ISwapChain.hpp>

namespace vdl::Window
{
  uint2 GetWindowSize()
  {
    return Engine::Get<IWindow>()->GetWindowSize();
  }

  RenderTexture GetRenderTexture()
  {
    return Engine::Get<ISwapChain>()->GetRenderTexture();
  }

  DepthStencilTexture GetDepthStencilTexture()
  {
    return Engine::Get<ISwapChain>()->GetDepthStencilTexture();
  }
}
