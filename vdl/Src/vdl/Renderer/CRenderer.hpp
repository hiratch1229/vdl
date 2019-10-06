#pragma once
#include "IRenderer.hpp"
#include "RendererCommand/RendererCommand.hpp"

#include <vdl/Camera.hpp>
#include <vdl/ConstantBuffer.hpp>

#include <vdl/Buffer/IBuffer.hpp>

#include <memory>

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
  RendererCommandList<Empty, vdl::uint> EmptyRendererCommandList_;
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

  void SetTopology(vdl::TopologyType _Topology, RenderType _Type)override;

  void SetScissor(const vdl::Scissor& _Scissor, RenderType _Type)override;

  void SetViewport(const vdl::Viewport& _Viewport, RenderType _Type)override;

  void SetBlendState(const vdl::BlendState& _BlendState, RenderType _Type)override;

  void SetDepthStencilState(const vdl::DepthStencilState& _DepthStencilState, RenderType _Type)override;

  void SetRasterizerState(const vdl::RasterizerState& _RasterizerState, RenderType _Type)override;

  void SetVertexShader(const vdl::VertexShader& _VertexShader, RenderType _Type)override;

  void SetHullShader(const vdl::HullShader& _HullShader, RenderType _Type)override;

  void SetDomainShader(const vdl::DomainShader& _DomainShader, RenderType _Type)override;

  void SetGeometryShader(const vdl::GeometryShader& _GeometryShader, RenderType _Type)override;

  void SetPixelShader(const vdl::PixelShader& _PixelShader, RenderType _Type)override;

  void SetShaderResources(vdl::uint _StartSlot, vdl::uint _ShaderResourceNum, const vdl::ShaderResource _ShaderResources[], ShaderType _Stage, RenderType _Type)override;

  void SetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[], ShaderType _Stage, RenderType _Type)override;

  void SetConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[], ShaderType _Stage, RenderType _Type)override;

  void Draw(vdl::uint _VertexCount)override;

  void Draw(const vdl::Texture& _Texture, const vdl::float2& _DestLeftTop, const vdl::float2& _DestSize, const vdl::float2& _SrcLeftPos, const vdl::float2& _SrcSize, const vdl::Radian& _Angle, const vdl::ColorF& _Color)override;

  void Draw(const vdl::StaticMesh& _StaticMesh, const vdl::Matrix& _World, const vdl::ColorF& _Color)override;

  void Draw(const vdl::SkinnedMesh& _SkinnedMesh, const vdl::Matrix& _World, const vdl::MotionBlendDatas& _MotionBlendDatas, const vdl::ColorF& _Color)override;

  void Clear(const vdl::RenderTexture& _RenderTexture, const vdl::ColorF& _ClearColor)override;

  void Clear(const vdl::DepthStencilTexture& _DepthStencilTexture, float _ClearDepth, vdl::uint _ClearStencil)override;
  
  void Clear(const vdl::UnorderedAccessTexture& _UnorderedAccessTexture, const vdl::ColorF& _ClearColor)override;

  void Flush()override;
};
