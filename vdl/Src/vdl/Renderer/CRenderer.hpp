#pragma once
#include "IRenderer.hpp"

#include <vdl/Camera.hpp>
#include <vdl/ConstantBuffer.hpp>

#include "RendererCommand/RendererCommand.hpp"

#include <vdl/Buffer/IBuffer.hpp>

#include <memory>
#include <assert.h>

class IDevice;
class IDeviceContext;

class CRenderer : public IRenderer
{
  static constexpr vdl::uint kRenderTypes = static_cast<vdl::uint>(RenderType::eNum);
  static constexpr vdl::uint kShaderTypes = static_cast<vdl::uint>(ShaderType::eGraphicsNum);
  static constexpr vdl::TextureVertex kRectangle[] = {
  { { -0.5f, -0.5f }, { 0.0f, 0.0f } },
  { { +0.5f, -0.5f }, { 1.0f, 0.0f } },
  { { -0.5f, +0.5f }, { 0.0f, 1.0f } },
  { { +0.5f, +0.5f }, { 1.0f, 1.0f } } };
private:
  struct OutputManager
  {
    vdl::RenderTextures RenderTextures;
    vdl::DepthStencilTexture DepthStencilTexture;
  public:
    [[nodiscard]] bool operator==(const OutputManager& _OutputManager)const { return RenderTextures == _OutputManager.RenderTextures && DepthStencilTexture == _OutputManager.DepthStencilTexture; }

    [[nodiscard]] bool operator!=(const OutputManager& _OutputManager)const { return RenderTextures != _OutputManager.RenderTextures || DepthStencilTexture != _OutputManager.DepthStencilTexture; }
  };
  struct TextureInstanceData
  {
    vdl::Matrix NDCTransform;
    vdl::float2 TexcoordScale;
    vdl::float2 TexcoordTranslate;
    vdl::ColorF Color;
  };
  struct StaticMeshInstanceData
  {
    vdl::Matrix World;
    vdl::ColorF Color;
  };
  struct SkinnedMeshinstanceData : public StaticMeshInstanceData
  {
    //  TODO
  };
  struct ConstantBufferData
  {
    vdl::Matrix ViewProjection;
  };
  struct CameraData
  {
    vdl::Camera Camera;
    vdl::ConstantBuffer<ConstantBufferData> ConstantBuffer;
    bool isChange;
  };
private:
  IDevice* pDevice_;
  IDeviceContext* pDeviceContext_;
  std::unique_ptr<IBuffer> pTextureVertexBuffer_;
  std::unique_ptr<IBuffer> pTextureInstanceBuffer_;
  std::unique_ptr<IBuffer> pStaticMeshInstanceBuffer_;
  std::unique_ptr<IBuffer> pSkinnedMeshInstanceBuffer_;
private:
  std::unique_ptr<CameraData> pStaticMeshCameraData_;
  std::unique_ptr<CameraData> pSkinnedMeshCameraData_;
private:
  OutputManager OutputManager_;
  RendererCommandList<vdl::Texture, TextureInstanceData> TextureRendererCommandList_;
  RendererCommandList<vdl::StaticMesh, StaticMeshInstanceData> StaticMeshRendererCommandList_;
  RendererCommandList<vdl::SkinnedMesh, SkinnedMeshinstanceData> SkinnedMeshRendererCommandList_;
public:
  CRenderer() = default;

  void Initialize()override;

  vdl::Matrix GetView(RenderType _Type)const override;

  vdl::Matrix GetProjection(RenderType _Type)const override;

  void SetCamera(const vdl::Camera& _Camera, RenderType _Type)override;

  void SetRenderTextures(const vdl::RenderTextures& _RenderTextures, const vdl::DepthStencilTexture& _DepthStencilTexture)override;

  void SetTopology(vdl::Topology _Topology, RenderType _Type)override
  {
    switch (_Type)
    {
    case RenderType::eNone:
      //  TODO
      break;
    case RenderType::eTexture:
      TextureRendererCommandList_.PushTopology(_Topology);
      break;
    case RenderType::eStaticMesh:
      StaticMeshRendererCommandList_.PushTopology(_Topology);
      break;
    case RenderType::eSkinnedMesh:
      SkinnedMeshRendererCommandList_.PushTopology(_Topology);
      break;
    default: assert(false);
    }
  }

