#pragma once
#include "Fwd.hpp"
#include "Color.hpp"
#include "Matrix.hpp"
#include "Model.hpp"
#include "MotionData.hpp"
#include "GraphicsStates.hpp"
#include "Shader.hpp"
#include "ConstantBuffer.hpp"
#include "UnorderedAccessBuffer.hpp"

#include <assert.h>

namespace vdl::Renderer3D
{
  Matrix GetView();

  Matrix GetProjection();

  void SetCamera(const Camera& _Camera);

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

  inline void SetGraphicsState(const GraphicsStates& _GraphicsStates)
  {
    SetBlendState(_GraphicsStates.BlendState);
    SetDepthStencilState(_GraphicsStates.DepthStencilState);
    SetRasterizerState(_GraphicsStates.RasterizerState);
  }

  void SetStaticMeshVertexShader(const VertexShader& _VertexShader);

  //void SetSkinnedMeshVertexShader(const VertexShader& _VertexShader);

  void SetMeshHullShader(const HullShader& _HullShader);

  void SetMeshDomainShader(const DomainShader& _DomainShader);

  void SetMeshGeometryShader(const GeometryShader& _GeometryShader);

  void SetMeshPixelShader(const PixelShader& _PixelShader);

  inline void SetStaticMeshShaders(const VertexShader& _VertexShader, const PixelShader& _PixelShader)
  {
    SetStaticMeshVertexShader(_VertexShader);
    SetMeshHullShader(HullShader());
    SetMeshDomainShader(DomainShader());
    SetMeshGeometryShader(GeometryShader());
    SetMeshPixelShader(_PixelShader);
  }

  inline void SetStaticMeshShaders(const VertexShader& _VertexShader, const GeometryShader& _GeometryShader, const PixelShader& _PixelShader)
  {
    SetStaticMeshVertexShader(_VertexShader);
    SetMeshHullShader(HullShader());
    SetMeshDomainShader(DomainShader());
    SetMeshGeometryShader(_GeometryShader);
    SetMeshPixelShader(_PixelShader);
  }

  inline void SetStaticMeshShaders(const VertexShader& _VertexShader, const HullShader& _HullShader, const DomainShader& _DomainShader, const PixelShader& _PixelShader)
  {
    SetStaticMeshVertexShader(_VertexShader);
    SetMeshHullShader(_HullShader);
    SetMeshDomainShader(_DomainShader);
    SetMeshGeometryShader(GeometryShader());
    SetMeshPixelShader(_PixelShader);
  }

  inline void SetStaticMeshShaders(const VertexShader& _VertexShader, const HullShader& _HullShader, const DomainShader& _DomainShader, const GeometryShader& _GeometryShader, const PixelShader& _PixelShader)
  {
    SetStaticMeshVertexShader(_VertexShader);
    SetMeshHullShader(_HullShader);
    SetMeshDomainShader(_DomainShader);
    SetMeshGeometryShader(_GeometryShader);
    SetMeshPixelShader(_PixelShader);
  }

  inline void SetStaticMeshGraphicsShaders(const GraphicsShaders& _GraphicsShaders)
  {
    SetStaticMeshVertexShader(_GraphicsShaders.VertexShader);
    SetMeshHullShader(_GraphicsShaders.HullShader);
    SetMeshDomainShader(_GraphicsShaders.DomainShader);
    SetMeshGeometryShader(_GraphicsShaders.GeometryShader);
    SetMeshPixelShader(_GraphicsShaders.PixelShader);
  }

  //inline void SetSkinnedMeshShaders(const VertexShader& _VertexShader, const PixelShader& _PixelShader)
  //{
  //  SetSkinnedMeshVertexShader(_VertexShader);
  //  SetMeshHullShader(HullShader());
  //  SetMeshDomainShader(DomainShader());
  //  SetMeshGeometryShader(GeometryShader());
  //  SetMeshPixelShader(_PixelShader);
  //}
  //
  //inline void SetSkinnedMeshShaders(const VertexShader& _VertexShader, const GeometryShader& _GeometryShader, const PixelShader& _PixelShader)
  //{
  //  SetSkinnedMeshVertexShader(_VertexShader);
  //  SetMeshHullShader(HullShader());
  //  SetMeshDomainShader(DomainShader());
  //  SetMeshGeometryShader(_GeometryShader);
  //  SetMeshPixelShader(_PixelShader);
  //}
  //
  //inline void SetSkinnedMeshShaders(const VertexShader& _VertexShader, const HullShader& _HullShader, const DomainShader& _DomainShader, const PixelShader& _PixelShader)
  //{
  //  SetSkinnedMeshVertexShader(_VertexShader);
  //  SetMeshHullShader(_HullShader);
  //  SetMeshDomainShader(_DomainShader);
  //  SetMeshGeometryShader(GeometryShader());
  //  SetMeshPixelShader(_PixelShader);
  //}
  //
  //inline void SetSkinnedMeshShaders(const VertexShader& _VertexShader, const HullShader& _HullShader, const DomainShader& _DomainShader, const GeometryShader& _GeometryShader, const PixelShader& _PixelShader)
  //{
  //  SetSkinnedMeshVertexShader(_VertexShader);
  //  SetMeshHullShader(_HullShader);
  //  SetMeshDomainShader(_DomainShader);
  //  SetMeshGeometryShader(_GeometryShader);
  //  SetMeshPixelShader(_PixelShader);
  //}
  //
  //inline void SetSkinnedMeshGraphicsShaders(const GraphicsShaders& _GraphicsShaders)
  //{
  //  SetSkinnedMeshVertexShader(_GraphicsShaders.VertexShader);
  //  SetMeshHullShader(_GraphicsShaders.HullShader);
  //  SetMeshDomainShader(_GraphicsShaders.DomainShader);
  //  SetMeshGeometryShader(_GraphicsShaders.GeometryShader);
  //  SetMeshPixelShader(_GraphicsShaders.PixelShader);
  //}

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

