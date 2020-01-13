#pragma once
#include "Fwd.hpp"
#include "Type2.hpp"
#include "Angle.hpp"
#include "Color.hpp"
#include "GraphicsStates.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "ConstantBuffer.hpp"
#include "UnorderedAccessBuffer.hpp"
#include "Variant.hpp"

namespace vdl::Renderer2D
{
  void SetTopology(TopologyType _Topology);

  void SetScissor(const Scissor& _Scissor);

  void SetViewport(const Viewport& _Viewport);

  void SetBlendState(const BlendState& _BlendState);

  void SetDepthStencilState(const DepthStencilState& _DepthStencilState);

  void SetRasterizerState(const RasterizerState& _RasterizerState);

  inline void SetGraphicsStates(const BlendState& _BlendState, const DepthStencilState& _DepthStencilState, const RasterizerState& _RasterizerState)
  {
    SetBlendState(_BlendState);
    SetDepthStencilState(_DepthStencilState);
    SetRasterizerState(_RasterizerState);
  }

  inline void SetGraphicsStates(const GraphicsStates& _GraphicsStates)
  {
    SetBlendState(_GraphicsStates.BlendState);
    SetDepthStencilState(_GraphicsStates.DepthStencilState);
    SetRasterizerState(_GraphicsStates.RasterizerState);
  }

  void SetVertexShader(const VertexShader& _VertexShader);

  void SetHullShader(const HullShader& _HullShader);

  void SetDomainShader(const DomainShader& _DomainShader);

  void SetGeometryShader(const GeometryShader& _GeometryShader);

  void SetPixelShader(const PixelShader& _PixelShader);

  inline void SetShaders(const VertexShader& _VertexShader, const PixelShader& _PixelShader)
  {
    SetVertexShader(_VertexShader);
    SetHullShader(HullShader());
    SetDomainShader(DomainShader());
    SetGeometryShader(GeometryShader());
    SetPixelShader(_PixelShader);
  }

  inline void SetShaders(const VertexShader& _VertexShader, const GeometryShader& _GeometryShader, const PixelShader& _PixelShader)
  {
    SetVertexShader(_VertexShader);
    SetHullShader(HullShader());
    SetDomainShader(DomainShader());
    SetGeometryShader(_GeometryShader);
    SetPixelShader(_PixelShader);
  }

  inline void SetShaders(const VertexShader& _VertexShader, const HullShader& _HullShader, const DomainShader& _DomainShader, const PixelShader& _PixelShader)
  {
    SetVertexShader(_VertexShader);
    SetHullShader(_HullShader);
    SetDomainShader(_DomainShader);
    SetGeometryShader(GeometryShader());
    SetPixelShader(_PixelShader);
  }

  inline void SetShaders(const VertexShader& _VertexShader, const HullShader& _HullShader, const DomainShader& _DomainShader, const GeometryShader& _GeometryShader, const PixelShader& _PixelShader)
  {
    SetVertexShader(_VertexShader);
    SetHullShader(_HullShader);
    SetDomainShader(_DomainShader);
    SetGeometryShader(_GeometryShader);
    SetPixelShader(_PixelShader);
  }

  inline void SetGraphicsShaders(const GraphicsShaders& _GraphicsShaders)
  {
    SetVertexShader(_GraphicsShaders.VertexShader);
    SetHullShader(_GraphicsShaders.HullShader);
    SetDomainShader(_GraphicsShaders.DomainShader);
    SetGeometryShader(_GraphicsShaders.GeometryShader);
    SetPixelShader(_GraphicsShaders.PixelShader);
  }

  void SetVertexStageShaderResources(uint _StartSlot, uint _ShaderResourceNum, const ShaderResource _ShaderResources[]);

  inline void SetVertexStageShaderResources(uint _StartSlot, uint _TextureNum, const Texture _Textures[])
  {
    std::vector<ShaderResource> ShaderResources(_TextureNum);
    {
      for (uint TextureCount = 0; TextureCount < _TextureNum; ++TextureCount)
      {
        ShaderResources[TextureCount] = _Textures[TextureCount];
      }
    }

    SetVertexStageShaderResources(_StartSlot, _TextureNum, ShaderResources.data());
  }

