#pragma once
#include "GraphicsState.hpp"

#include <type_traits>

namespace std
{
  template<>
  struct std::hash<vdl::RenderTextureBlendState>
  {
    size_t operator()(const vdl::RenderTextureBlendState& _KeyVal)const
    {
      return std::hash<vdl::RenderTextureBlendState::DataType>()(_KeyVal.Data);
    }
  };

  template<>
  struct std::hash<vdl::DepthStencilOpState>
  {
    size_t operator()(const vdl::DepthStencilOpState& _KeyVal)const
    {
      return std::hash<vdl::DepthStencilOpState::DataType>()(_KeyVal.Data);
    }
  };

  template<>
  struct std::hash<vdl::DepthStencilState>
  {
    size_t operator()(const vdl::DepthStencilState& _KeyVal)const
    {
      return std::hash<vdl::DepthStencilState::DataType>()(_KeyVal.Data);
    }
  };

  template<>
  struct std::hash<vdl::RasterizerState>
  {
    size_t operator()(const vdl::RasterizerState& _KeyVal)const
    {
      return std::hash<vdl::RasterizerState::DataType>()(_KeyVal.Data);
    }
  };
}