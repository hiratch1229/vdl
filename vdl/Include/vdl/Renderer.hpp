#pragma once
#include "Fwd.hpp"
#include "GraphicsState.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "Color.hpp"
#include "ConstantBuffer.hpp"

namespace vdl::Renderer
{
  void SetRenderTextures(const RenderTextures& _RenderTextures, const DepthStencilTexture& _DepthStencilTexture);

  inline void SetRenderTexture(const RenderTexture& _RenderTexture, const DepthStencilTexture& _DepthStencilTexture)
  {
    SetRenderTextures({ _RenderTexture }, _DepthStencilTexture);
  }

  void Clear(const RenderTexture& _RenderTexure, const ColorF& _ClearColor = Palette::Black);

  void Clear(const DepthStencilTexture& _DepthStencilTexture, float _ClearDepth = 1.0f, uint _ClearStencil = 0);

  void Clear(const UnorderedAccessTexture& _UnorderedAccessTexture, const ColorF& _ClearColor = Palette::Black);

  void SetTopology(TopologyType _Topology);

  void SetScissor(const Scissor& _Scissor);

  void SetViewport(const Viewport& _Viewport);

  void SetBlendState(const BlendState& _BlendState);

  void SetDepthStencilState(const DepthStencilState& _DepthStencilState);

  void SetRasterizerState(const RasterizerState& _RasterizerState);

  inline void SetGraphicsState(const GraphicsState& _GraphicsState)
  {
    SetBlendState(_GraphicsState.BlendState);
    SetDepthStencilState(_GraphicsState.DepthSteniclState);
    SetRasterizerState(_GraphicsState.RasterizerState);
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

  void SetVertexStageTextures(uint _StartSlot, uint _TextureNum, const Texture _Textures[]);

  void SetHullStageTextures(uint _StartSlot, uint _TextureNum, const Texture _Textures[]);

  void SetDomainStageTextures(uint _StartSlot, uint _TextureNum, const Texture _Textures[]);

  void SetGeometryStageTextures(uint _StartSlot, uint _TextureNum, const Texture _Textures[]);

  void SetPixelStageTextures(uint _StartSlot, uint _TextureNum, const Texture _Textures[]);

  void SetVertexStageSamplers(uint _StartSlot, uint _SamplerNum, const Sampler _Sampler[]);

  void SetHullStageSamplers(uint _StartSlot, uint _SamplerNum, const Sampler _Sampler[]);

  void SetDomainStageSamplers(uint _StartSlot, uint _SamplerNum, const Sampler _Sampler[]);

  void SetGeometryStageSamplers(uint _StartSlot, uint _SamplerNum, const Sampler _Sampler[]);

  void SetPixelStageSamplers(uint _StartSlot, uint _SamplerNum, const Sampler _Sampler[]);

  void SetVertexStageConstantBuffers(uint _StartSlot, uint _BufferNum, const Detail::ConstantBufferData _ConstantBuffers[]);

  void SetHullStageConstantBuffers(uint _StartSlot, uint _BufferNum, const Detail::ConstantBufferData _ConstantBuffers[]);

  void SetDomainStageConstantBuffers(uint _StartSlot, uint _BufferNum, const Detail::ConstantBufferData _ConstantBuffers[]);

  void SetGeometryStageConstantBuffers(uint _StartSlot, uint _BufferNum, const Detail::ConstantBufferData _ConstantBuffers[]);

  void SetPixelStageConstantBuffers(uint _StartSlot, uint _BufferNum, const Detail::ConstantBufferData _ConstantBuffers[]);

  template<class T>
  inline void SetVertexStageConstantBuffers(uint _StartSlot, uint _BufferNum, const ConstantBuffer<T> _ConstantBuffers[])
  {
    std::vector<Detail::ConstantBufferData> ConstantBuffers(_BufferNum);
    {
      for (uint ConstantBufferCount = 0; ConstantBufferCount < _BufferNum; ++ConstantBufferCount)
      {
        ConstantBuffers[ConstantBufferCount] = _ConstantBuffers[ConstantBufferCount].GetDetail();
      }
    }

    SetVertexStageConstantBuffers(_StartSlot, _BufferNum, ConstantBuffers.data());
  }

  template<class T>
  inline void SetHullStageConstantBuffers(uint _StartSlot, uint _BufferNum, const ConstantBuffer<T> _ConstantBuffers[])
  {
    std::vector<Detail::ConstantBufferData> ConstantBuffers(_BufferNum);
    {
      for (uint ConstantBufferCount = 0; ConstantBufferCount < _BufferNum; ++ConstantBufferCount)
      {
        ConstantBuffers[ConstantBufferCount] = _ConstantBuffers[ConstantBufferCount].GetDetail();
      }
    }

    SetHullStageConstantBuffers(_StartSlot, _BufferNum, ConstantBuffers.data());
  }

  template<class T>
  inline void SetDomainStageConstantBuffers(uint _StartSlot, uint _BufferNum, const ConstantBuffer<T> _ConstantBuffers[])
  {
    std::vector<Detail::ConstantBufferData> ConstantBuffers(_BufferNum);
    {
      for (uint ConstantBufferCount = 0; ConstantBufferCount < _BufferNum; ++ConstantBufferCount)
      {
        ConstantBuffers[ConstantBufferCount] = _ConstantBuffers[ConstantBufferCount].GetDetail();
      }
    }

    SetDomainStageConstantBuffers(_StartSlot, _BufferNum, ConstantBuffers.data());
  }

  template<class T>
  inline void SetGeometryStageConstantBuffers(uint _StartSlot, uint _BufferNum, const ConstantBuffer<T> _ConstantBuffers[])
  {
    std::vector<Detail::ConstantBufferData> ConstantBuffers(_BufferNum);
    {
      for (uint ConstantBufferCount = 0; ConstantBufferCount < _BufferNum; ++ConstantBufferCount)
      {
        ConstantBuffers[ConstantBufferCount] = _ConstantBuffers[ConstantBufferCount].GetDetail();
      }
    }

    SetGeometryStageConstantBuffers(_StartSlot, _BufferNum, ConstantBuffers.data());
  }

  template<class T>
  inline void SetPixelStageConstantBuffers(uint _StartSlot, uint _BufferNum, const ConstantBuffer<T> _ConstantBuffers[])
  {
    std::vector<Detail::ConstantBufferData> ConstantBuffers(_BufferNum);
    {
      for (uint ConstantBufferCount = 0; ConstantBufferCount < _BufferNum; ++ConstantBufferCount)
      {
        ConstantBuffers[ConstantBufferCount] = _ConstantBuffers[ConstantBufferCount].GetDetail();
      }
    }

    SetPixelStageConstantBuffers(_StartSlot, _BufferNum, ConstantBuffers.data());
  }

  void Draw(uint _VertexCount);
}
