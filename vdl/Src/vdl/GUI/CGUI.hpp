#pragma once
#include "IGUI.hpp"

#include <vdl/Scissor.hpp>
#include <vdl/Viewport.hpp>
#include <vdl/GraphicsStates.hpp>
#include <vdl/Shader.hpp>
#include <vdl/Texture.hpp>
#include <vdl/Sampler.hpp>
#include <vdl/ConstantBuffer.hpp>
#include <vdl/UnorderedAccessBuffer.hpp>

#include <vdl/Buffer/Buffer.hpp>

#include <vdl/pch/Windows/pch.hpp>

#include <memory>

class ISystem;
class IDevice;
class IDeviceContext;
class IWindow;
class IKeyboard;
class IMouse;
class IBufferManager;

class CGUI : public IGUI
{
  struct ConstantBufferData
  {
    vdl::float2 Scale;
    vdl::float2 Translate;
  };
private:
  ISystem* pSystem_;
  IDevice* pDevice_;
  IDeviceContext* pDeviceContext_;
  IWindow* pWindow_;
  IKeyboard* pKeyboard_;
  IMouse* pMouse_;
  IBufferManager* pBufferManager_;
  HWND hWnd_;
private:
  vdl::VertexShader VertexShader_;
  vdl::HullShader HullShader_;
  vdl::DomainShader DomainShader_;
  vdl::GeometryShader GeometryShader_;
  vdl::PixelShader PixelShader_;
  vdl::GraphicsStates GraphicsStates_;
  vdl::RenderTextures RenderTextures_;
  vdl::DepthStencilTexture DepthStencilTexture_;
  vdl::ShaderResource Font_;
  vdl::Sampler Sampler_;
  std::unique_ptr<vdl::ConstantBuffer<ConstantBufferData>> pConstantBuffer_;
  vdl::Scissor Scissor_;
  vdl::Viewport Viewport_;
private:
  VertexBuffer VertexBuffer_;
  vdl::uint VertexBufferSize_ = 0;
  IndexBuffer IndexBuffer_;
  vdl::uint IndexBufferSize_ = 0;
  InstanceBuffer InstanceBuffer_;
public:
  CGUI() = default;

  ~CGUI();

  void Initialize()override;
  
  void Update()override;

  void Draw()override;
};
