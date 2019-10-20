#include "IGUI.hpp"

#include "CGUI.hpp"

IGUI* IGUI::Create()
{
  return new CGUI;
}