  inline void SetVertexStageShaderResources(uint _StartSlot, uint _UnorderedAccessBufferDataNum, const Detail::UnorderedAccessBufferData _UnorderedAccessBufferDatas[])
  {
    std::vector<ShaderResource> ShaderResources(_UnorderedAccessBufferDataNum);
    {
      for (uint UnorderedAccessBufferDataCount = 0; UnorderedAccessBufferDataCount < _UnorderedAccessBufferDataNum; ++UnorderedAccessBufferDataCount)
      {
        ShaderResources[UnorderedAccessBufferDataCount] = _UnorderedAccessBufferDatas[UnorderedAccessBufferDataCount];
      }
    }

    SetVertexStageShaderResources(0, _UnorderedAccessBufferDataNum, ShaderResources.data());
  }

  template<class T>
  inline void SetVertexStageShaderResources(uint _StartSlot, uint _UnorderedAccessBufferNum, const UnorderedAccessBuffer<T> _UnorderedAccessBuffers[])
  {
    std::vector<ShaderResource> ShaderResources(_UnorderedAccessBufferNum);
    {
      for (uint UnorderedAccessBufferCount = 0; UnorderedAccessBufferCount < _UnorderedAccessBufferNum; ++UnorderedAccessBufferCount)
      {
        ShaderResources[UnorderedAccessBufferCount] = _UnorderedAccessBuffers[UnorderedAccessBufferCount].GetDetail();
      }
    }

    SetVertexStageShaderResources(_StartSlot, _UnorderedAccessBufferNum, ShaderResources.data());
  }

  void SetHullStageShaderResources(uint _StartSlot, uint _ShaderResourceNum, const ShaderResource _ShaderResources[]);

  inline void SetHullStageShaderResources(uint _StartSlot, uint _TextureNum, const Texture _Textures[])
  {
    std::vector<ShaderResource> ShaderResources(_TextureNum);
    {
      for (uint TextureCount = 0; TextureCount < _TextureNum; ++TextureCount)
      {
        ShaderResources[TextureCount] = _Textures[TextureCount];
      }
    }

    SetHullStageShaderResources(_StartSlot, _TextureNum, ShaderResources.data());
  }

  inline void SetHullStageShaderResources(uint _StartSlot, uint _UnorderedAccessBufferDataNum, const Detail::UnorderedAccessBufferData _UnorderedAccessBufferDatas[])
  {
    std::vector<ShaderResource> ShaderResources(_UnorderedAccessBufferDataNum);
    {
      for (uint UnorderedAccessBufferDataCount = 0; UnorderedAccessBufferDataCount < _UnorderedAccessBufferDataNum; ++UnorderedAccessBufferDataCount)
      {
        ShaderResources[UnorderedAccessBufferDataCount] = _UnorderedAccessBufferDatas[UnorderedAccessBufferDataCount];
      }
    }

    SetHullStageShaderResources(0, _UnorderedAccessBufferDataNum, ShaderResources.data());
  }

  template<class T>
  inline void SetHullStageShaderResources(uint _StartSlot, uint _UnorderedAccessBufferNum, const UnorderedAccessBuffer<T> _UnorderedAccessBuffers[])
  {
    std::vector<ShaderResource> ShaderResources(_UnorderedAccessBufferNum);
    {
      for (uint UnorderedAccessBufferCount = 0; UnorderedAccessBufferCount < _UnorderedAccessBufferNum; ++UnorderedAccessBufferCount)
      {
        ShaderResources[UnorderedAccessBufferCount] = _UnorderedAccessBuffers[UnorderedAccessBufferCount].GetDetail();
      }
    }

    SetHullStageShaderResources(_StartSlot, _UnorderedAccessBufferNum, ShaderResources.data());
  }

  void SetDomainStageShaderResources(uint _StartSlot, uint _ShaderResourceNum, const ShaderResource _ShaderResources[]);

  inline void SetDomainStageShaderResources(uint _StartSlot, uint _TextureNum, const Texture _Textures[])
  {
    std::vector<ShaderResource> ShaderResources(_TextureNum);
    {
      for (uint TextureCount = 0; TextureCount < _TextureNum; ++TextureCount)
      {
        ShaderResources[TextureCount] = _Textures[TextureCount];
      }
    }

    SetDomainStageShaderResources(_StartSlot, _TextureNum, ShaderResources.data());
  }

