#pragma once
#include "BlendState.hpp"
#include "DepthStencilState.hpp"
#include "RasterizerState.hpp"
#include "Sampler.hpp"

#include <type_traits>
#include <string>

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
  struct std::hash<vdl::BlendState>
  {
    size_t operator()(const vdl::BlendState& _KeyVal)const
    {
      std::string Str = std::to_string(_KeyVal.AlphaToCoverageEnable) + std::to_string(_KeyVal.IndependentBlendEnable);

      for (auto& RenderTexture : _KeyVal.RenderTexture)
      {
        Str += RenderTexture.Data;
      }

      return std::hash<std::string>()(Str);
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

  template<>
  struct std::hash<vdl::Sampler>
  {
    size_t operator()(const vdl::Sampler& _KeyVal)const
    {
      return std::hash<vdl::Sampler::DataType>()(_KeyVal.Data);
    }
  };
}
