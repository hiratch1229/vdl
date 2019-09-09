#pragma once
#include "IRenderer.hpp"

#include <vdl/Buffer/IBuffer.hpp>

#include <vdl/Scissor.hpp>
#include <vdl/Viewport.hpp>
#include <vdl/GraphicsState.hpp>
#include <vdl/Texture.hpp>
#include <vdl/Sampler.hpp>
#include <vdl/ConstantBuffer.hpp>
#include <vdl/Shader.hpp>

#include <assert.h>

class IDevice;
class IDeviceContext;

class CRenderer : public IRenderer
{
  static constexpr vdl::uint kRenderTypes = static_cast<vdl::uint>(RenderType::eNum);
  static constexpr vdl::uint kShaderTypes = static_cast<vdl::uint>(ShaderType::eGraphicsNum);
  static constexpr vdl::Vertex2D kRectangle[] = {
  { { -0.5f, -0.5f }, { 0.0f, 0.0f } },
  { { +0.5f, -0.5f }, { 1.0f, 0.0f } },
  { { -0.5f, +0.5f }, { 0.0f, 1.0f } },
  { { +0.5f, +0.5f }, { 1.0f, 1.0f } } };
private:
  struct OutputManager
  {
    vdl::RenderTexture RenderTexture;
    vdl::DepthStencilTexture DepthStencilTexture;
  };
  struct Shaders
  {
    vdl::VertexShader VertexShader;
    vdl::HullShader HullShader;
    vdl::DomainShader DomainShader;
    vdl::GeometryShader GeometryShader;
    vdl::PixelShader PixelShader;
  };
  using Samplers = std::vector<vdl::Sampler>;
  using Textures = std::vector<vdl::Texture>;
  using ConstantBuffers = std::vector<vdl::detail::ConstantBufferData>;
  struct Instance2D
  {
    vdl::Matrix NDCTransform;
    vdl::float2 TexcoordScale;
    vdl::float2 TexcoordTranslate;
    vdl::ColorF Color;
  };
  struct Instance3D
  {
    vdl::Matrix World;
    vdl::ColorF MaterialColor;
  };
private:
  IDevice* pDevice_;
  IDeviceContext* pDeviceContext_;
  std::unique_ptr<IBuffer> pVertexBuffer2D_;
  std::unique_ptr<IBuffer> pInstanceBuffer2D_;
  std::vector<Instance2D> Instances2D_;
  std::unique_ptr<IBuffer> pInstanceBuffer3D_;
  std::vector<Instance3D> Instances3D_;
private:
  vdl::Scissor Scissors_[kRenderTypes];
  vdl::Viewport Viewports_[kRenderTypes];
  vdl::GraphicsState GraphicsStates_[kRenderTypes];
  OutputManager OutputManagers_[kRenderTypes];
  Shaders Shaders_[kRenderTypes];
  Samplers Samplers_[kRenderTypes][kShaderTypes];
  Textures Textures_[kRenderTypes][kShaderTypes];
  ConstantBuffers ConstantBuffers_[kRenderTypes][kShaderTypes];
private:
  void SetState(RenderType _RenderType);
  void FlushSprite();
  void FlushStaticMesh();
  void FlushSkinnedMesh();
public:
  CRenderer() = default;

  void Initialize()override;

  void SetScissor(const vdl::Scissor& _Scissor, RenderType _RenderType)override
  {
    assert(static_cast<vdl::uint>(_RenderType) < kRenderTypes);

    Scissors_[static_cast<vdl::uint>(_RenderType)] = _Scissor;
  }

  void SetViewport(const vdl::Viewport& _Viewport, RenderType _RenderType)override
  {
    assert(static_cast<vdl::uint>(_RenderType) < kRenderTypes);

    Viewports_[static_cast<vdl::uint>(_RenderType)] = _Viewport;
  }

  void SetBlendState(const vdl::BlendState& _BlendState, RenderType _RenderType)override
  {
    assert(static_cast<vdl::uint>(_RenderType) < kRenderTypes);

    GraphicsStates_[static_cast<vdl::uint>(_RenderType)].BlendState = _BlendState;
  }

  void SetDepthStencilState(const vdl::DepthStencilState& _DepthStencilState, RenderType _RenderType)override
  {
    assert(static_cast<vdl::uint>(_RenderType) < kRenderTypes);

    GraphicsStates_[static_cast<vdl::uint>(_RenderType)].DepthSteniclState = _DepthStencilState;
  }

  void SetRasterizerState(const vdl::RasterizerState& _RasterizerState, RenderType _RenderType)override
  {
    assert(static_cast<vdl::uint>(_RenderType) < kRenderTypes);

    GraphicsStates_[static_cast<vdl::uint>(_RenderType)].RasterizerState = _RasterizerState;
  }

