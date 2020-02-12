#pragma once
#include <vdl/Fwd.hpp>
#include <vdl/Texture.hpp>

#include <vdl/pch/Vulkan/pch.hpp>
#include <vdl/Texture/Vulkan/CTexture.hpp>

class CDevice;

class SwapChain 
{
  vk::UniqueSurfaceKHR Surface_;
  vk::UniqueSwapchainKHR VkSwapChain_;
  std::vector<CRenderTexture> VkRenderTextures_;
  vdl::RenderTextures RenderTextures_;
  vdl::DepthStencilTexture DepthStencilTexture_;
  vk::UniqueCommandPool CommandPool_;
  std::vector<vk::UniqueCommandBuffer> CommandBuffers_;
  vk::UniqueSemaphore Semaphore_;
  vdl::uint CurrentBufferIndex_ = 0;
  bool isAfterPresent_;
public:
  [[nodiscard]] const vk::SwapchainKHR& GetSwapChain()const { return VkSwapChain_.get(); }

  [[nodiscard]] const vdl::uint& GetCurrentBufferIndex()const { return CurrentBufferIndex_; }

  [[nodiscard]] CRenderTexture* GetVkRenderTexture() { return &VkRenderTextures_[CurrentBufferIndex_]; }

  [[nodiscard]] const vdl::RenderTexture& GetRenderTexture()const { return RenderTextures_[0]; }

  [[nodiscard]] const vdl::DepthStencilTexture& GetDepthStencilTexture()const { return DepthStencilTexture_; }

  [[nodiscard]] const vk::SwapchainKHR* operator->()const { return &VkSwapChain_.get(); }

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
  SwapChain() = default;

  ~SwapChain();

  void Initialize(CDevice* _pDevice);

  void AcquireNextImage(const vk::Device& _Device);
};
