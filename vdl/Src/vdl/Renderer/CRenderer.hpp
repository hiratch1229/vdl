#pragma once
#include "IRenderer.hpp"

#include <vdl/Buffer/IBuffer.hpp>

#include <vdl/GraphicsState.hpp>
#include <vdl/Texture.hpp>
#include <vdl/Sampler.hpp>
#include <vdl/ConstantBuffer.hpp>
#include <vdl/Shader.hpp>

#include <assert.h>

class IDeviceContext;

class CRenderer : public IRenderer
{
  static constexpr vdl::uint kRenderTypes = static_cast<vdl::uint>(RenderType::eNum);
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
  IDeviceContext* pDeviceContext_;
  std::unique_ptr<IBuffer> pInstanceBuffer2D_;
  std::unique_ptr<IBuffer> pInstanceBuffer3D_;
private:
  vdl::GraphicsState GraphicsStates_[kRenderTypes];
  Samplers Samplers_[kRenderTypes];
  Textures Textures_[kRenderTypes];
  ConstantBuffers ConstantBuffers_[kRenderTypes];
  Shaders Shaders_[kRenderTypes];
private:
  void FlushSprite();
  void FlushStaticMesh();
  void FlushSkinnedMesh();
public:
  CRenderer() = default;

  void Initialize()override;

  void SetGraphicsState(const vdl::GraphicsState& _GraphicsState, RenderType _RenderType)override
  {
    assert(static_cast<vdl::uint>(_RenderType) < kRenderTypes);

    GraphicsStates_[static_cast<vdl::uint>(_RenderType)] = _GraphicsState;
  }

  void SetGraphicsSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Sampler[], ShaderType _Type, RenderType _RenderType)override
  {
    assert(static_cast<vdl::uint>(_RenderType) < kRenderTypes);

    Samplers& Samplers = Samplers_[static_cast<vdl::uint>(_RenderType)];

    if (const vdl::uint RequiredSize = _StartSlot + _SamplerNum; Samplers.size())
    {
      Samplers.resize(RequiredSize);
    }

    ::memcpy(&Samplers[_StartSlot], _Sampler, _SamplerNum * sizeof(vdl::Sampler));
  }

  void SetGraphicsTextures(vdl::uint _StartSlot, vdl::uint _TextureNum, const vdl::Texture _Textures[], ShaderType _Type, RenderType _RenderType)override
  {
    assert(static_cast<vdl::uint>(_RenderType) < kRenderTypes);

    Textures& Textures = Textures_[static_cast<vdl::uint>(_RenderType)];

    if (const vdl::uint RequiredSize = _StartSlot + _TextureNum; Textures.size())
    {
      Textures.resize(RequiredSize);
    }

    ::memcpy(&Textures[_StartSlot], _Textures, _TextureNum * sizeof(vdl::Texture));
  }

  void SetGraphicsConstantBuffers(vdl::uint _StartSlot, vdl::uint _BufferNum, const vdl::detail::ConstantBufferData _ConstantBuffers[], ShaderType _Type, RenderType _RenderType)override
  {
    assert(static_cast<vdl::uint>(_RenderType) < kRenderTypes);

    ConstantBuffers& ConstantBuffers = ConstantBuffers_[static_cast<vdl::uint>(_RenderType)];

    if (const vdl::uint RequiredSize = _StartSlot + _BufferNum; ConstantBuffers.size())
    {
      ConstantBuffers.resize(RequiredSize);
    }

    ::memcpy(&ConstantBuffers[_StartSlot], _ConstantBuffers, _BufferNum * sizeof(vdl::detail::ConstantBufferData));
  }

  void SetGraphicsShaders(const vdl::VertexShader& _VertexShader, const vdl::HullShader& _HullShader, const vdl::DomainShader& _DomainShader, const vdl::GeometryShader& _GeometryShader, const vdl::PixelShader& _PixelShader, RenderType _RenderType)override
  {
    assert(static_cast<vdl::uint>(_RenderType) < kRenderTypes);

    Shaders& Shaders = Shaders_[static_cast<vdl::uint>(_RenderType)];

    Shaders.VertexShader = _VertexShader;
    Shaders.HullShader = _HullShader;
    Shaders.DomainShader = _DomainShader;
    Shaders.GeometryShader = _GeometryShader;
    Shaders.PixelShader = _PixelShader;
  }

  void Draw(const vdl::Texture& _Texture, const vdl::float2& _DstLeftTop, const vdl::float2& _DstSize, const vdl::float2& _SrcLeftPos, const vdl::float2& _SrcSize, const vdl::Radian& _Angle, const vdl::ColorF& _Color)override;

  void Draw(const vdl::StaticMesh& _StaticMesh, const vdl::Matrix& _World, const vdl::ColorF& _Color)override;

  void Draw(const vdl::SkinnedMesh& _SkinnedMesh, const vdl::Matrix& _World, const std::vector<vdl::MotionBlendData>& _MotionBlendDatas, const vdl::ColorF& _Color)override;

  void Flush()override;
};
