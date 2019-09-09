#pragma once
#include <vdl/Fwd.hpp>
#include <vdl/Types.hpp>

#include <vdl/Shader/IShader.hpp>

#include <vector>

enum class RenderType
{
  eNone,
  eSprite,
  eStaticMesh,
  eSkinnedMesh,

  eNum
};


class IRenderer
{
public:
  static IRenderer* Create();

  IRenderer() = default;

  virtual ~IRenderer() = default;

  virtual void Initialize() = 0;

  virtual void SetGraphicsState(const vdl::GraphicsState& _GraphicsState, RenderType _RenderType) = 0;

  virtual void SetGraphicsSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Sampler[], ShaderType _Type, RenderType _RenderType) = 0;

  virtual void SetGraphicsTextures(vdl::uint _StartSlot, vdl::uint _TextureNum, const vdl::Texture _Textures[], ShaderType _Type, RenderType _RenderType) = 0;

  virtual void SetGraphicsConstantBuffers(vdl::uint _StartSlot, vdl::uint _BufferNum, const vdl::detail::ConstantBufferData _ConstantBuffers[], ShaderType _Type, RenderType _RenderType) = 0;

  virtual void SetGraphicsShaders(const vdl::VertexShader& _VertexShader, const vdl::HullShader& _HullShader, const vdl::DomainShader& _DomainShader, const vdl::GeometryShader& _GeometryShader, const vdl::PixelShader& _PixelShader, RenderType _RenderType) = 0;

  virtual void Draw(const vdl::Texture& _Texture, const vdl::float2& _DstLeftTop, const vdl::float2& _DstSize, const vdl::float2& _SrcLeftPos, const vdl::float2& _SrcSize, const vdl::Radian& _Angle, const vdl::ColorF& _Color) = 0;

  virtual void Draw(const vdl::StaticMesh& _StaticMesh, const vdl::Matrix& _World, const vdl::ColorF& _Color) = 0;

  virtual void Draw(const vdl::SkinnedMesh& _SkinnedMesh, const vdl::Matrix& _World, const std::vector<vdl::MotionBlendData>& _MotionBlendDatas, const vdl::ColorF& _Color) = 0;

  virtual void Flush() = 0;

  //virtual void SetComputeShader(const vdl::ComputeShader& _ComputeShader) = 0;
};
