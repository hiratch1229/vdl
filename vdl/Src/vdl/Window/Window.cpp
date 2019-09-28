#include <vdl/Window.hpp>

#include <vdl/Engine.hpp>
#include <vdl/Window/IWindow.hpp>

namespace vdl::Window
{
  uint2 GetWindowSize()
  {
    return Engine::Get<IWindow>()->GetWindowSize();
  }
}
