#pragma once
#include "Fwd.hpp"

namespace vdl
{
  enum class InputLayoutType : uint8_t
  {
    eNone,
    eTexture,
    eStaticMesh,
    eSkinnedMesh,

    //  TODO:“à•”—p‚È‚Ì‚Å‰B‚·
    eGUI
  };
}
