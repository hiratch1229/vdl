#pragma once
#include "IRenderer.hpp"

class CRenderer : public IRenderer
{
public:
  CRenderer() = default;

  void Initialize()override;

  void SetGraphicsBlendState(const vdl::BlendState& _BlendState, vdl::RenderType _RenderType)override;

  void SetGraphicsDepthStencilState(const vdl::DepthStencilState& _DepthStencilState, vdl::RenderType _RenderType)override;

  void SetGraphicsRasterizerState(const vdl::RasterizerState& _RasterizerState, vdl::RenderType _RenderType)override;

  void SetGraphicsSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::SamplerState _SamplerStates[], ShaderType _Type, vdl::RenderType _RenderType)override;

  void SetGraphicsTextures(vdl::uint _StartSlot, vdl::uint _TextureNum, const vdl::Texture _Textures[], ShaderType _Type, vdl::RenderType _RenderType)override;

  void SetGraphicsConstantBuffers(vdl::uint _StartSlot, vdl::uint _BufferNum, const vdl::detail::ConstantBufferData _ConstantBuffers[], ShaderType _Type, vdl::RenderType _RenderType)override;

  void SetGraphicsShaders(const vdl::VertexShader& _VertexShader, const vdl::HullShader& _HullShader, const vdl::DomainShader& _DomainShader, const vdl::GeometryShader& _GeometryShader, const vdl::PixelShader& _PixelShader, vdl::RenderType _RenderType)override;

  void Draw(const vdl::Texture& _Texture, const vdl::float2& _DstLeftTop, const vdl::float2& _DstSize, const vdl::float2& _SrcLeftPos, const vdl::float2& _SrcSize, const vdl::Radian& _Angle, const vdl::ColorF& _Color)override;

  void Draw(const vdl::StaticMesh& _StaticMesh, const vdl::Matrix& _World, const vdl::ColorF& _Color)override;

  void Draw(const vdl::SkinnedMesh& _SkinnedMesh, const vdl::Matrix& _World, const std::vector<vdl::MotionBlendData>& _MotionBlendDatas, const vdl::ColorF& _Color)override;

  void Flush()override;
};
