#pragma once
#include <vdl/Platform/Platform.hpp>

#include <vdl/Texture.hpp>

class ISwapChain
{
public:
  static ISwapChain* Create();

  ISwapChain() = default;

  virtual ~ISwapChain() = default;
  
  [[nodiscard]] virtual PlatformType GetPlatform()const = 0;

  virtual void Initialize() = 0;

  virtual void ScreenClear() = 0;

  virtual void Present() = 0;

  virtual void ChangeWindowMode() = 0;

  virtual void ScreenShot() = 0;

  [[nodiscard]] virtual const vdl::RenderTexture& GetRenderTexture()const = 0;

  [[nodiscard]] virtual const vdl::DepthStencilTexture& GetDepthStencilTexture()const = 0;
};