  void SetScissor(const vdl::Scissor& _Scissor, RenderType _Type)override
  {
    switch (_Type)
    {
    case RenderType::eNone:
      //  TODO
      break;
    case RenderType::eTexture:
      TextureRendererCommandList_.PushScissor(_Scissor);
      break;
    case RenderType::eStaticMesh:
      StaticMeshRendererCommandList_.PushScissor(_Scissor);
      break;
    case RenderType::eSkinnedMesh:
      SkinnedMeshRendererCommandList_.PushScissor(_Scissor);
      break;
    default: assert(false);
    }
  }

  void SetViewport(const vdl::Viewport& _Viewport, RenderType _Type)override
  {
    switch (_Type)
    {
    case RenderType::eNone:
      //  TODO
      break;
    case RenderType::eTexture:
      TextureRendererCommandList_.PushViewport(_Viewport);
      break;
    case RenderType::eStaticMesh:
      StaticMeshRendererCommandList_.PushViewport(_Viewport);
      break;
    case RenderType::eSkinnedMesh:
      SkinnedMeshRendererCommandList_.PushViewport(_Viewport);
      break;
    default: assert(false);
    }
  }

  void SetBlendState(const vdl::BlendState& _BlendState, RenderType _Type)override
  {
    switch (_Type)
    {
    case RenderType::eNone:
      //  TODO
      break;
    case RenderType::eTexture:
      TextureRendererCommandList_.PushBlendState(_BlendState);
      break;
    case RenderType::eStaticMesh:
      StaticMeshRendererCommandList_.PushBlendState(_BlendState);
      break;
    case RenderType::eSkinnedMesh:
      SkinnedMeshRendererCommandList_.PushBlendState(_BlendState);
      break;
    default: assert(false);
    }
  }

  void SetDepthStencilState(const vdl::DepthStencilState& _DepthStencilState, RenderType _Type)override
  {
    switch (_Type)
    {
    case RenderType::eNone:
      //  TODO
      break;
    case RenderType::eTexture:
      TextureRendererCommandList_.PushDepthStencilState(_DepthStencilState);
      break;
    case RenderType::eStaticMesh:
      StaticMeshRendererCommandList_.PushDepthStencilState(_DepthStencilState);
      break;
    case RenderType::eSkinnedMesh:
      SkinnedMeshRendererCommandList_.PushDepthStencilState(_DepthStencilState);
      break;
    default: assert(false);
    }
  }

  void SetRasterizerState(const vdl::RasterizerState& _RasterizerState, RenderType _Type)override
  {
    switch (_Type)
    {
    case RenderType::eNone:
      //  TODO
      break;
    case RenderType::eTexture:
      TextureRendererCommandList_.PushRasterizerState(_RasterizerState);
      break;
    case RenderType::eStaticMesh:
      StaticMeshRendererCommandList_.PushRasterizerState(_RasterizerState);
      break;
    case RenderType::eSkinnedMesh:
      SkinnedMeshRendererCommandList_.PushRasterizerState(_RasterizerState);
      break;
    default: assert(false);
    }
  }

  void SetVertexShader(const vdl::VertexShader& _VertexShader, RenderType _Type)override
  {
    switch (_Type)
    {
    case RenderType::eNone:
      //  TODO
      break;
    case RenderType::eTexture:
      TextureRendererCommandList_.PushVertexShader(_VertexShader);
      break;
    case RenderType::eStaticMesh:
      StaticMeshRendererCommandList_.PushVertexShader(_VertexShader);
      break;
    case RenderType::eSkinnedMesh:
      SkinnedMeshRendererCommandList_.PushVertexShader(_VertexShader);
      break;
    default: assert(false);
    }
  }

  void SetHullShader(const vdl::HullShader& _HullShader, RenderType _Type)override
  {
    switch (_Type)
    {
    case RenderType::eNone:
      //  TODO
      break;
    case RenderType::eTexture:
      TextureRendererCommandList_.PushHullShader(_HullShader);
      break;
    case RenderType::eStaticMesh:
      StaticMeshRendererCommandList_.PushHullShader(_HullShader);
      break;
    case RenderType::eSkinnedMesh:
      SkinnedMeshRendererCommandList_.PushHullShader(_HullShader);
      break;
    default: assert(false);
    }
  }