  inline void SetDomainStageShaderResources(uint _StartSlot, uint _UnorderedAccessBufferDataNum, const Detail::UnorderedAccessBufferData _UnorderedAccessBufferDatas[])
  {
    std::vector<ShaderResource> ShaderResources(_UnorderedAccessBufferDataNum);
    {
      for (uint UnorderedAccessBufferDataCount = 0; UnorderedAccessBufferDataCount < _UnorderedAccessBufferDataNum; ++UnorderedAccessBufferDataCount)
      {
        ShaderResources[UnorderedAccessBufferDataCount] = _UnorderedAccessBufferDatas[UnorderedAccessBufferDataCount];
      }
    }

    SetDomainStageShaderResources(0, _UnorderedAccessBufferDataNum, ShaderResources.data());
  }

  template<class T>
  inline void SetDomainStageShaderResources(uint _StartSlot, uint _UnorderedAccessBufferNum, const UnorderedAccessBuffer<T> _UnorderedAccessBuffers[])
  {
    std::vector<ShaderResource> ShaderResources(_UnorderedAccessBufferNum);
    {
      for (uint UnorderedAccessBufferCount = 0; UnorderedAccessBufferCount < _UnorderedAccessBufferNum; ++UnorderedAccessBufferCount)
      {
        ShaderResources[UnorderedAccessBufferCount] = _UnorderedAccessBuffers[UnorderedAccessBufferCount].GetDetail();
      }
    }

    SetDomainStageShaderResources(_StartSlot, _UnorderedAccessBufferNum, ShaderResources.data());
  }

  void SetGeometryStageShaderResources(uint _StartSlot, uint _ShaderResourceNum, const ShaderResource _ShaderResources[]);

  inline void SetGeometryStageShaderResources(uint _StartSlot, uint _TextureNum, const Texture _Textures[])
  {
    std::vector<ShaderResource> ShaderResources(_TextureNum);
    {
      for (uint TextureCount = 0; TextureCount < _TextureNum; ++TextureCount)
      {
        ShaderResources[TextureCount] = _Textures[TextureCount];
      }
    }

    SetGeometryStageShaderResources(_StartSlot, _TextureNum, ShaderResources.data());
  }

  inline void SetGeometryStageShaderResources(uint _StartSlot, uint _UnorderedAccessBufferDataNum, const Detail::UnorderedAccessBufferData _UnorderedAccessBufferDatas[])
  {
    std::vector<ShaderResource> ShaderResources(_UnorderedAccessBufferDataNum);
    {
      for (uint UnorderedAccessBufferDataCount = 0; UnorderedAccessBufferDataCount < _UnorderedAccessBufferDataNum; ++UnorderedAccessBufferDataCount)
      {
        ShaderResources[UnorderedAccessBufferDataCount] = _UnorderedAccessBufferDatas[UnorderedAccessBufferDataCount];
      }
    }

    SetGeometryStageShaderResources(0, _UnorderedAccessBufferDataNum, ShaderResources.data());
  }

  template<class T>
  inline void SetGeometryStageShaderResources(uint _StartSlot, uint _UnorderedAccessBufferNum, const UnorderedAccessBuffer<T> _UnorderedAccessBuffers[])
  {
    std::vector<ShaderResource> ShaderResources(_UnorderedAccessBufferNum);
    {
      for (uint UnorderedAccessBufferCount = 0; UnorderedAccessBufferCount < _UnorderedAccessBufferNum; ++UnorderedAccessBufferCount)
      {
        ShaderResources[UnorderedAccessBufferCount] = _UnorderedAccessBuffers[UnorderedAccessBufferCount].GetDetail();
      }
    }

    SetGeometryStageShaderResources(_StartSlot, _UnorderedAccessBufferNum, ShaderResources.data());
  }

  void SetPixelStageShaderResources(uint _StartSlot, uint _ShaderResourceNum, const ShaderResource _ShaderResources[]);

  inline void SetPixelStageShaderResources(uint _StartSlot, uint _TextureNum, const Texture _Textures[])
  {
    std::vector<ShaderResource> ShaderResources(_TextureNum);
    {
      for (uint TextureCount = 0; TextureCount < _TextureNum; ++TextureCount)
      {
        ShaderResources[TextureCount] = _Textures[TextureCount];
      }
    }

    SetPixelStageShaderResources(_StartSlot, _TextureNum, ShaderResources.data());
  }

