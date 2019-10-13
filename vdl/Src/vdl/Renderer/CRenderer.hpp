#pragma once
#include "IRenderer.hpp"
#include "RendererCommand/RendererCommand.hpp"

#include <vdl/Camera.hpp>
#include <vdl/ConstantBuffer.hpp>

#include <vdl/Buffer/Buffer.hpp>
#include <vdl/Instance/Instance.hpp>

#include <memory>

class IDevice;
class IDeviceContext;

class CRenderer : public IRenderer
{
  static constexpr vdl::uint kShaderTypes = static_cast<vdl::uint>(ShaderType::eGraphicsNum);
  static constexpr vdl::Vertex2D kRectangle[] = {
  { { -0.5f, -0.5f }, { 0.0f, 0.0f } },
  { { +0.5f, -0.5f }, { 1.0f, 0.0f } },
  { { -0.5f, +0.5f }, { 0.0f, 1.0f } },
  { { +0.5f, +0.5f }, { 1.0f, 1.0f } } };
private:
  struct ConstantBufferData
  {
    vdl::Matrix ViewProjection;
    vdl::Matrix BoneTransforms[Constants::kMaxBoneNum];
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
private:
  VertexBuffer TextureVertexBuffer_;
  InstanceBuffer NoneInstanceBuffer_;
  InstanceBuffer TextureInstanceBuffer_;
  InstanceBuffer MeshInstanceBuffer_;
private:
  std::unique_ptr<CameraData> pCameraData_;
private:
  vdl::OutputManager OutputManager_;
  RendererCommandList<Empty, vdl::uint> EmptyRendererCommandList_;
  RendererCommandList<vdl::Texture, Instance2D> TextureRendererCommandList_;
  RendererCommandList<vdl::Mesh, Instance3D> MeshRendererCommandList_;
public:
  CRenderer() = default;

  void Initialize()override;

  vdl::Matrix GetView()const override;

  vdl::Matrix GetProjection()const override;

  void SetCamera(const vdl::Camera& _Camera)override;

  void SetRenderTextures(const vdl::RenderTextures& _RenderTextures, const vdl::DepthStencilTexture& _DepthStencilTexture)override;

  void SetTopology(vdl::TopologyType _Topology, vdl::InputLayoutType _Type)override;

  void SetScissor(const vdl::Scissor& _Scissor, vdl::InputLayoutType _Type)override;

  void SetViewport(const vdl::Viewport& _Viewport, vdl::InputLayoutType _Type)override;

  void SetBlendState(const vdl::BlendState& _BlendState, vdl::InputLayoutType _Type)override;

  void SetDepthStencilState(const vdl::DepthStencilState& _DepthStencilState, vdl::InputLayoutType _Type)override;

  void SetRasterizerState(const vdl::RasterizerState& _RasterizerState, vdl::InputLayoutType _Type)override;

  void SetVertexShader(const vdl::VertexShader& _VertexShader, vdl::InputLayoutType _Type)override;

  void SetHullShader(const vdl::HullShader& _HullShader, vdl::InputLayoutType _Type)override;

  void SetDomainShader(const vdl::DomainShader& _DomainShader, vdl::InputLayoutType _Type)override;

  void SetGeometryShader(const vdl::GeometryShader& _GeometryShader, vdl::InputLayoutType _Type)override;

  void SetPixelShader(const vdl::PixelShader& _PixelShader, vdl::InputLayoutType _Type)override;

  void SetShaderResources(vdl::uint _StartSlot, vdl::uint _ShaderResourceNum, const vdl::ShaderResource _ShaderResources[], ShaderType _Stage, vdl::InputLayoutType _Type)override;

  void SetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[], ShaderType _Stage, vdl::InputLayoutType _Type)override;

  void SetConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[], ShaderType _Stage, vdl::InputLayoutType _Type)override;

  void Draw(vdl::uint _VertexCount)override;

  void Draw(const vdl::Texture& _Texture, const vdl::float2& _DestLeftTop, const vdl::float2& _DestSize, const vdl::float2& _SrcLeftPos, const vdl::float2& _SrcSize, const vdl::Radian& _Angle, const vdl::ColorF& _Color)override;

  void Draw(const vdl::Mesh& _Mesh, const vdl::Matrix& _World, const vdl::MotionBlendDatas& _MotionBlendDatas, const vdl::ColorF& _Color)override;

  void Clear(const vdl::RenderTexture& _RenderTexture, const vdl::ColorF& _ClearColor)override;

  void Clear(const vdl::DepthStencilTexture& _DepthStencilTexture, float _ClearDepth, vdl::uint _ClearStencil)override;

  void Clear(const vdl::UnorderedAccessTexture& _UnorderedAccessTexture, const vdl::ColorF& _ClearColor)override;

  void Flush()override;
};
