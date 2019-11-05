#include "CSwapChain.hpp"

#include <vdl/Engine.hpp>
#include <vdl/Window/IWindow.hpp>
#include <vdl/Device/Vulkan/CDevice.hpp>
#include <vdl/DeviceContext/Vulkan/CDeviceContext.hpp>
#include <vdl/TextureManager/ITextureManager.hpp>

#include <vdl/Format/Vulkan/Format.hpp>

#include <vdl/Format/Format.hpp>
#include <vdl/Constants/Constants.hpp>

#include <vdl/DetectMemoryLeak.hpp>

void CSwapChain::Initialize()
{
  pDeviceContext_ = static_cast<CDeviceContext*>(Engine::Get<IDeviceContext>());
  pWindow_ = Engine::Get<IWindow>();

  CDevice* pDevice = static_cast<CDevice*>(Engine::Get<IDevice>());
  VkDevice_ = pDevice->GetDevice();

  const HWND hWnd = static_cast<HWND>(pWindow_->GetHandle());
  const vk::PhysicalDevice& PhysicalDevice = pDevice->GetPhysicalDevice();

  constexpr vk::Format kSwapChainFormat = Cast(vdl::FormatType::eSwapChain);

  vk::Format Format;
  vdl::uint SwapChainImageNum = Constants::kBackBufferNum;

  //  スワップチェーンの作成
  {
    //  サーフェスの作成
    {
      vk::Win32SurfaceCreateInfoKHR SurfaceInfo;
      {
        SurfaceInfo.hinstance = ::GetModuleHandle(nullptr);
        SurfaceInfo.hwnd = hWnd;
      }

      Surface_ = pDevice->GetInstance().createWin32SurfaceKHRUnique(SurfaceInfo);
      assert(Surface_);
    }

    vk::ColorSpaceKHR ColorSpace;
    //  サポートされているフォーマットの取得
    {
      std::vector<vk::SurfaceFormatKHR> SurfaceFormats = PhysicalDevice.getSurfaceFormatsKHR(Surface_.get());
      assert(!SurfaceFormats.empty());

      Format = (SurfaceFormats[0].format == vk::Format::eUndefined ? vk::Format::eB8G8R8A8Unorm : SurfaceFormats[0].format);
      for (auto& SurfaceFormat : SurfaceFormats)
      {
        if (SurfaceFormat.format == kSwapChainFormat)
        {
          Format = kSwapChainFormat;
        }
      }

      ColorSpace = SurfaceFormats[0].colorSpace;
    }

    vk::SurfaceCapabilitiesKHR SurfaceCapabilities = PhysicalDevice.getSurfaceCapabilitiesKHR(Surface_.get());

    const vdl::uint GraphicsQueueIndex = pDevice->FindQueue(vk::QueueFlagBits::eGraphics, Surface_.get());

    vk::PresentModeKHR SwapChainPresentMode = vk::PresentModeKHR::eFifo;
    //  Presentモードを選択
    {
      std::vector<vk::PresentModeKHR> PresentModes = PhysicalDevice.getSurfacePresentModesKHR(Surface_.get());
      for (auto& PresentMode : PresentModes)
      {
        if (PresentMode == vk::PresentModeKHR::eMailbox)
        {
          SwapChainPresentMode = vk::PresentModeKHR::eMailbox;
          break;
        }
        else if (PresentMode == vk::PresentModeKHR::eImmediate)
        {
          SwapChainPresentMode = vk::PresentModeKHR::eImmediate;
        }
      }
    }

    if (SwapChainImageNum > SurfaceCapabilities.maxImageCount)
    {
      SwapChainImageNum = SurfaceCapabilities.maxImageCount;
    }

    vk::SurfaceTransformFlagBitsKHR PreTransform = SurfaceCapabilities.currentTransform;

    vk::SwapchainCreateInfoKHR SwapChainInfo;
    {
      SwapChainInfo.surface = Surface_.get();
      SwapChainInfo.minImageCount = SwapChainImageNum;
      SwapChainInfo.imageFormat = Format;
      SwapChainInfo.imageColorSpace = ColorSpace;
      SwapChainInfo.imageExtent = vk::Extent2D(Constants::kDefaultWindowSize.x, Constants::kDefaultWindowSize.y);
      SwapChainInfo.imageArrayLayers = 1;
      SwapChainInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst;
      SwapChainInfo.imageSharingMode = vk::SharingMode::eExclusive;
      SwapChainInfo.queueFamilyIndexCount = 0;
      SwapChainInfo.pQueueFamilyIndices = nullptr;
      SwapChainInfo.preTransform = PreTransform;
      SwapChainInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
      SwapChainInfo.presentMode = SwapChainPresentMode;
      SwapChainInfo.clipped = true;
      SwapChainInfo.oldSwapchain = vk::SwapchainKHR();
    }

    VkSwapChain_ = VkDevice_.createSwapchainKHRUnique(SwapChainInfo);
    assert(VkSwapChain_);
  }

  const vk::CommandBuffer& CommandBuffer = pDevice->GetCommandBuffer();
  CommandBuffer.begin(vk::CommandBufferBeginInfo());

  //  バックバッファの作成
  {
    std::vector<vk::Image> Images = VkDevice_.getSwapchainImagesKHR(VkSwapChain_.get());

    vk::ImageViewCreateInfo ImageViewInfo;
    {
      ImageViewInfo.viewType = vk::ImageViewType::e2D;
      ImageViewInfo.format = Format;
      ImageViewInfo.components.r = vk::ComponentSwizzle::eR;
      ImageViewInfo.components.g = vk::ComponentSwizzle::eG;
      ImageViewInfo.components.b = vk::ComponentSwizzle::eB;
      ImageViewInfo.components.a = vk::ComponentSwizzle::eA;
      ImageViewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
      ImageViewInfo.subresourceRange.baseArrayLayer = 0;
      ImageViewInfo.subresourceRange.baseMipLevel = 0;
      ImageViewInfo.subresourceRange.layerCount = 1;
      ImageViewInfo.subresourceRange.levelCount = 1;
    }

    const vk::ImageSubresourceRange SubresourceRange = { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 };

    VkRenderTextures_.resize(SwapChainImageNum);
    for (vdl::uint i = 0; i < SwapChainImageNum; ++i)
    {
      CRenderTexture& VkRenderTexture = VkRenderTextures_[i];
      VkRenderTexture.Image = vk::UniqueImage(Images[i]);
      ImageViewInfo.image = VkRenderTexture.Image.get();
      VkRenderTexture.View = VkDevice_.createImageViewUnique(ImageViewInfo);
      VkRenderTexture.Format = vdl::FormatType::eSwapChain;
      VkRenderTexture.VkFormat = Format;
      VkRenderTexture.TextureSize = Constants::kDefaultWindowSize;

      VkRenderTexture.SetImageLayout(CommandBuffer, vk::ImageLayout::eColorAttachmentOptimal, SubresourceRange);
    }
  }

  CommandBuffer.end();

  //  Submit
  {
    vk::SubmitInfo SubmitInfo;
    {
      SubmitInfo.commandBufferCount = 1;
      SubmitInfo.pCommandBuffers = &CommandBuffer;
    }
    pDevice->SubmitAndWait(SubmitInfo);
  }

  //  レンダーテクスチャの作成
  {
    RenderTextures_[0] = vdl::RenderTexture(Constants::kDefaultWindowSize, vdl::FormatType::eSwapChain);

    CSwapChainRenderTexture* pRenderTexture = new CSwapChainRenderTexture;

    Engine::Get<ITextureManager>()->SetTexture(RenderTextures_[0].GetID(), pRenderTexture);
  }

  //  深度ステンシルテクスチャの作成
  DepthStencilTexture_ = vdl::DepthStencilTexture(Constants::kDefaultWindowSize, vdl::FormatType::eDefaultDepthStencil);

  //  コマンドプールの作成
  {
    vk::CommandPoolCreateInfo CommandPoolInfo;
    {
      CommandPoolInfo.queueFamilyIndex = pDevice->GetGraphicsQueueIndex();
      CommandPoolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    }

    CommandPool_ = VkDevice_.createCommandPoolUnique(CommandPoolInfo);
    assert(CommandPool_);
  }

  //  コマンドバッファの作成
  {
    vk::CommandBufferAllocateInfo CommandBufferAllocateInfo;
    {
      CommandBufferAllocateInfo.commandPool = CommandPool_.get();
      CommandBufferAllocateInfo.level = vk::CommandBufferLevel::ePrimary;
      CommandBufferAllocateInfo.commandBufferCount = SwapChainImageNum;
    }

    CommandBuffers_ = VkDevice_.allocateCommandBuffersUnique(CommandBufferAllocateInfo);
  }

  //  セマフォの作成
  {
    Semaphore_ = VkDevice_.createSemaphoreUnique(vk::SemaphoreCreateInfo());
    assert(Semaphore_);
  }
}

CSwapChain::~CSwapChain()
{
  for (auto& VkRenderTexture : VkRenderTextures_)
  {
    VkRenderTexture.Image.release();
    VkRenderTexture.View.reset();
  };
}

void CSwapChain::ScreenClear()
{
  vk::ResultValue Result = VkDevice_.acquireNextImageKHR(VkSwapChain_.get(), UINT64_MAX, Semaphore_.get(), vk::Fence());
  assert(Result.result == vk::Result::eSuccess);
  CurrentBufferIndex_ = Result.value;
  isAfterPresent_ = true;

  pDeviceContext_->ClearRenderTexture(RenderTextures_[0], pWindow_->GetScreenClearColor());
  pDeviceContext_->ClearDepthStencilTexture(DepthStencilTexture_, Constants::kDefaultClearDepth, Constants::kDefaultClearStencil);
  pDeviceContext_->SetRenderTextures(RenderTextures_, DepthStencilTexture_);
}

void CSwapChain::Present()
{
  pDeviceContext_->Present();
}

void CSwapChain::ChangeWindowMode()
{
  //  TODO
}

void CSwapChain::ScreenShot()
{
  //  TODO
}
