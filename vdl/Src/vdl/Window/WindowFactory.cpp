#include "IWindow.hpp"

#include <vdl/Platform.hpp>

#if defined VDL_TARGET_WINDOWS
#include "Windows/CWindow.hpp"
#elif defined VDL_TARGET_LINUX
#include "Linux/CWindow.hpp"
#endif

IWindow* IWindow::Create()
{
  return new CWindow;
}
