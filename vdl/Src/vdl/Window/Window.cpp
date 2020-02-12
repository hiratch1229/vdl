#include <vdl/Window.hpp>

#include <vdl/Engine.hpp>
#include <vdl/Window/IWindow.hpp>
#include <vdl/DeviceContext/IDeviceContext.hpp>

namespace vdl::Window
{
  uint2 GetWindowSize()
  {
    return Engine::Get<IWindow>()->GetWindowSize();
  }

  RenderTexture GetRenderTexture()
  {
    return Engine::Get<IDeviceContext>()->GetRenderTexture();
  }

  DepthStencilTexture GetDepthStencilTexture()
  {
    return Engine::Get<IDeviceContext>()->GetDepthStencilTexture();
  }
}
