#pragma once
#include "../IGUI.hpp"

#include <vdl/GraphicsState.hpp>
#include <vdl/SamplerState.hpp>
#include <vdl/Shader.hpp>
#include <vdl/Texture.hpp>
#include <vdl/Scissor.hpp>
#include <vdl/Viewport.hpp>

#include <vdl/Buffer/IBuffer.hpp>

#include <Windows.h>
#include <memory>

class ISystem;
class IDevice;
class IDeviceContext;
class IKeyboard;
class IMouse;

class CGUI : public IGUI
{
  ISystem* pSystem_;
  IDevice* pDevice_;
  IDeviceContext* pDeviceContext_;
  IKeyboard* pKeyboard_;
  IMouse* pMouse_;
  HWND hWnd_;
private:
  vdl::VertexShader VertexShader_;
  vdl::PixelShader PixelShader_;
  vdl::Texture Font_;
  vdl::GraphicsState GraphicsState_;
  vdl::SamplerState SamplerState_;
  vdl::Scissor Scissor_;
  vdl::Viewport Viewport_;
private:
  std::unique_ptr<IBuffer> pVertexBuffers_[2];
  vdl::uint VertexBufferSize_ = 0;
  std::unique_ptr<IBuffer> pIndexBuffers_[2];
  vdl::uint IndexBufferSize_ = 0;
public:
  CGUI() = default;

  ~CGUI();

  void Initialize()override;
  
  void Update()override;

  void Draw()override;
};
