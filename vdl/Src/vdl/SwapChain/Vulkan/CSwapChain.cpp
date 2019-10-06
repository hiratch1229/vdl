#include "CSwapChain.hpp"

#include <vdl/Engine.hpp>
#include <vdl/Device/Vulkan/CDevice.hpp>
#include <vdl/DeviceContext/Vulkan/CDeviceContext.hpp>
#include <vdl/Window/IWindow.hpp>

#include <vdl/Constants/Constants.hpp>

#include <vdl/Format/Vulkan/Format.hpp>

void CSwapChain::Initialize()
{
  pDeviceContext_ = static_cast<CDeviceContext*>(Engine::Get<IDeviceContext>());
  pWindow_ = Engine::Get<IWindow>();

  CDevice* pDevice = static_cast<CDevice*>(Engine::Get<IDevice>());
  Instance_ = pDevice->GetInstance();
  VkDevice_ = pDevice->GetDevice();

  const HWND hWnd = static_cast<HWND>(pWindow_->GetHandle());
  const vk::PhysicalDevice& PhysicalDevice = pDevice->GetPhysicalDevice();

  constexpr vk::Format kSwapChainFormat = Cast(vdl::FormatType::eSwapChain);
  constexpr vk::Format kDepthStencilFormat = Cast(vdl::FormatType::eDepthStencil);

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

      Surface_ = Instance_.createWin32SurfaceKHRUnique(SurfaceInfo);
      assert(Surface_);
    }

    vk::ColorSpaceKHR ColorSpace;
    //  サポートされているフォーマットの取得
    {
      std::vector<vk::SurfaceFormatKHR> SurfaceFormats = PhysicalDevice.getSurfaceFormatsKHR(Surface_.get());
      assert(!SurfaceFormats.empty());
     
      Format = (SurfaceFormats[0].format == vk::Format::eUndefined) ? vk::Format::eB8G8R8A8Unorm : SurfaceFormats[0].format;
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

    PresentInfo_.swapchainCount = 1;
    PresentInfo_.pSwapchains = &VkSwapChain_.get();
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

    RenderTextures_.resize(SwapChainImageNum);
    for (vdl::uint i = 0; i < SwapChainImageNum; ++i)
    {
      CRenderTexture& RenderTarget = RenderTextures_[i];
      RenderTarget.Image = vk::UniqueImage(Images[i]);
      ImageViewInfo.image = RenderTarget.Image.get();
      RenderTarget.View = VkDevice_.createImageViewUnique(ImageViewInfo);

      RenderTarget.SetImageLayout(CommandBuffer, vk::ImageLayout::eColorAttachmentOptimal, SubresourceRange);
    }
  }

  //  深度ステンシルバッファの作成
  {
    vk::ImageTiling ImageTiling;
    {
      vk::FormatProperties FormatProperties = PhysicalDevice.getFormatProperties(kDepthStencilFormat);

      if (FormatProperties.linearTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment)
      {
        ImageTiling = vk::ImageTiling::eLinear;
      }
      else if (FormatProperties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment)
      {
        ImageTiling = vk::ImageTiling::eOptimal;
      }
    }

    //  イメージの作成
    {
      vk::ImageCreateInfo ImageInfo;
      {
        ImageInfo.imageType = vk::ImageType::e2D;
        ImageInfo.format = kDepthStencilFormat;
        ImageInfo.extent = vk::Extent3D(Constants::kDefaultWindowSize.x, Constants::kDefaultWindowSize.y, 1);
        ImageInfo.mipLevels = 1;
        ImageInfo.arrayLayers = 1;
        ImageInfo.samples = vk::SampleCountFlagBits::e1;
        ImageInfo.tiling = ImageTiling;
        ImageInfo.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eTransferDst;
        ImageInfo.sharingMode = vk::SharingMode::eExclusive;
        ImageInfo.queueFamilyIndexCount = 0;
        ImageInfo.pQueueFamilyIndices = nullptr;
        ImageInfo.initialLayout = vk::ImageLayout::eUndefined;
      }

      DepthStencilTexture_.Image = VkDevice_.createImageUnique(ImageInfo);
      assert(DepthStencilTexture_.Image);
    }

    //  メモリの確保
    {
      vk::MemoryRequirements MemoryRequirement = VkDevice_.getImageMemoryRequirements(DepthStencilTexture_.Image.get());

      vk::MemoryAllocateInfo MemoryAllocateInfo;
      {
        MemoryAllocateInfo.allocationSize = MemoryRequirement.size;
        MemoryAllocateInfo.memoryTypeIndex = pDevice->GetMemoryTypeIndex(MemoryRequirement.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal);
      }

      DepthStencilTexture_.Memory = VkDevice_.allocateMemoryUnique(MemoryAllocateInfo);
      assert(DepthStencilTexture_.Memory);

      VkDevice_.bindImageMemory(DepthStencilTexture_.Image.get(), DepthStencilTexture_.Memory.get(), 0);
    }

    vk::ImageSubresourceRange SubresourceRange;
    {
      SubresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil;
      SubresourceRange.baseMipLevel = 0;
      SubresourceRange.levelCount = 1;
      SubresourceRange.baseArrayLayer = 0;
      SubresourceRange.layerCount = 1;
    }

    //  ビューの作成
    {
      vk::ImageViewCreateInfo ImageViewCreateInfo;
      {
        ImageViewCreateInfo.image = DepthStencilTexture_.Image.get();
        ImageViewCreateInfo.viewType = vk::ImageViewType::e2D;
        ImageViewCreateInfo.format = kDepthStencilFormat;
        ImageViewCreateInfo.components.r = vk::ComponentSwizzle::eR;
        ImageViewCreateInfo.components.g = vk::ComponentSwizzle::eG;
        ImageViewCreateInfo.components.b = vk::ComponentSwizzle::eB;
        ImageViewCreateInfo.components.a = vk::ComponentSwizzle::eA;
        ImageViewCreateInfo.subresourceRange = SubresourceRange;
      }

      DepthStencilTexture_.View = VkDevice_.createImageViewUnique(ImageViewCreateInfo);
      assert(DepthStencilTexture_.View);
    }

    //  テクスチャ
    {
      DepthStencilTexture_.SetImageLayout(CommandBuffer, vk::ImageLayout::eDepthStencilAttachmentOptimal, SubresourceRange);
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

  //  コマンドプールの作成
  {
    vk::CommandPoolCreateInfo CommandPoolInfo;
    {
      CommandPoolInfo.queueFamilyIndex = pDevice->GetGrapchisQueueIndex();
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
    vk::SemaphoreCreateInfo SemaphoreInfo;

    PresentSemaphore_ = VkDevice_.createSemaphoreUnique(SemaphoreInfo);
    assert(PresentSemaphore_);

    RenderSemaphore_ = VkDevice_.createSemaphoreUnique(SemaphoreInfo);
    assert(RenderSemaphore_);
  }
}

void CSwapChain::ScreenClear()
{

}

void CSwapChain::Present()
{

}

void CSwapChain::ChangeWindowMode()
{
  //  TODO
}

void CSwapChain::ScreenShot()
{
  //  TODO
}
