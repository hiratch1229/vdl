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
#include <vdl/Variant.hpp>

#include <vdl/Buffer/Buffer.hpp>
#include <vdl/CommandList/GraphicsCommandList/GraphicsCommandList.hpp>

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
private:
  vdl::RenderTextures RenderTextures_;
  vdl::DepthStencilTexture DepthStencilTexture_;
  GraphicsCommandList<vdl::Texture, std::nullptr_t> GraphicsCommandList_;
  std::unique_ptr<vdl::ConstantBuffer<ConstantBufferData>> pConstantBuffer_;
private:
  VertexBuffer VertexBuffer_;
  vdl::uint VertexBufferSize_ = 0;
  IndexBuffer IndexBuffer_;
  vdl::uint IndexBufferSize_ = 0;
public:
  CGUI() = default;

  ~CGUI();

  PlatformFlags GetPlatform()const final { return PlatformFlag::eCommon; }

  void Initialize()override;
  
  void Update()override;

  void Draw()override;
};