  void SetRenderTexture(const vdl::RenderTexture& _RenderTexture, const vdl::DepthStencilTexture& _DepthStenilTexture, RenderType _RenderType)override
  {
    assert(static_cast<vdl::uint>(_RenderType) < kRenderTypes);

    OutputManagers_[static_cast<vdl::uint>(_RenderType)] = { _RenderTexture, _DepthStenilTexture };
  }

  void SetVertexShader(const vdl::VertexShader& _VertexShader, RenderType _RenderType)override
  {
    assert(static_cast<vdl::uint>(_RenderType) < kRenderTypes);

    Shaders_[static_cast<vdl::uint>(_RenderType)].VertexShader = _VertexShader;
  }

  void SetHullShader(const vdl::HullShader& _HullShader, RenderType _RenderType)override
  {
    assert(static_cast<vdl::uint>(_RenderType) < kRenderTypes);

    Shaders_[static_cast<vdl::uint>(_RenderType)].HullShader = _HullShader;
  }

  void SetDomainShader(const vdl::DomainShader& _DomainShader, RenderType _RenderType)override
  {
    assert(static_cast<vdl::uint>(_RenderType) < kRenderTypes);

    Shaders_[static_cast<vdl::uint>(_RenderType)].DomainShader = _DomainShader;
  }

  void SetGeometryShader(const vdl::GeometryShader& _GeometryShader, RenderType _RenderType)override
  {
    assert(static_cast<vdl::uint>(_RenderType) < kRenderTypes);

    Shaders_[static_cast<vdl::uint>(_RenderType)].GeometryShader = _GeometryShader;
  }

  void SetPixelShader(const vdl::PixelShader& _PixelShader, RenderType _RenderType)override
  {
    assert(static_cast<vdl::uint>(_RenderType) < kRenderTypes);

    Shaders_[static_cast<vdl::uint>(_RenderType)].PixelShader = _PixelShader;
  }

  void SetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Sampler[], ShaderType _ShaderType, RenderType _RenderType)override
  {
    assert(static_cast<vdl::uint>(_RenderType) < kRenderTypes && static_cast<vdl::uint>(_ShaderType) < kShaderTypes);

    Samplers& Samplers = Samplers_[static_cast<vdl::uint>(_RenderType)][static_cast<vdl::uint>(_ShaderType)];

    if (const vdl::uint RequiredSize = _StartSlot + _SamplerNum; Samplers.size())
    {
      Samplers.resize(RequiredSize);
    }

    ::memcpy(&Samplers[_StartSlot], _Sampler, _SamplerNum * sizeof(vdl::Sampler));
  }

  void SetTextures(vdl::uint _StartSlot, vdl::uint _TextureNum, const vdl::Texture _Textures[], ShaderType _ShaderType, RenderType _RenderType)override
  {
    assert(static_cast<vdl::uint>(_RenderType) < kRenderTypes && static_cast<vdl::uint>(_ShaderType) < kShaderTypes);

    Textures& Textures = Textures_[static_cast<vdl::uint>(_RenderType)][static_cast<vdl::uint>(_ShaderType)];

    if (const vdl::uint RequiredSize = _StartSlot + _TextureNum; Textures.size())
    {
      Textures.resize(RequiredSize);
    }

    ::memcpy(&Textures[_StartSlot], _Textures, _TextureNum * sizeof(vdl::Texture));
  }

  void SetConstantBuffers(vdl::uint _StartSlot, vdl::uint _BufferNum, const vdl::detail::ConstantBufferData _ConstantBuffers[], ShaderType _ShaderType, RenderType _RenderType)override
  {
    assert(static_cast<vdl::uint>(_RenderType) < kRenderTypes && static_cast<vdl::uint>(_ShaderType) < kShaderTypes);

    ConstantBuffers& ConstantBuffers = ConstantBuffers_[static_cast<vdl::uint>(_RenderType)][static_cast<vdl::uint>(_ShaderType)];

    if (const vdl::uint RequiredSize = _StartSlot + _BufferNum; ConstantBuffers.size())
    {
      ConstantBuffers.resize(RequiredSize);
    }

    ::memcpy(&ConstantBuffers[_StartSlot], _ConstantBuffers, _BufferNum * sizeof(vdl::detail::ConstantBufferData));
  }

  void Draw(const vdl::Texture& _Texture, const vdl::float2& _DstLeftTop, const vdl::float2& _DstSize, const vdl::float2& _SrcLeftPos, const vdl::float2& _SrcSize, const vdl::Radian& _Angle, const vdl::ColorF& _Color)override;

  void Draw(const vdl::StaticMesh& _StaticMesh, const vdl::Matrix& _World, const vdl::ColorF& _Color)override;

  void Draw(const vdl::SkinnedMesh& _SkinnedMesh, const vdl::Matrix& _World, const std::vector<vdl::MotionBlendData>& _MotionBlendDatas, const vdl::ColorF& _Color)override;

  void Flush()override;
};