  void SetDomainShader(const vdl::DomainShader& _DomainShader, RenderType _Type)override
  {
    switch (_Type)
    {
    case RenderType::eNone:
      //  TODO
      break;
    case RenderType::eTexture:
      TextureRendererCommandList_.PushDomainShader(_DomainShader);
      break;
    case RenderType::eStaticMesh:
      StaticMeshRendererCommandList_.PushDomainShader(_DomainShader);
      break;
    case RenderType::eSkinnedMesh:
      SkinnedMeshRendererCommandList_.PushDomainShader(_DomainShader);
      break;
    default: assert(false);
    }
  }

  void SetGeometryShader(const vdl::GeometryShader& _GeometryShader, RenderType _Type)override
  {
    switch (_Type)
    {
    case RenderType::eNone:
      //  TODO
      break;
    case RenderType::eTexture:
      TextureRendererCommandList_.PushGeometryShader(_GeometryShader);
      break;
    case RenderType::eStaticMesh:
      StaticMeshRendererCommandList_.PushGeometryShader(_GeometryShader);
      break;
    case RenderType::eSkinnedMesh:
      SkinnedMeshRendererCommandList_.PushGeometryShader(_GeometryShader);
      break;
    default: assert(false);
    }
  }

  void SetPixelShader(const vdl::PixelShader& _PixelShader, RenderType _Type)override
  {
    switch (_Type)
    {
    case RenderType::eNone:
      //  TODO
      break;
    case RenderType::eTexture:
      TextureRendererCommandList_.PushPixelShader(_PixelShader);
      break;
    case RenderType::eStaticMesh:
      StaticMeshRendererCommandList_.PushPixelShader(_PixelShader);
      break;
    case RenderType::eSkinnedMesh:
      SkinnedMeshRendererCommandList_.PushPixelShader(_PixelShader);
      break;
    default: assert(false);
    }
  }

  void SetTextures(vdl::uint _StartSlot, vdl::uint _TextureNum, const vdl::Texture _Textures[], ShaderType _Stage, RenderType _Type)override
  {
    auto SetTexture = [&](auto* _pRendererCommandList)->void
    {
      for (vdl::uint TextureCount = 0; TextureCount < _TextureNum; ++TextureCount)
      {
        switch (_Stage)
        {
        case ShaderType::eVertexShader:
          _pRendererCommandList->PushVertexStageTexture(_Textures[TextureCount], _StartSlot + TextureCount);
          break;
        case ShaderType::eHullShader:
          _pRendererCommandList->PushHullStageTexture(_Textures[TextureCount], _StartSlot + TextureCount);
          break;
        case ShaderType::eDomainShader:
          _pRendererCommandList->PushDomainStageTexture(_Textures[TextureCount], _StartSlot + TextureCount);
          break;
        case ShaderType::eGeometryShader:
          _pRendererCommandList->PushGeometryStageTexture(_Textures[TextureCount], _StartSlot + TextureCount);
          break;
        case ShaderType::ePixelShader:
          _pRendererCommandList->PushPixelStageTexture(_Textures[TextureCount], _StartSlot + TextureCount);
          break;
        default: assert(false);
        }

      }
    };

    switch (_Type)
    {
    case RenderType::eNone:
      //  TODO
      break;
    case RenderType::eTexture:
      SetTexture(&TextureRendererCommandList_);
      break;
    case RenderType::eStaticMesh:
      SetTexture(&StaticMeshRendererCommandList_);
      break;
    case RenderType::eSkinnedMesh:
      SetTexture(&SkinnedMeshRendererCommandList_);
      break;
    default: assert(false);
    }
  }

