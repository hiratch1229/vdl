#pragma once
#include "../ISwapChain.hpp"

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.hpp>

#include <vdl/Texture/Vulkan/CTexture.hpp>

class CDeviceContext;
class IWindow;

class CSwapChain : public ISwapChain
{
  vk::Instance Instance_;
  vk::Device VkDevice_;
  vk::Queue GraphicsQueue_;
  vk::PresentInfoKHR PresentInfo_;
private:
  CDeviceContext* pDeviceContext_;
  IWindow* pWindow_;
private:
  vk::UniqueSwapchainKHR VkSwapChain_;
  vk::UniqueSurfaceKHR Surface_;
  std::vector<CRenderTexture> RenderTextures_;
  CDepthStencilTexture DepthStencilTexture_;
  vk::UniqueCommandPool CommandPool_;
  std::vector<vk::UniqueCommandBuffer> CommandBuffers_;
  vk::UniqueSemaphore PresentSemaphore_;
  vk::UniqueSemaphore RenderSemaphore_;
public:
  CSwapChain() = default;

  void Initialize()override;

  void ScreenClear()override;

  void Present()override;

  void ChangeWindowMode()override;

  void ScreenShot()override;
};
