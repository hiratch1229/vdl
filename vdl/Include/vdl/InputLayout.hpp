#pragma once
#include "Fwd.hpp"

namespace vdl
{
  enum class InputLayoutType : uint8_t
  {
    eNone,
    eTexture,
    eMesh,

    //  TODO:内部用なので隠す
    eGUI
  };
}
