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
  vk::UniqueSurfaceKHR Surface_;
  vk::UniqueSwapchainKHR VkSwapChain_;
  std::vector<CRenderTexture> VkRenderTextures_;
  CDepthStencilTexture VkDepthStencilTexture_;
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
  [[nodiscard]] CRenderTexture* GetRenderTexture() { return &VkRenderTextures_[CurrentBufferIndex_]; }
  [[nodiscard]] CDepthStencilTexture* GetDepthStencilTexture() { return &VkDepthStencilTexture_; }
public:
  CSwapChain() = default;

  ~CSwapChain();

  void Initialize()override;

  void ScreenClear()override;

  void Present()override;

  void ChangeWindowMode()override;

  void ScreenShot()override;
};
