#pragma once
#include <vdl/Topology.hpp>

#include <vulkan/vulkan.hpp>

#include <assert.h>

inline vk::PrimitiveTopology Cast(vdl::TopologyType _Topology)
{
  switch (_Topology)
  {
  case vdl::TopologyType::ePointList:
    return vk::PrimitiveTopology::ePointList;
  case vdl::TopologyType::eLineList:
    return vk::PrimitiveTopology::eLineList;
  case vdl::TopologyType::eLineStrip:
    return vk::PrimitiveTopology::eLineStrip;
  case vdl::TopologyType::eTriangleList:
    return vk::PrimitiveTopology::eTriangleList;
  case vdl::TopologyType::eTriangleStrip:
    return vk::PrimitiveTopology::eTriangleStrip;
  case vdl::TopologyType::ePatchList1ControlPoint:
  case vdl::TopologyType::ePatchList2ControlPoint:
  case vdl::TopologyType::ePatchList3ControlPoint:
  case vdl::TopologyType::ePatchList4ControlPoint:
  case vdl::TopologyType::ePatchList5ControlPoint:
  case vdl::TopologyType::ePatchList6ControlPoint:
  case vdl::TopologyType::ePatchList7ControlPoint:
  case vdl::TopologyType::ePatchList8ControlPoint:
  case vdl::TopologyType::ePatchList9ControlPoint:
  case vdl::TopologyType::ePatchList10ControlPoint:
  case vdl::TopologyType::ePatchList11ControlPoint:
  case vdl::TopologyType::ePatchList12ControlPoint:
  case vdl::TopologyType::ePatchList13ControlPoint:
  case vdl::TopologyType::ePatchList14ControlPoint:
  case vdl::TopologyType::ePatchList15ControlPoint:
  case vdl::TopologyType::ePatchList16ControlPoint:
  case vdl::TopologyType::ePatchList17ControlPoint:
  case vdl::TopologyType::ePatchList18ControlPoint:
  case vdl::TopologyType::ePatchList19ControlPoint:
  case vdl::TopologyType::ePatchList20ControlPoint:
  case vdl::TopologyType::ePatchList21ControlPoint:
  case vdl::TopologyType::ePatchList22ControlPoint:
  case vdl::TopologyType::ePatchList23ControlPoint:
  case vdl::TopologyType::ePatchList24ControlPoint:
  case vdl::TopologyType::ePatchList25ControlPoint:
  case vdl::TopologyType::ePatchList26ControlPoint:
  case vdl::TopologyType::ePatchList27ControlPoint:
  case vdl::TopologyType::ePatchList28ControlPoint:
  case vdl::TopologyType::ePatchList29ControlPoint:
  case vdl::TopologyType::ePatchList30ControlPoint:
  case vdl::TopologyType::ePatchList31ControlPoint:
  case vdl::TopologyType::ePatchList32ControlPoint:
    return vk::PrimitiveTopology::ePatchList;
  default: assert(false);
  }


}

inline vk::PipelineTessellationStateCreateInfo GetTessellationStateInfo(vdl::TopologyType _Topology)
{
  assert(Cast(_Topology) == vk::PrimitiveTopology::ePatchList);

  return vk::PipelineTessellationStateCreateInfo(vk::PipelineTessellationStateCreateFlags(),
    static_cast<vdl::uint32_t>(_Topology) - static_cast<vdl::uint32_t>(vdl::TopologyType::ePatchList1ControlPoint) + 1);
}
