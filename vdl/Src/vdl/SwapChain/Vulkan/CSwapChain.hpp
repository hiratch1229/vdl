#pragma once
#include "../ISwapChain.hpp"

#include <vdl/Texture.hpp>

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.hpp>

#include <vdl/Texture/Vulkan/CTexture.hpp>

class CDeviceContext;
class IWindow;

class CSwapChain : public ISwapChain
{
  vk::Device VkDevice_;
  vk::Queue GraphicsQueue_;
private:
  CDeviceContext* pDeviceContext_;
  IWindow* pWindow_;
private:
  vk::UniqueSwapchainKHR VkSwapChain_;
  vk::UniqueSurfaceKHR Surface_;
  std::vector<CRenderTexture> VkRenderTextures_;
  CDepthStencilTexture VkDepthStencilTexture_;
  vk::UniqueCommandPool CommandPool_;
  std::vector<vk::UniqueCommandBuffer> CommandBuffers_;
  vk::UniqueSemaphore PresentSemaphore_;
  vk::UniqueSemaphore RenderSemaphore_;
  vk::PresentInfoKHR PresentInfo_;
  vdl::uint NextBufferNumber_ = 0;
  bool isFirstAfterPresent_;
private:
  vdl::RenderTextures RenderTextures_;
  vdl::DepthStencilTexture DepthStencilTexture_;
public:
  [[nodiscard]] CRenderTexture* GetRenderTexture() { return &VkRenderTextures_[NextBufferNumber_]; }
  [[nodiscard]] CDepthStencilTexture* GetDepthStencilTexture() { return &VkDepthStencilTexture_; }
public:
  CSwapChain() = default;

  void Initialize()override;

  void ScreenClear()override;

  void Present()override;

  void ChangeWindowMode()override;

  void ScreenShot()override;
};