  inline void SetPixelStageShaderResources(uint _StartSlot, uint _UnorderedAccessBufferDataNum, const Detail::UnorderedAccessBufferData _UnorderedAccessBufferDatas[])
  {
    std::vector<ShaderResource> ShaderResources(_UnorderedAccessBufferDataNum);
    {
      for (uint UnorderedAccessBufferDataCount = 0; UnorderedAccessBufferDataCount < _UnorderedAccessBufferDataNum; ++UnorderedAccessBufferDataCount)
      {
        ShaderResources[UnorderedAccessBufferDataCount] = _UnorderedAccessBufferDatas[UnorderedAccessBufferDataCount];
      }
    }

    SetPixelStageShaderResources(0, _UnorderedAccessBufferDataNum, ShaderResources.data());
  }

  template<class T>
  inline void SetPixelStageShaderResources(uint _StartSlot, uint _UnorderedAccessBufferNum, const UnorderedAccessBuffer<T> _UnorderedAccessBuffers[])
  {
    std::vector<ShaderResource> ShaderResources(_UnorderedAccessBufferNum);
    {
      for (uint UnorderedAccessBufferCount = 0; UnorderedAccessBufferCount < _UnorderedAccessBufferNum; ++UnorderedAccessBufferCount)
      {
        ShaderResources[UnorderedAccessBufferCount] = _UnorderedAccessBuffers[UnorderedAccessBufferCount].GetDetail();
      }
    }

    SetPixelStageShaderResources(_StartSlot, _UnorderedAccessBufferNum, ShaderResources.data());
  }



  void SetVertexStageSamplers(uint _StartSlot, uint _SamplerNum, const Sampler _Sampler[]);

  void SetHullStageSamplers(uint _StartSlot, uint _SamplerNum, const Sampler _Sampler[]);

  void SetDomainStageSamplers(uint _StartSlot, uint _SamplerNum, const Sampler _Sampler[]);

  void SetGeometryStageSamplers(uint _StartSlot, uint _SamplerNum, const Sampler _Sampler[]);

  void SetPixelStageSamplers(uint _StartSlot, uint _SamplerNum, const Sampler _Sampler[]);

  void SetVertexStageConstantBuffers(uint _StartSlot, uint _BufferNum, const Detail::ConstantBufferData _ConstantBuffers[]);

  template<class T>
  inline void SetVertexStageConstantBuffers(uint _Slot, uint _BufferNum, const ConstantBuffer<T> _ConstantBuffers[])
  {
    std::vector<Detail::ConstantBufferData> ConstantBuffers(_BufferNum);
    {
      for (uint ConstantBufferCount = 0; ConstantBufferCount < _BufferNum; ++ConstantBufferCount)
      {
        ConstantBuffers[ConstantBufferCount] = _ConstantBuffers[ConstantBufferCount].GetDetail();
      }
    }

    SetVertexStageConstantBuffers(_Slot, _BufferNum, ConstantBuffers.data());
  }

  void SetHullStageConstantBuffers(uint _StartSlot, uint _BufferNum, const Detail::ConstantBufferData _ConstantBuffers[]);

  template<class T>
  inline void SetHullStageConstantBuffers(uint _Slot, uint _BufferNum, const ConstantBuffer<T> _ConstantBuffers[])
  {
    std::vector<Detail::ConstantBufferData> ConstantBuffers(_BufferNum);
    {
      for (uint ConstantBufferCount = 0; ConstantBufferCount < _BufferNum; ++ConstantBufferCount)
      {
        ConstantBuffers[ConstantBufferCount] = _ConstantBuffers[ConstantBufferCount].GetDetail();
      }
    }

    SetHullStageConstantBuffers(_Slot, _BufferNum, ConstantBuffers.data());
  }

  void SetDomainStageConstantBuffers(uint _StartSlot, uint _BufferNum, const Detail::ConstantBufferData _ConstantBuffers[]);

  template<class T>
  inline void SetDomainStageConstantBuffers(uint _Slot, uint _BufferNum, const ConstantBuffer<T> _ConstantBuffers[])
  {
    std::vector<Detail::ConstantBufferData> ConstantBuffers(_BufferNum);
    {
      for (uint ConstantBufferCount = 0; ConstantBufferCount < _BufferNum; ++ConstantBufferCount)
      {
        ConstantBuffers[ConstantBufferCount] = _ConstantBuffers[ConstantBufferCount].GetDetail();
      }
    }

    SetDomainStageConstantBuffers(_Slot, _BufferNum, ConstantBuffers.data());
  }