  inline void SetVertexStageShaderResources(uint _StartSlot, uint _CubeTextureNum, const CubeTexture _CubeTextures[])
  {
    std::vector<ShaderResource> ShaderResources(_CubeTextureNum);
    {
      for (uint CubeTextureCount = 0; CubeTextureCount < _CubeTextureNum; ++CubeTextureCount)
      {
        ShaderResources[CubeTextureCount] = _CubeTextures[CubeTextureCount];
      }
    }

    SetVertexStageShaderResources(_StartSlot, _CubeTextureNum, ShaderResources.data());
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

  inline void SetHullStageShaderResources(uint _StartSlot, uint _CubeTextureNum, const CubeTexture _CubeTextures[])
  {
    std::vector<ShaderResource> ShaderResources(_CubeTextureNum);
    {
      for (uint CubeTextureCount = 0; CubeTextureCount < _CubeTextureNum; ++CubeTextureCount)
      {
        ShaderResources[CubeTextureCount] = _CubeTextures[CubeTextureCount];
      }
    }

    SetHullStageShaderResources(_StartSlot, _CubeTextureNum, ShaderResources.data());
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

  inline void SetDomainStageShaderResources(uint _StartSlot, uint _CubeTextureNum, const CubeTexture _CubeTextures[])
  {
    std::vector<ShaderResource> ShaderResources(_CubeTextureNum);
    {
      for (uint CubeTextureCount = 0; CubeTextureCount < _CubeTextureNum; ++CubeTextureCount)
      {
        ShaderResources[CubeTextureCount] = _CubeTextures[CubeTextureCount];
      }
    }

    SetDomainStageShaderResources(_StartSlot, _CubeTextureNum, ShaderResources.data());
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

  inline void SetGeometryStageShaderResources(uint _StartSlot, uint _CubeTextureNum, const CubeTexture _CubeTextures[])
  {
    std::vector<ShaderResource> ShaderResources(_CubeTextureNum);
    {
      for (uint CubeTextureCount = 0; CubeTextureCount < _CubeTextureNum; ++CubeTextureCount)
      {
        ShaderResources[CubeTextureCount] = _CubeTextures[CubeTextureCount];
      }
    }

    SetGeometryStageShaderResources(_StartSlot, _CubeTextureNum, ShaderResources.data());
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

  inline void SetPixelStageShaderResources(uint _StartSlot, uint _CubeTextureNum, const CubeTexture _CubeTextures[])
  {
    std::vector<ShaderResource> ShaderResources(_CubeTextureNum);
    {
      for (uint CubeTextureCount = 0; CubeTextureCount < _CubeTextureNum; ++CubeTextureCount)
      {
        ShaderResources[CubeTextureCount] = _CubeTextures[CubeTextureCount];
      }
    }

    SetPixelStageShaderResources(_StartSlot, _CubeTextureNum, ShaderResources.data());
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

  void Draw(const StaticMesh& _StaticMesh, const Matrix& _World = Matrix::Identity(), const Color4F& _Color = Palette::White);

  inline void Draw(const Model& _Model, const Matrix& _World = Matrix::Identity(), const Color4F& _Color = Palette::White)
  {
    assert(!_Model.isEmpty());

    for (auto& Mesh : _Model.GetMeshes())
    {
      Draw(Mesh, _World, _Color);
    }
  }

  //void Draw(const SkinnedMesh& _SkinnedMesh, const Matrix& _World, const MotionData& _MotionData, const ColorF& _Color = Palette::White);
  //
  //void Draw(const SkinnedMesh& _SkinnedMesh, const Matrix& _World, const MotionBlendDatas& _MotionBlendDatas, const ColorF& _Color = Palette::White);
  //
  //inline void Draw(const Model& _Model, const Matrix& _World, const MotionData& _MotionData, const ColorF& _Color = Palette::White)
  //{
  //  for (auto& Mesh : _Model.GetMeshes())
  //  {
  //    Draw(Mesh, _World, _MotionData, _Color);
  //  }
  //}
  //
  //inline void Draw(const Model& _Model, const Matrix& _World, const std::vector<MotionBlendData>& _MotionBlendDatas, const ColorF& _Color = Palette::White)
  //{
  //  for (auto& Mesh : _Model.GetMeshes())
  //  {
  //    Draw(Mesh, _World, _MotionBlendDatas, _Color);
  //  }
  //}
}
