#pragma once
#include "../ISwapChain.hpp"

#include <vdl/Texture.hpp>

#include <vdl/Texture/Vulkan/CTexture.hpp>

#include <vdl/pch/Vulkan/pch.hpp>

class IWindow;
class CDeviceContext;
class IRenderer;

class CSwapChain : public ISwapChain
{
  IWindow* pWindow_;
  CDeviceContext* pDeviceContext_;
  IRenderer* pRenderer_;
private:
  vk::Device VkDevice_;
  vk::Queue GraphicsQueue_;
private:
  vk::UniqueSurfaceKHR Surface_;
  vk::UniqueSwapchainKHR VkSwapChain_;
  std::vector<CRenderTexture> VkRenderTextures_;
  vk::UniqueCommandPool CommandPool_;
  std::vector<vk::UniqueCommandBuffer> CommandBuffers_;
  vk::UniqueSemaphore Semaphore_;
  vdl::uint CurrentBufferIndex_ = 0;
  bool isAfterPresent_;
private:
  vdl::RenderTextures RenderTextures_;
  vdl::DepthStencilTexture DepthStencilTexture_;
public:
  [[nodiscard]] const vk::SwapchainKHR& GetSwapChain()const { return VkSwapChain_.get(); }
  [[nodiscard]] const vdl::uint& GetCurrentBufferIndex()const { return CurrentBufferIndex_; }
  [[nodiscard]] vk::Semaphore GetSemaphore()
  {
    if (isAfterPresent_)
    {
      isAfterPresent_ = false;
      return Semaphore_.get();
    }

    return vk::Semaphore();
  }
public:
  [[nodiscard]] CRenderTexture* GetVkRenderTexture() { return &VkRenderTextures_[CurrentBufferIndex_]; }
public:
  CSwapChain() = default;

  ~CSwapChain();

  PlatformFlags GetPlatform()const final { return PlatformFlag::eVulkan; }

  void Initialize()override;

  void ScreenClear()override;

  void Present()override;

  void ChangeWindowMode()override;

  void ScreenShot()override;

  const vdl::RenderTexture& GetRenderTexture()const override { return RenderTextures_[0]; }

  const vdl::DepthStencilTexture& GetDepthStencilTexture()const override { return DepthStencilTexture_; }
};