  void SetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[], ShaderType _Stage, RenderType _Type)override
  {
    auto SetSampler = [&](auto* _pRendererCommandList)->void
    {
      for (vdl::uint SamplerCount = 0; SamplerCount < _SamplerNum; ++SamplerCount)
      {
        switch (_Stage)
        {
        case ShaderType::eVertexShader:
          _pRendererCommandList->PushVertexStageSampler(_Samplers[SamplerCount], _StartSlot + SamplerCount);
          break;
        case ShaderType::eHullShader:
          _pRendererCommandList->PushHullStageSampler(_Samplers[SamplerCount], _StartSlot + SamplerCount);
          break;
        case ShaderType::eDomainShader:
          _pRendererCommandList->PushDomainStageSampler(_Samplers[SamplerCount], _StartSlot + SamplerCount);
          break;
        case ShaderType::eGeometryShader:
          _pRendererCommandList->PushGeometryStageSampler(_Samplers[SamplerCount], _StartSlot + SamplerCount);
          break;
        case ShaderType::ePixelShader:
          _pRendererCommandList->PushPixelStageSampler(_Samplers[SamplerCount], _StartSlot + SamplerCount);
          break;
        default: assert(false);
        }

      }
    };

    switch (_Type)
    {
    case RenderType::eNone:
      //  TODO
      break;
    case RenderType::eTexture:
      SetSampler(&TextureRendererCommandList_);
      break;
    case RenderType::eStaticMesh:
      SetSampler(&StaticMeshRendererCommandList_);
      break;
    case RenderType::eSkinnedMesh:
      SetSampler(&SkinnedMeshRendererCommandList_);
      break;
    default: assert(false);
    }
  }

  void SetConstantBuffers(vdl::uint _StartSlot, vdl::uint _BufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[], ShaderType _Stage, RenderType _Type)override
  {
    auto SetConstantBuffer = [&](auto* _pRendererCommandList)->void
    {
      for (vdl::uint ConstantBufferCount = 0; ConstantBufferCount < _BufferNum; ++ConstantBufferCount)
      {
        switch (_Stage)
        {
        case ShaderType::eVertexShader:
          _pRendererCommandList->PushVertexStageConstantBuffer(_ConstantBuffers[ConstantBufferCount], _StartSlot + ConstantBufferCount);
          break;
        case ShaderType::eHullShader:
          _pRendererCommandList->PushHullStageConstantBuffer(_ConstantBuffers[ConstantBufferCount], _StartSlot + ConstantBufferCount);
          break;
        case ShaderType::eDomainShader:
          _pRendererCommandList->PushDomainStageConstantBuffer(_ConstantBuffers[ConstantBufferCount], _StartSlot + ConstantBufferCount);
          break;
        case ShaderType::eGeometryShader:
          _pRendererCommandList->PushGeometryStageConstantBuffer(_ConstantBuffers[ConstantBufferCount], _StartSlot + ConstantBufferCount);
          break;
        case ShaderType::ePixelShader:
          _pRendererCommandList->PushPixelStageConstantBuffer(_ConstantBuffers[ConstantBufferCount], _StartSlot + ConstantBufferCount);
          break;
        default: assert(false);
        }

      }
    };

    switch (_Type)
    {
    case RenderType::eNone:
      //  TODO
      break;
    case RenderType::eTexture:
      SetConstantBuffer(&TextureRendererCommandList_);
      break;
    case RenderType::eStaticMesh:
      SetConstantBuffer(&StaticMeshRendererCommandList_);
      break;
    case RenderType::eSkinnedMesh:
      SetConstantBuffer(&SkinnedMeshRendererCommandList_);
      break;
    default: assert(false);
    }
  }

  void Draw(const vdl::Texture& _Texture, const vdl::float2& _DestLeftTop, const vdl::float2& _DestSize, const vdl::float2& _SrcLeftPos, const vdl::float2& _SrcSize, const vdl::Radian& _Angle, const vdl::ColorF& _Color)override;

  void Draw(const vdl::StaticMesh& _StaticMesh, const vdl::Matrix& _World, const vdl::ColorF& _Color)override;

  void Draw(const vdl::SkinnedMesh& _SkinnedMesh, const vdl::Matrix& _World, const vdl::MotionBlendDatas& _MotionBlendDatas, const vdl::ColorF& _Color)override;

  void Clear(const vdl::RenderTexture& _RenderTexture, const vdl::ColorF& _ClearColor)override;

  void Clear(const vdl::DepthStencilTexture& _DepthStencilTexture, float _ClearDepth, vdl::uint _ClearStencil)override;

  void Flush()override;
};
