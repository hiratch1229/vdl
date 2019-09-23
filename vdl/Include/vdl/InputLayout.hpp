#pragma once
#include "vdl/Types.hpp"

namespace vdl
{
  enum class InputLayout : uint8_t
  {
    eNone,
    eTexture,
    eStaticMesh,
    eSkinnedMesh,

    //  TODO:“à•”—p‚È‚Ì‚Å‰B‚·
    eGUI
  };
}
