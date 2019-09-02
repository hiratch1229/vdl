#pragma once
#include <vdl/Fwd.hpp>
#include <vdl/Types.hpp>

#include <vdl/Shader/IShader.hpp>

class IRenderer
{
public:
  static IRenderer* Create();

  IRenderer() = default;

  virtual ~IRenderer() = default;

  virtual void Initialize() = 0;

  virtual void Flush() = 0;

  virtual void SetGraphicsBlendState(const vdl::BlendState& _BlendState, vdl::RenderType _RenderType) = 0;

  virtual void SetGraphicsDepthStencilState(const vdl::DepthStencilState& _DepthStencilState, vdl::RenderType _RenderType) = 0;

  virtual void SetGraphicsRasterizerState(const vdl::RasterizerState& _RasterizerState, vdl::RenderType _RenderType) = 0;

  virtual void SetGraphicsSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::SamplerState _SamplerStates[], ShaderType _Type, vdl::RenderType _RenderType) = 0;

  virtual void SetGraphicsTextures(vdl::uint _StartSlot, vdl::uint _TextureNum, const vdl::Texture _Textures[], ShaderType _Type, vdl::RenderType _RenderType) = 0;

  virtual void SetGraphicsConstantBuffers(vdl::uint _StartSlot, vdl::uint _BufferNum, const vdl::detail::ConstantBufferData _ConstantBuffers[], ShaderType _Type, vdl::RenderType _RenderType) = 0;

  virtual void SetGraphicsShaders(const vdl::VertexShader& _VertexShader, const vdl::HullShader& _HullShader, const vdl::DomainShader& _DomainShader, const vdl::GeometryShader& _GeometryShader, const vdl::PixelShader& _PixelShader, vdl::RenderType _RenderType) = 0;

  //virtual void SetComputeShader(const vdl::ComputeShader& _ComputeShader) = 0;
};
