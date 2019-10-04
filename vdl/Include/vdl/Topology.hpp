#pragma once
#include "Fwd.hpp"

namespace vdl
{
  enum class TopologyType : uint8_t
  {
    ePointList,
    eLineList,
    eLineStrip,
    eTriangleList,
    eTriangleStrip,
    ePatchList1ControlPoint,
    ePatchList2ControlPoint,
    ePatchList3ControlPoint,
    ePatchList4ControlPoint,
    ePatchList5ControlPoint,
    ePatchList6ControlPoint,
    ePatchList7ControlPoint,
    ePatchList8ControlPoint,
    ePatchList9ControlPoint,
    ePatchList10ControlPoint,
    ePatchList11ControlPoint,
    ePatchList12ControlPoint,
    ePatchList13ControlPoint,
    ePatchList14ControlPoint,
    ePatchList15ControlPoint,
    ePatchList16ControlPoint,
    ePatchList17ControlPoint,
    ePatchList18ControlPoint,
    ePatchList19ControlPoint,
    ePatchList20ControlPoint,
    ePatchList21ControlPoint,
    ePatchList22ControlPoint,
    ePatchList23ControlPoint,
    ePatchList24ControlPoint,
    ePatchList25ControlPoint,
    ePatchList26ControlPoint,
    ePatchList27ControlPoint,
    ePatchList28ControlPoint,
    ePatchList29ControlPoint,
    ePatchList30ControlPoint,
    ePatchList31ControlPoint,
    ePatchList32ControlPoint,

    eDefaultTexture = eTriangleStrip,
    eDefaultStaticMesh = eTriangleList,
    eDefaultSkinnedMesh = eTriangleList,
  };
}