  void SetGeometryStageConstantBuffers(uint _StartSlot, uint _BufferNum, const Detail::ConstantBufferData _ConstantBuffers[]);

  template<class T>
  inline void SetGeometryStageConstantBuffers(uint _Slot, uint _BufferNum, const ConstantBuffer<T> _ConstantBuffers[])
  {
    std::vector<Detail::ConstantBufferData> ConstantBuffers(_BufferNum);
    {
      for (uint ConstantBufferCount = 0; ConstantBufferCount < _BufferNum; ++ConstantBufferCount)
      {
        ConstantBuffers[ConstantBufferCount] = _ConstantBuffers[ConstantBufferCount].GetDetail();
      }
    }

    SetGeometryStageConstantBuffers(_Slot, _BufferNum, ConstantBuffers.data());
  }

  void SetPixelStageConstantBuffers(uint _StartSlot, uint _BufferNum, const Detail::ConstantBufferData _ConstantBuffers[]);

  template<class T>
  inline void SetPixelStageConstantBuffers(uint _Slot, uint _BufferNum, const ConstantBuffer<T> _ConstantBuffers[])
  {
    std::vector<Detail::ConstantBufferData> ConstantBuffers(_BufferNum);
    {
      for (uint ConstantBufferCount = 0; ConstantBufferCount < _BufferNum; ++ConstantBufferCount)
      {
        ConstantBuffers[ConstantBufferCount] = _ConstantBuffers[ConstantBufferCount].GetDetail();
      }
    }

    SetPixelStageConstantBuffers(_Slot, _BufferNum, ConstantBuffers.data());
  }

  void Draw(const Texture& _Texture, const float2& _DestLeftTop, const float2& _DestSize, const float2& _SrcLeftPos, const float2& _SrcSize, const Degree& _Angle, const Color4F& _Color = Palette::White);

  inline void Draw(const Texture& _Texture, const float2& _DestLeftTop, const float2& _DestSize, const float2& _SrcLeftPos, const float2& _SrcSize, const Color4F& _Color = Palette::White)
  {
    Draw(_Texture, _DestLeftTop, _SrcSize, _SrcLeftPos, _SrcSize, 0.0f, _Color);
  }

  inline void Draw(const Texture& _Texture, const float2& _DestLeftTop, const float2& _SrcLeftPos, const float2& _SrcSize, const Degree& _Angle, const Color4F& _Color = Palette::White)
  {
    Draw(_Texture, _DestLeftTop, _SrcSize, _SrcLeftPos, _SrcSize, _Angle, _Color);
  }

  inline void Draw(const Texture& _Texture, const float2& _DestLeftTop, const float2& _SrcLeftPos, const float2& _SrcSize, const Color4F& _Color = Palette::White)
  {
    Draw(_Texture, _DestLeftTop, _SrcSize, _SrcLeftPos, _SrcSize, 0.0f, _Color);
  }

  inline void Draw(const Texture& _Texture, const float2& _DestLeftTop, const float2& _DestSize, const Degree& _Angle, const Color4F& _Color = Palette::White)
  {
    Draw(_Texture, _DestLeftTop, _DestSize, float2(0.0f), _Texture.GetSize(), _Angle, _Color);
  }

  inline void Draw(const Texture& _Texture, const float2& _DestLeftTop, const float2& _DestSize, const Color4F& _Color = Palette::White)
  {
    Draw(_Texture, _DestLeftTop, _DestSize, float2(0.0f), _Texture.GetSize(), 0.0f, _Color);
  }

  inline void Draw(const Texture& _Texture, const float2& _DestLeftTop, const Degree& _Angle, const Color4F& _Color = Palette::White)
  {
    const uint2 Size = _Texture.GetSize();
    Draw(_Texture, _DestLeftTop, Size, float2(0.0f), Size, _Angle, _Color);
  }

  inline void Draw(const Texture& _Texture, const float2& _DestLeftTop, const Color4F& _Color = Palette::White)
  {
    const uint2 Size = _Texture.GetSize();
    Draw(_Texture, _DestLeftTop, Size, float2(0.0f), Size, 0.0f, _Color);
  }

  inline void Draw(const Texture& _Texture, const Color4F& _Color = Palette::White)
  {
    const uint2 Size = _Texture.GetSize();
    Draw(_Texture, float2(0.0f), Size, float2(0.0f), Size, 0.0f, _Color);
  }
}
