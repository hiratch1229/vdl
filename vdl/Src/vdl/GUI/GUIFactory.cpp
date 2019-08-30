#include "IGUI.hpp"

#include <vdl/Platform.hpp>

#if defined VDL_TARGET_WINDOWS
#include "Windows/CGUI.hpp"
#endif

IGUI* IGUI::Create()
{
  return new CGUI;
}
