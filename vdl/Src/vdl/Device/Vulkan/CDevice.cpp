#include "CDevice.hpp"

#include <vdl/Format/Vulkan/Format.hpp>
#include <vdl/Texture/Vulkan/CTexture.hpp>
#include <vdl/Shader/Vulkan/CShader.hpp>
#include <vdl/Misc/Windows/Misc.hpp>

#include <vdl/Constants/Constants.hpp>

#include <vdl/Image.hpp>
#include <vdl/Macro.hpp>

#include <glslang/SPIRV/GlslangToSpv.h>
#include <glslang/StandAlone/DirStackFileIncluder.h>
#include <glslang/StandAlone/ResourceLimits.hpp>

#include <sstream>

namespace
{
  inline constexpr EShLanguage Cast(ShaderType _Type)
  {
    switch (_Type)
    {
    case ShaderType::eVertexShader:
      return EShLangVertex;
    case ShaderType::eHullShader:
      return EShLangTessControl;
    case ShaderType::eDomainShader:
      return EShLangTessEvaluation;
    case ShaderType::eGeometryShader:
      return EShLangGeometry;
    case ShaderType::ePixelShader:
      return EShLangFragment;
    case ShaderType::eComputeShader:
      return EShLangCompute;
    default: assert(false);
    }

    return EShLanguage();
  }

  inline void ComplineShader(std::vector<vdl::uint>* _pCode, ShaderType _Type, const char* _Source, vdl::uint _DataSize, const char* _EntryPoint)
  {
    constexpr EShMessages kControls = static_cast<EShMessages>(EShMsgCascadingErrors | EShMsgReadHlsl
      | EShMsgSpvRules | EShMsgKeepUncalled | EShMsgVulkanRules);

    const EShLanguage Stage = Cast(_Type);

    bool Result = true;

    glslang::InitializeProcess();

    //  シェーダーファイルのコンパイル
    glslang::TShader Shader(Stage);
    {
      Shader.setStringsWithLengths(&_Source, reinterpret_cast<int*>(&_DataSize), 1);
      Shader.setEntryPoint(_EntryPoint);
      Shader.setEnvInput(glslang::EShSourceHlsl, Stage, glslang::EShClientVulkan, 110);
      Shader.setEnvClient(glslang::EShClientVulkan, glslang::EShTargetVulkan_1_1);
      Shader.setEnvTarget(glslang::EShTargetSpv, glslang::EShTargetSpv_1_3);

      DirStackFileIncluder Includer;
      Result = Shader.parse(&glslang::DefaultTBuiltInResource, 110, false, kControls, Includer);
      _ASSERT_EXPR_A(Result, "コンパイルに失敗");
    }

    //  シェーダーファイルのリンク
    glslang::TProgram Program;
    {
      Program.addShader(&Shader);

      Result = Program.link(kControls);
      _ASSERT_EXPR_A(Result, "シェーダーファイルのリンクに失敗");
    }

    class IntermTraverser : public glslang::TIntermTraverser
    {
      const ShaderType Type_;
    public:
      IntermTraverser(ShaderType _Type)
        : Type_(_Type) {}
    public:
      void visitSymbol(glslang::TIntermSymbol* _pSymbol)override
      {
        const glslang::TBasicType Type = _pSymbol->getBasicType();

        vdl::uint LayoutSet = 0;
        if (Type == glslang::TBasicType::EbtSampler)
        {
          const glslang::TSampler& Sampler = _pSymbol->getType().getSampler();

          //  Sampler
          if (Sampler.type == glslang::EbtVoid && Sampler.dim == glslang::EsdNone)
          {
            if (Type_ == ShaderType::eComputeShader)
            {
              LayoutSet = kGraphicsShaderStageNum * kGraphicsDescriptorTypeNum + 0;
            }
            else
            {
              LayoutSet = kGraphicsShaderStageNum * static_cast<vdl::uint>(DescriptorType::eSampler) + static_cast<vdl::uint>(Type_);
            }
          }
          //  Texture
          else if (_pSymbol->getQualifier().layoutFormat == glslang::TLayoutFormat::ElfNone)
          {
            if (Type_ == ShaderType::eComputeShader)
            {
              LayoutSet = kGraphicsShaderStageNum * kGraphicsDescriptorTypeNum + static_cast<vdl::uint>(DescriptorType::eShaderResouce);
            }
            else
            {
              LayoutSet = kGraphicsShaderStageNum * static_cast<vdl::uint>(DescriptorType::eShaderResouce) + static_cast<vdl::uint>(Type_);
            }
          }
          //  RWTexture
          else
          {
            assert(Type_ == ShaderType::eComputeShader);
            LayoutSet = kGraphicsShaderStageNum * kGraphicsDescriptorTypeNum + static_cast<vdl::uint>(DescriptorType::eUnorderedAccessObject);
          }
        }
        else if (Type == glslang::TBasicType::EbtBlock)
        {
          const glslang::TQualifier& Qualifier = _pSymbol->getQualifier();

          //  ConstantBuffer
          if (Qualifier.storage == glslang::TStorageQualifier::EvqUniform)
          {
            if (Type_ == ShaderType::eComputeShader)
            {
              LayoutSet = kGraphicsShaderStageNum * kGraphicsDescriptorTypeNum + static_cast<vdl::uint>(DescriptorType::eConstantBuffer);
            }
            else
            {
              LayoutSet = kGraphicsShaderStageNum * static_cast<vdl::uint>(DescriptorType::eConstantBuffer) + static_cast<vdl::uint>(Type_);
            }
          }
          //  RWStructuredBuffer
          else
          {
            if (Type_ == ShaderType::eComputeShader)
            {
              LayoutSet = kGraphicsShaderStageNum * kGraphicsDescriptorTypeNum + static_cast<vdl::uint>(DescriptorType::eUnorderedAccessObject);
            }
            else
            {
              LayoutSet = kGraphicsShaderStageNum * static_cast<vdl::uint>(DescriptorType::eUnorderedAccessObject) + static_cast<vdl::uint>(Type_);
            }
          }
        }
        else
        {
          return;
        }

        _pSymbol->getWritableType().getQualifier().layoutSet = LayoutSet;
      }
    };

    IntermTraverser IntermTraverser(_Type);
    Program.buildReflection();

    glslang::TIntermediate* pIntermediate = Program.getIntermediate(Stage);
    pIntermediate->getTreeRoot()->traverse(&IntermTraverser);

    glslang::GlslangToSpv(*pIntermediate, *_pCode);

    glslang::FinalizeProcess();
  }

  inline void ComplineShader(std::vector<vdl::uint>* _pCode, ShaderType _Type, const char* _FilePath, const char* _EntryPoint)
  {
    //  シェーダーファイルのロード
    std::string Data;
    {
      std::ifstream IStream(_FilePath);
      _ASSERT_EXPR_A(IStream, (std::string(_FilePath) + "が存在しません。").c_str());

      IStream.seekg(0, std::ios::end);
      Data.reserve(static_cast<std::string::size_type>(IStream.tellg()));
      IStream.seekg(0, std::ios::beg);
      Data.assign(std::istreambuf_iterator<char>(IStream), std::istreambuf_iterator<char>());
    }

    ::ComplineShader(_pCode, _Type, Data.data(), static_cast<vdl::uint>(Data.size()), _EntryPoint);
  }

  // デバッグ時のメッセージコールバック
  VkBool32 DebugMessageCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT, uint64_t, size_t, int32_t, const char* pLayerPrefix, const char* pMsg, void*)
  {
    VkBool32 Result = false;

    std::string Message;
    {
      std::stringstream Buf;
      if (flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT)
      {
        Buf << "INFORMATION: ";
        Result = true;
      }
      else if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
      {
        Buf << "WARNING: ";
      }
      else if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
      {
        Buf << "PERFORMANCE_WARNING: ";
      }
      else if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
      {
        Buf << "ERROR: ";
      }
      else if (flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT)
      {
        Buf << "DEBUG: ";
        Result = true;
      }

      Buf << "[" << pLayerPrefix << "] :" << pMsg << std::endl;
      Message = Buf.str();
    }

    // デバッグウィンドウに出力
    OutputDebugStringA(Message.c_str());

    return Result;
  }
}

void CDevice::Initialize()
{
  const char* Layers[] = {
    "VK_LAYER_LUNARG_standard_validation",
    "VK_LAYER_GOOGLE_threading"
  };

  //  インスタンスの作成
  {
    const char* Extensions[] = {
      VK_KHR_SURFACE_EXTENSION_NAME,
      VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#if defined DEBUG | _DEBUG
      VK_EXT_DEBUG_REPORT_EXTENSION_NAME, //  デバッグレポート用
#endif
    };

    vk::ApplicationInfo ApplicationInfo;
    {
      ApplicationInfo.pApplicationName = Constants::kDefaultWindowName;
      ApplicationInfo.apiVersion = 1;
      ApplicationInfo.pEngineName = Constants::kDefaultWindowName;
      ApplicationInfo.engineVersion = 1;
      ApplicationInfo.apiVersion = VK_API_VERSION_1_1;
    }

    vk::InstanceCreateInfo InstanceInfo;
    {
      InstanceInfo.pApplicationInfo = &ApplicationInfo;
      InstanceInfo.enabledExtensionCount = vdl::Macro::ArraySize(Extensions);
      InstanceInfo.ppEnabledExtensionNames = Extensions;
#if defined DEBUG | _DEBUG
      InstanceInfo.enabledLayerCount = vdl::Macro::ArraySize(Layers);
      InstanceInfo.ppEnabledLayerNames = Layers;
#endif
    }

    Instance_ = vk::createInstanceUnique(InstanceInfo);
    assert(Instance_);
  }

  //  物理デバイスの取得
  {
    std::vector<vk::PhysicalDevice> PhysicalDevices = std::move(Instance_->enumeratePhysicalDevices());
    assert(!PhysicalDevices.empty());

    //  TODO:MultiGPU
    PhysicalDevice_ = PhysicalDevices[0];
  }

  GraphicsQueueIndex_ = FindQueue(vk::QueueFlagBits::eGraphics);
  ComputeQueueIndex_ = FindQueue(vk::QueueFlagBits::eCompute);
  CopyQueueIndex_ = FindQueue(vk::QueueFlagBits::eTransfer);

  //  論理デバイスの作成
  {
    const char* Extensions[] = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };

    const float QueuePriorities[] = { 1.0f };

    std::vector<vk::DeviceQueueCreateInfo> DeviceQueueInfos(3);
    {
      DeviceQueueInfos[0].queueFamilyIndex = GraphicsQueueIndex_;
      DeviceQueueInfos[0].queueCount = 1;
      DeviceQueueInfos[0].pQueuePriorities = QueuePriorities;

      DeviceQueueInfos[1].queueFamilyIndex = ComputeQueueIndex_;
      DeviceQueueInfos[1].queueCount = 1;
      DeviceQueueInfos[1].pQueuePriorities = QueuePriorities;

      DeviceQueueInfos[2].queueFamilyIndex = CopyQueueIndex_;
      DeviceQueueInfos[2].queueCount = 1;
      DeviceQueueInfos[2].pQueuePriorities = QueuePriorities;
    }

    vk::PhysicalDeviceFeatures EnabledFeatures = PhysicalDevice_.getFeatures();
    {
      EnabledFeatures.largePoints = false;
      EnabledFeatures.shaderTessellationAndGeometryPointSize = false;
    }

    vk::DeviceCreateInfo DeviceInfo;
    {
      DeviceInfo.queueCreateInfoCount = DeviceQueueInfos.size();
      DeviceInfo.pQueueCreateInfos = DeviceQueueInfos.data();
      DeviceInfo.enabledLayerCount = vdl::Macro::ArraySize(Layers);
      DeviceInfo.ppEnabledLayerNames = Layers;
      DeviceInfo.enabledExtensionCount = vdl::Macro::ArraySize(Extensions);
      DeviceInfo.ppEnabledExtensionNames = Extensions;
      DeviceInfo.pEnabledFeatures = &EnabledFeatures;
    }

    Device_ = PhysicalDevice_.createDeviceUnique(DeviceInfo);
    assert(Device_);
  }

  //  コマンドプールの作成
  {
    vk::CommandPoolCreateInfo CommandPoolInfo;
    {
      CommandPoolInfo.queueFamilyIndex = CopyQueueIndex_;
      CommandPoolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    }

    CommandPool_ = Device_->createCommandPoolUnique(CommandPoolInfo);
    assert(CommandPool_);
  }

  //  コマンドバッファの作成
  {
    vk::CommandBufferAllocateInfo CommandBufferAllocateInfo;
    {
      CommandBufferAllocateInfo.commandPool = CommandPool_.get();
      CommandBufferAllocateInfo.level = vk::CommandBufferLevel::ePrimary;
      CommandBufferAllocateInfo.commandBufferCount = 1;
    }

    CommandBuffer_ = std::move(Device_->allocateCommandBuffersUnique(CommandBufferAllocateInfo).front());
  }

  //  キューの取得
  {
    GraphicsQueue_ = Device_->getQueue(GraphicsQueueIndex_, 0);
    assert(GraphicsQueue_);

    ComputeQueue_ = Device_->getQueue(ComputeQueueIndex_, 0);
    assert(ComputeQueue_);

    CopyQueue_ = Device_->getQueue(CopyQueueIndex_, 0);
    assert(CopyQueue_);
  }

#if defined(DEBUG) | defined(_DEBUG)
  //  デバッグ用コールバック設定
  {
    DestroyReportFunction_ = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(Instance_->getProcAddr("vkDestroyDebugReportCallbackEXT"));

    const vk::DebugReportCallbackCreateInfoEXT DebugReportCallbackInfo = { vk::DebugReportFlagBitsEXT::eError | vk::DebugReportFlagBitsEXT::eWarning | vk::DebugReportFlagBitsEXT::ePerformanceWarning, reinterpret_cast<PFN_vkDebugReportCallbackEXT>(DebugMessageCallback) };

    reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(Instance_->getProcAddr("vkCreateDebugReportCallbackEXT"))
      (static_cast<VkInstance>(Instance_.get()), reinterpret_cast<const VkDebugReportCallbackCreateInfoEXT*>(&DebugReportCallbackInfo), nullptr, reinterpret_cast<VkDebugReportCallbackEXT*>(&DebugReportCallBack_));
    assert(DebugReportCallBack_);
  }
#endif
}

CDevice::~CDevice()
{
#if defined(DEBUG) | defined(_DEBUG)
  DestroyReportFunction_(static_cast<VkInstance>(Instance_.get()), static_cast<VkDebugReportCallbackEXT>(DebugReportCallBack_), nullptr);
#endif
}

void CDevice::CreateVertexBuffer(IBuffer** _ppVertexBuffer, vdl::uint, vdl::uint _BufferSize)
{
  assert(_ppVertexBuffer);

  CVertexBuffer* pVertexBuffer = new CVertexBuffer;
  CreateBuffer(&pVertexBuffer->BufferData, _BufferSize, vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

  (*_ppVertexBuffer) = std::move(pVertexBuffer);
}

void CDevice::CreateVertexBuffer(IBuffer** _ppVertexBuffer, const void* _Vertices, vdl::uint _Stride, vdl::uint _BufferSize)
{
  assert(_ppVertexBuffer);

  CVertexBuffer* pVertexBuffer = new CVertexBuffer;
  CreateBuffer(&pVertexBuffer->BufferData, _BufferSize, vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal);

  BufferData StagingBuffer;
  CreateStagingBuffer(&StagingBuffer, _Vertices, _BufferSize);
  CopyBuffer(StagingBuffer.Buffer.get(), pVertexBuffer->BufferData.Buffer.get(), _BufferSize);

  (*_ppVertexBuffer) = std::move(pVertexBuffer);
}

void CDevice::CreateInstanceBuffer(IBuffer** _ppInstanceBuffer, vdl::uint, vdl::uint _BufferSize)
{
  assert(_ppInstanceBuffer);

  CInstanceBuffer* pInstanceBuffer = new CInstanceBuffer;
  CreateBuffer(&pInstanceBuffer->BufferData, _BufferSize, vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

  (*_ppInstanceBuffer) = std::move(pInstanceBuffer);
}

void CDevice::CreateIndexBuffer(IBuffer** _ppIndexBuffer, vdl::uint _BufferSize, IndexType _IndexType)
{
  assert(_ppIndexBuffer);

  CIndexBuffer* pIndexBuffer = new CIndexBuffer;
  CreateBuffer(&pIndexBuffer->BufferData, _BufferSize, vk::BufferUsageFlagBits::eIndexBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

  (*_ppIndexBuffer) = std::move(pIndexBuffer);
}

void CDevice::CreateIndexBuffer(IBuffer** _ppIndexBuffer, const void* _Indices, vdl::uint _BufferSize, IndexType _IndexType)
{
  assert(_ppIndexBuffer);

  CIndexBuffer* pIndexBuffer = new CIndexBuffer;
  pIndexBuffer->IndexType = _IndexType;
  CreateBuffer(&pIndexBuffer->BufferData, _BufferSize, vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal);

  BufferData StagingBuffer;
  CreateStagingBuffer(&StagingBuffer, _Indices, _BufferSize);
  CopyBuffer(StagingBuffer.Buffer.get(), pIndexBuffer->BufferData.Buffer.get(), _BufferSize);

  (*_ppIndexBuffer) = std::move(pIndexBuffer);
}

void CDevice::CreateConstantBuffer(IBuffer** _ppConstantBuffer, vdl::uint _BufferSize)
{
  assert(_ppConstantBuffer);

  CConstantBuffer* pConstantBuffer = new CConstantBuffer;
  pConstantBuffer->BufferSize = _BufferSize;
  CreateBuffer(&pConstantBuffer->BufferData, _BufferSize, vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

  (*_ppConstantBuffer) = std::move(pConstantBuffer);
}

void CDevice::CreateUnorderedAccessBuffer(IBuffer** _ppUnorderedAccessBuffer, vdl::uint, vdl::uint _BufferSize, const void* _Buffer)
{
  assert(_ppUnorderedAccessBuffer);

  CUnordererdAccessBuffer* pUnorderedAccessBuffer = new CUnordererdAccessBuffer;
  CreateBuffer(&pUnorderedAccessBuffer->BufferData, _BufferSize, vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal);

  if (_Buffer)
  {
    BufferData StagingBuffer;
    CreateStagingBuffer(&StagingBuffer, _Buffer, _BufferSize);
    CopyBuffer(StagingBuffer.Buffer.get(), pUnorderedAccessBuffer->BufferData.Buffer.get(), _BufferSize);
  }

  (*_ppUnorderedAccessBuffer) = std::move(pUnorderedAccessBuffer);
}

void CDevice::CreateTexture(ITexture** _ppTexture, const vdl::Image& _Image)
{
  assert(_ppTexture);

  constexpr vk::Format kTextureFormat = Cast(Constants::kTextureFormat);

  CTexture* pTexture = new CTexture;
  pTexture->TextureSize = _Image.GetSize();

  BufferData StagingBuffer;
  CreateStagingBuffer(&StagingBuffer, _Image.Buffer(), _Image.BufferSize());

  //  イメージ作成
  {
    vk::ImageCreateInfo ImageInfo;
    {
      ImageInfo.imageType = vk::ImageType::e2D;
      ImageInfo.format = kTextureFormat;
      ImageInfo.extent = { pTexture->TextureSize.x, pTexture->TextureSize.y, 1 };
      ImageInfo.mipLevels = 1;
      ImageInfo.arrayLayers = 1;
      ImageInfo.samples = vk::SampleCountFlagBits::e1;
      ImageInfo.tiling = vk::ImageTiling::eOptimal;
      ImageInfo.usage = vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst;
      ImageInfo.sharingMode = vk::SharingMode::eExclusive;
      ImageInfo.initialLayout = vk::ImageLayout::eUndefined;
    }

    pTexture->Image = Device_->createImageUnique(ImageInfo);
    assert(pTexture->Image);
  }

  //  メモリの確保
  {
    vk::MemoryRequirements MemoryRequirement = Device_->getImageMemoryRequirements(pTexture->Image.get());

    vk::MemoryAllocateInfo MemoryAllocateInfo;
    {
      MemoryAllocateInfo.allocationSize = MemoryRequirement.size;
      MemoryAllocateInfo.memoryTypeIndex = GetMemoryTypeIndex(MemoryRequirement.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal);
    }

    pTexture->Memory = Device_->allocateMemoryUnique(MemoryAllocateInfo);
    assert(pTexture->Memory);
    Device_->bindImageMemory(pTexture->Image.get(), pTexture->Memory.get(), 0);
  }

  //  メモリのコピー
  {
    CommandBuffer_->begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));

    vk::BufferImageCopy CopyRegion;
    {
      CopyRegion.bufferOffset = 0;
      CopyRegion.bufferRowLength = 0;
      CopyRegion.bufferImageHeight = 0;
      CopyRegion.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
      CopyRegion.imageSubresource.mipLevel = 0;
      CopyRegion.imageSubresource.baseArrayLayer = 0;
      CopyRegion.imageSubresource.layerCount = 1;
      CopyRegion.imageOffset = { 0, 0, 0 };
      CopyRegion.imageExtent = { pTexture->TextureSize.x, pTexture->TextureSize.y , 1 };
    }

    vk::ImageSubresourceRange SubresourceRange;
    {
      SubresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
      SubresourceRange.baseMipLevel = 0;
      SubresourceRange.levelCount = 1;
      SubresourceRange.baseArrayLayer = 0;
      SubresourceRange.layerCount = 1;
    }

    pTexture->SetImageLayout(CommandBuffer_.get(), vk::ImageLayout::eTransferDstOptimal, SubresourceRange);
    CommandBuffer_->copyBufferToImage(StagingBuffer.Buffer.get(), pTexture->Image.get(), vk::ImageLayout::eTransferDstOptimal, CopyRegion);
    pTexture->SetImageLayout(CommandBuffer_.get(), vk::ImageLayout::eShaderReadOnlyOptimal, SubresourceRange);

    CommandBuffer_->end();

    vk::SubmitInfo SubmitInfo;
    {
      SubmitInfo.commandBufferCount = 1;
      SubmitInfo.pCommandBuffers = &CommandBuffer_.get();
    }
    SubmitAndWait(SubmitInfo, vk::QueueFlagBits::eTransfer);
  }

  //  ビューの作成
  {
    vk::ImageViewCreateInfo ImageViewInfo;
    {
      ImageViewInfo.image = pTexture->Image.get();
      ImageViewInfo.viewType = vk::ImageViewType::e2D;
      ImageViewInfo.format = kTextureFormat;
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

    pTexture->View = Device_->createImageViewUnique(ImageViewInfo);
    assert(pTexture->View);
  }

  (*_ppTexture) = std::move(pTexture);
}

void CDevice::CreateRenderTexture(ITexture** _ppRenderTexture, const vdl::uint2& _TextureSize, vdl::FormatType _Format)
{
  assert(_ppRenderTexture);

  const vk::Format Format = Cast(_Format);

  CRenderTexture* pRenderTexture = new CRenderTexture;
  pRenderTexture->TextureSize = _TextureSize;

  //  イメージ作成
  {
    vk::ImageCreateInfo ImageInfo;
    {
      ImageInfo.imageType = vk::ImageType::e2D;
      ImageInfo.format = Format;
      ImageInfo.extent = { pRenderTexture->TextureSize.x, pRenderTexture->TextureSize.y, 1 };
      ImageInfo.mipLevels = 1;
      ImageInfo.arrayLayers = 1;
      ImageInfo.samples = vk::SampleCountFlagBits::e1;
      ImageInfo.tiling = vk::ImageTiling::eOptimal;
      ImageInfo.usage = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled;
      ImageInfo.sharingMode = vk::SharingMode::eExclusive;
      ImageInfo.initialLayout = vk::ImageLayout::eUndefined;
    }

    pRenderTexture->Image = Device_->createImageUnique(ImageInfo);
    assert(pRenderTexture->Image);
  }

  //  メモリの確保
  {
    vk::MemoryRequirements MemoryRequirement = Device_->getImageMemoryRequirements(pRenderTexture->Image.get());

    vk::MemoryAllocateInfo MemoryAllocateInfo;
    {
      MemoryAllocateInfo.allocationSize = MemoryRequirement.size;
      MemoryAllocateInfo.memoryTypeIndex = GetMemoryTypeIndex(MemoryRequirement.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal);
    }

    pRenderTexture->Memory = Device_->allocateMemoryUnique(MemoryAllocateInfo);
    assert(pRenderTexture->Memory);
    Device_->bindImageMemory(pRenderTexture->Image.get(), pRenderTexture->Memory.get(), 0);
  }

  //  ビューの作成
  {
    vk::ImageViewCreateInfo ImageViewInfo;
    {
      ImageViewInfo.image = pRenderTexture->Image.get();
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

    pRenderTexture->View = Device_->createImageViewUnique(ImageViewInfo);
    assert(pRenderTexture->View);
  }

  //  レイアウトの変更
  {
    vk::ImageSubresourceRange SubresourceRange;
    {
      SubresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
      SubresourceRange.baseMipLevel = 0;
      SubresourceRange.levelCount = 1;
      SubresourceRange.baseArrayLayer = 0;
      SubresourceRange.layerCount = 1;
    }

    CommandBuffer_->begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
    pRenderTexture->SetImageLayout(CommandBuffer_.get(), vk::ImageLayout::eShaderReadOnlyOptimal, SubresourceRange);
    CommandBuffer_->end();

    vk::SubmitInfo SubmitInfo;
    {
      SubmitInfo.commandBufferCount = 1;
      SubmitInfo.pCommandBuffers = &CommandBuffer_.get();
    }

    SubmitAndWait(SubmitInfo, vk::QueueFlagBits::eTransfer);
  }

  (*_ppRenderTexture) = std::move(pRenderTexture);
}

void CDevice::CreateDepthStecilTexture(ITexture** _ppDepthStecilTexture, const vdl::uint2& _TextureSize, vdl::FormatType _Format)
{
  assert(_ppDepthStecilTexture);

  const vk::Format Format = Cast(_Format);

  CDepthStencilTexture* pDepthStencilTexture = new CDepthStencilTexture;
  pDepthStencilTexture->TextureSize = _TextureSize;

  //  イメージ作成
  {
    vk::ImageCreateInfo ImageInfo;
    {
      ImageInfo.imageType = vk::ImageType::e2D;
      ImageInfo.format = Format;
      ImageInfo.extent = { pDepthStencilTexture->TextureSize.x, pDepthStencilTexture->TextureSize.y, 1 };
      ImageInfo.mipLevels = 1;
      ImageInfo.arrayLayers = 1;
      ImageInfo.samples = vk::SampleCountFlagBits::e1;
      ImageInfo.tiling = vk::ImageTiling::eOptimal;
      ImageInfo.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled;
      ImageInfo.sharingMode = vk::SharingMode::eExclusive;
      ImageInfo.initialLayout = vk::ImageLayout::eUndefined;
    }

    pDepthStencilTexture->Image = Device_->createImageUnique(ImageInfo);
    assert(pDepthStencilTexture->Image);
  }

  //  メモリの確保
  {
    vk::MemoryRequirements MemoryRequirement = Device_->getImageMemoryRequirements(pDepthStencilTexture->Image.get());

    vk::MemoryAllocateInfo MemoryAllocateInfo;
    {
      MemoryAllocateInfo.allocationSize = MemoryRequirement.size;
      MemoryAllocateInfo.memoryTypeIndex = GetMemoryTypeIndex(MemoryRequirement.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal);
    }

    pDepthStencilTexture->Memory = Device_->allocateMemoryUnique(MemoryAllocateInfo);
    assert(pDepthStencilTexture->Memory);
    Device_->bindImageMemory(pDepthStencilTexture->Image.get(), pDepthStencilTexture->Memory.get(), 0);
  }

  //  ビューの作成
  {
    vk::ImageViewCreateInfo ImageViewInfo;
    {
      ImageViewInfo.image = pDepthStencilTexture->Image.get();
      ImageViewInfo.viewType = vk::ImageViewType::e2D;
      ImageViewInfo.format = Format;
      ImageViewInfo.components.r = vk::ComponentSwizzle::eR;
      ImageViewInfo.components.g = vk::ComponentSwizzle::eG;
      ImageViewInfo.components.b = vk::ComponentSwizzle::eB;
      ImageViewInfo.components.a = vk::ComponentSwizzle::eA;
      ImageViewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil;
      ImageViewInfo.subresourceRange.baseArrayLayer = 0;
      ImageViewInfo.subresourceRange.baseMipLevel = 0;
      ImageViewInfo.subresourceRange.layerCount = 1;
      ImageViewInfo.subresourceRange.levelCount = 1;
    }

    pDepthStencilTexture->View = Device_->createImageViewUnique(ImageViewInfo);
    assert(pDepthStencilTexture->View);
  }

  //  レイアウトの変更
  {
    vk::ImageSubresourceRange SubresourceRange;
    {
      SubresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil;
      SubresourceRange.baseMipLevel = 0;
      SubresourceRange.levelCount = 1;
      SubresourceRange.baseArrayLayer = 0;
      SubresourceRange.layerCount = 1;
    }

    CommandBuffer_->begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
    pDepthStencilTexture->SetImageLayout(CommandBuffer_.get(), vk::ImageLayout::eDepthStencilAttachmentOptimal, SubresourceRange);
    CommandBuffer_->end();

    vk::SubmitInfo SubmitInfo;
    {
      SubmitInfo.commandBufferCount = 1;
      SubmitInfo.pCommandBuffers = &CommandBuffer_.get();
    }

    SubmitAndWait(SubmitInfo, vk::QueueFlagBits::eTransfer);
  }

  (*_ppDepthStecilTexture) = std::move(pDepthStencilTexture);
}

void CDevice::CreateUnorderedAccessTexture(ITexture** _ppUnorderedAccessTexture, const vdl::uint2& _TextureSize, vdl::FormatType _Format)
{
  assert(_ppUnorderedAccessTexture);

  const vk::Format Format = Cast(_Format);

  CUnorderedAccessTexture* pUnorderedAccessTexture = new CUnorderedAccessTexture;
  pUnorderedAccessTexture->TextureSize = _TextureSize;

  //  イメージ作成
  {
    vk::ImageCreateInfo ImageInfo;
    {
      ImageInfo.imageType = vk::ImageType::e2D;
      ImageInfo.format = Format;
      ImageInfo.extent = { pUnorderedAccessTexture->TextureSize.x, pUnorderedAccessTexture->TextureSize.y, 1 };
      ImageInfo.mipLevels = 1;
      ImageInfo.arrayLayers = 1;
      ImageInfo.samples = vk::SampleCountFlagBits::e1;
      ImageInfo.tiling = vk::ImageTiling::eOptimal;
      ImageInfo.usage = vk::ImageUsageFlagBits::eStorage | vk::ImageUsageFlagBits::eSampled;
      ImageInfo.sharingMode = vk::SharingMode::eExclusive;
      ImageInfo.initialLayout = vk::ImageLayout::eUndefined;
    }

    pUnorderedAccessTexture->Image = Device_->createImageUnique(ImageInfo);
    assert(pUnorderedAccessTexture->Image);
  }

  //  メモリの確保
  {
    vk::MemoryRequirements MemoryRequirement = Device_->getImageMemoryRequirements(pUnorderedAccessTexture->Image.get());

    vk::MemoryAllocateInfo MemoryAllocateInfo;
    {
      MemoryAllocateInfo.allocationSize = MemoryRequirement.size;
      MemoryAllocateInfo.memoryTypeIndex = GetMemoryTypeIndex(MemoryRequirement.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal);
    }

    pUnorderedAccessTexture->Memory = Device_->allocateMemoryUnique(MemoryAllocateInfo);
    assert(pUnorderedAccessTexture->Memory);
    Device_->bindImageMemory(pUnorderedAccessTexture->Image.get(), pUnorderedAccessTexture->Memory.get(), 0);
  }

  //  ビューの作成
  {
    vk::ImageViewCreateInfo ImageViewInfo;
    {
      ImageViewInfo.image = pUnorderedAccessTexture->Image.get();
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

    pUnorderedAccessTexture->View = Device_->createImageViewUnique(ImageViewInfo);
    assert(pUnorderedAccessTexture->View);
  }

  //  レイアウトの変更
  {
    vk::ImageSubresourceRange SubresourceRange;
    {
      SubresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
      SubresourceRange.baseMipLevel = 0;
      SubresourceRange.levelCount = 1;
      SubresourceRange.baseArrayLayer = 0;
      SubresourceRange.layerCount = 1;
    }

    CommandBuffer_->begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
    pUnorderedAccessTexture->SetImageLayout(CommandBuffer_.get(), vk::ImageLayout::eShaderReadOnlyOptimal, SubresourceRange);
    CommandBuffer_->end();

    vk::SubmitInfo SubmitInfo;
    {
      SubmitInfo.commandBufferCount = 1;
      SubmitInfo.pCommandBuffers = &CommandBuffer_.get();
    }

    SubmitAndWait(SubmitInfo, vk::QueueFlagBits::eTransfer);
  }

  (*_ppUnorderedAccessTexture) = std::move(pUnorderedAccessTexture);
}

void CDevice::WriteMemory(IBuffer* _pDstBuffer, const void* _pSrcBuffer, vdl::uint _BufferSize)const
{
  switch (_pDstBuffer->GetType())
  {
  case BufferType::eVertexBuffer:
    WriteMemory(&static_cast<CVertexBuffer*>(_pDstBuffer)->BufferData, _pSrcBuffer, _BufferSize);
    break;
  case BufferType::eInstanceBuffer:
    WriteMemory(&static_cast<CInstanceBuffer*>(_pDstBuffer)->BufferData, _pSrcBuffer, _BufferSize);
    break;
  case BufferType::eIndexBuffer:
    WriteMemory(&static_cast<CIndexBuffer*>(_pDstBuffer)->BufferData, _pSrcBuffer, _BufferSize);
    break;
  case BufferType::eConstantBuffer:
    WriteMemory(&static_cast<CConstantBuffer*>(_pDstBuffer)->BufferData, _pSrcBuffer, _BufferSize);
    break;
  case BufferType::eUnorderedAccessBuffer:
    WriteMemory(&static_cast<CUnordererdAccessBuffer*>(_pDstBuffer)->BufferData, _pSrcBuffer, _BufferSize);
    break;
  default: assert(false);
  }
}

void CDevice::LoadShader(IShader** _ppShader, const char* _FilePath, const char* _EntryPoint, ShaderType _Type)
{
  assert(_ppShader);

  std::vector<vdl::uint> Spriv;
  ::ComplineShader(&Spriv, _Type, _FilePath, _EntryPoint);

  vk::ShaderModuleCreateInfo ShaderModuleInfo;
  {
    ShaderModuleInfo.codeSize = Spriv.size() * sizeof(vdl::uint);
    ShaderModuleInfo.pCode = Spriv.data();
  }

  switch (_Type)
  {
    //case ShaderType::eVertexShader:
    //  break;
  case ShaderType::eHullShader:
  {
    CHullShader* pHullShader = new CHullShader;
    pHullShader->ShaderData.EntryPoint = _EntryPoint;
    pHullShader->ShaderData.Module = Device_->createShaderModuleUnique(ShaderModuleInfo);

    (*_ppShader) = std::move(pHullShader);
  }
  break;
  case ShaderType::eDomainShader:
  {
    CDomainShader* pDomainShader = new CDomainShader;
    pDomainShader->ShaderData.EntryPoint = _EntryPoint;
    pDomainShader->ShaderData.Module = Device_->createShaderModuleUnique(ShaderModuleInfo);

    (*_ppShader) = std::move(pDomainShader);
  }
  break;
  case ShaderType::eGeometryShader:
  {
    CGeometryShader* pGeometryShader = new CGeometryShader;
    pGeometryShader->ShaderData.EntryPoint = _EntryPoint;
    pGeometryShader->ShaderData.Module = Device_->createShaderModuleUnique(ShaderModuleInfo);

    (*_ppShader) = std::move(pGeometryShader);
  }
  break;
  case ShaderType::ePixelShader:
  {
    CPixelShader* pPixelShader = new CPixelShader;
    pPixelShader->ShaderData.EntryPoint = _EntryPoint;
    pPixelShader->ShaderData.Module = Device_->createShaderModuleUnique(ShaderModuleInfo);

    (*_ppShader) = std::move(pPixelShader);
  }
  break;
  case ShaderType::eComputeShader:
  {
    CComputeShader* pComputeShader = new CComputeShader;
    pComputeShader->ShaderData.EntryPoint = _EntryPoint;
    pComputeShader->ShaderData.Module = Device_->createShaderModuleUnique(ShaderModuleInfo);

    (*_ppShader) = std::move(pComputeShader);
  }
  break;
  default: assert(false);
  }
}

void CDevice::LoadShader(IShader** _ppShader, const char* _Source, vdl::uint _DataSize, const char* _EntryPoint, ShaderType _Type)
{
  assert(_ppShader);

  std::vector<vdl::uint> Spriv;
  ::ComplineShader(&Spriv, _Type, _Source, _DataSize, _EntryPoint);

  vk::ShaderModuleCreateInfo ShaderModuleInfo;
  {
    ShaderModuleInfo.codeSize = Spriv.size() * sizeof(vdl::uint);
    ShaderModuleInfo.pCode = Spriv.data();
  }

  switch (_Type)
  {
    //case ShaderType::eVertexShader:
    //  break;
  case ShaderType::eHullShader:
  {
    CHullShader* pHullShader = new CHullShader;
    pHullShader->ShaderData.EntryPoint = _EntryPoint;
    pHullShader->ShaderData.Module = Device_->createShaderModuleUnique(ShaderModuleInfo);

    (*_ppShader) = std::move(pHullShader);
  }
  break;
  case ShaderType::eDomainShader:
  {
    CDomainShader* pDomainShader = new CDomainShader;
    pDomainShader->ShaderData.EntryPoint = _EntryPoint;
    pDomainShader->ShaderData.Module = Device_->createShaderModuleUnique(ShaderModuleInfo);

    (*_ppShader) = std::move(pDomainShader);
  }
  break;
  case ShaderType::eGeometryShader:
  {
    CGeometryShader* pGeometryShader = new CGeometryShader;
    pGeometryShader->ShaderData.EntryPoint = _EntryPoint;
    pGeometryShader->ShaderData.Module = Device_->createShaderModuleUnique(ShaderModuleInfo);

    (*_ppShader) = std::move(pGeometryShader);
  }
  break;
  case ShaderType::ePixelShader:
  {
    CPixelShader* pPixelShader = new CPixelShader;
    pPixelShader->ShaderData.EntryPoint = _EntryPoint;
    pPixelShader->ShaderData.Module = Device_->createShaderModuleUnique(ShaderModuleInfo);

    (*_ppShader) = std::move(pPixelShader);
  }
  break;
  case ShaderType::eComputeShader:
  {
    CComputeShader* pComputeShader = new CComputeShader;
    pComputeShader->ShaderData.EntryPoint = _EntryPoint;
    pComputeShader->ShaderData.Module = Device_->createShaderModuleUnique(ShaderModuleInfo);

    (*_ppShader) = std::move(pComputeShader);
  }
  break;
  default: assert(false);
  }
}

void CDevice::LoadShader(IVertexShader** _ppVertexShader, const char* _FilePath, const char* _EntryPoint, vdl::InputLayoutType _InputLayout)
{
  assert(_ppVertexShader);

  CVertexShader* pVertexShader = new CVertexShader;
  pVertexShader->InputLayout = _InputLayout;

  std::vector<vdl::uint> Spriv;
  ::ComplineShader(&Spriv, ShaderType::eVertexShader, _FilePath, _EntryPoint);

  vk::ShaderModuleCreateInfo ShaderModuleInfo;
  {
    ShaderModuleInfo.codeSize = Spriv.size() * sizeof(vdl::uint);
    ShaderModuleInfo.pCode = Spriv.data();
  }

  pVertexShader->ShaderData.EntryPoint = _EntryPoint;
  pVertexShader->ShaderData.Module = Device_->createShaderModuleUnique(ShaderModuleInfo);

  (*_ppVertexShader) = std::move(pVertexShader);
}

void CDevice::LoadShader(IVertexShader** _ppVertexShader, const char* _Source, vdl::uint _DataSize, const char* _EntryPoint, vdl::InputLayoutType _InputLayout)
{
  assert(_ppVertexShader);

  CVertexShader* pVertexShader = new CVertexShader;
  pVertexShader->InputLayout = _InputLayout;

  std::vector<vdl::uint> Spriv;
  ::ComplineShader(&Spriv, ShaderType::eVertexShader, _Source, _DataSize, _EntryPoint);

  vk::ShaderModuleCreateInfo ShaderModuleInfo;
  {
    ShaderModuleInfo.codeSize = Spriv.size() * sizeof(vdl::uint);
    ShaderModuleInfo.pCode = Spriv.data();
  }

  pVertexShader->ShaderData.EntryPoint = _EntryPoint;
  pVertexShader->ShaderData.Module = Device_->createShaderModuleUnique(ShaderModuleInfo);

  (*_ppVertexShader) = std::move(pVertexShader);
}

//--------------------------------------------------

void CDevice::SubmitAndWait(vk::SubmitInfo _SubmitInfo, vk::QueueFlags _QueueFlag)const
{
  assert(_QueueFlag & vk::QueueFlagBits::eGraphics || _QueueFlag & vk::QueueFlagBits::eCompute || _QueueFlag & vk::QueueFlagBits::eTransfer);

  const vk::Queue& Queue = (_QueueFlag & vk::QueueFlagBits::eGraphics ? GraphicsQueue_ : _QueueFlag & vk::QueueFlagBits::eCompute ? ComputeQueue_ : CopyQueue_);
  Queue.submit(_SubmitInfo, vk::Fence());
  Queue.waitIdle();
}

//--------------------------------------------------

vdl::uint CDevice::FindQueue(vk::QueueFlags _QueueFlag, const vk::SurfaceKHR& _Surface = vk::SurfaceKHR())const
{
  std::vector<vk::QueueFamilyProperties> QueueFamilyProperties = PhysicalDevice_.getQueueFamilyProperties();
  const vdl::uint QueueFamiliyNum = QueueFamilyProperties.size();

  for (vdl::uint QueueFamiliyCount = 0; QueueFamiliyCount < QueueFamiliyNum; ++QueueFamiliyCount)
  {
    if (QueueFamilyProperties[QueueFamiliyCount].queueFlags & _QueueFlag)
    {
      //  サーフェスのサポートがされていない時は続ける
      if (_Surface && !PhysicalDevice_.getSurfaceSupportKHR(QueueFamiliyCount, _Surface))
      {
        continue;
      }

      return QueueFamiliyCount;
    }
  }

  _ASSERT_EXPR_A(false, "Not Found.");

  return 0;
}

vdl::uint CDevice::GetMemoryTypeIndex(vdl::uint _MemoryTypeBits, const vk::MemoryPropertyFlags& _MemoryPropertys)const
{
  const vk::PhysicalDeviceMemoryProperties DeviceMemoryProperties = PhysicalDevice_.getMemoryProperties();

  for (vdl::uint TypeCount = 0; TypeCount < DeviceMemoryProperties.memoryTypeCount; ++TypeCount)
  {
    if (((_MemoryTypeBits & 1) == 1)
      && (DeviceMemoryProperties.memoryTypes[TypeCount].propertyFlags & _MemoryPropertys) == _MemoryPropertys)
    {
      return TypeCount;
    }
    _MemoryTypeBits >>= 1;
  }

  _ASSERT_EXPR_A(false, "Not Found.");

  return 0;
}

void CDevice::CreateBuffer(BufferData* _pBuffer, vk::DeviceSize _BufferSize, vk::BufferUsageFlags _Usage, vk::MemoryPropertyFlags _Properties)const
{
  assert(_pBuffer);

  //  バッファの作成
  {
    vk::BufferCreateInfo BufferInfo;
    {
      BufferInfo.size = _BufferSize;
      BufferInfo.usage = _Usage;
      BufferInfo.sharingMode = vk::SharingMode::eExclusive;
      BufferInfo.queueFamilyIndexCount = 0;
      BufferInfo.pQueueFamilyIndices = nullptr;
    }

    _pBuffer->Buffer = Device_->createBufferUnique(BufferInfo);
    assert(_pBuffer->Buffer);
  }

  //  メモリの確保
  {
    const vk::MemoryRequirements MemoryRequirement = Device_->getBufferMemoryRequirements(_pBuffer->Buffer.get());

    vk::MemoryAllocateInfo MemoryAllocateInfo;
    {
      MemoryAllocateInfo.allocationSize = MemoryRequirement.size;
      MemoryAllocateInfo.memoryTypeIndex = GetMemoryTypeIndex(MemoryRequirement.memoryTypeBits, _Properties);
    }
    _pBuffer->Memory = Device_->allocateMemoryUnique(MemoryAllocateInfo);
    assert(_pBuffer->Memory);

    Device_->bindBufferMemory(_pBuffer->Buffer.get(), _pBuffer->Memory.get(), 0);
  }

  //  ステージングしない時ポインタを取得する
  if (!(_Properties & vk::MemoryPropertyFlagBits::eDeviceLocal))
  {
    _pBuffer->pData = Device_->mapMemory(_pBuffer->Memory.get(), 0, _BufferSize);
  }
}

void CDevice::CreateStagingBuffer(BufferData* _pStagingBuffer, const void* _Buffer, vdl::uint _BufferSize)const
{
  assert(_pStagingBuffer);

  CreateBuffer(_pStagingBuffer, _BufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

  //  メモリへ書き込み
  WriteMemory(_pStagingBuffer, _Buffer, _BufferSize);
}

void CDevice::CopyBuffer(vk::Buffer _SrcBuffer, vk::Buffer _DstBuffer, vdl::uint _BufferSize)const
{
  CommandBuffer_->begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));

  //  バッファをコピー
  {
    vk::BufferCopy CopyRegion;
    {
      CopyRegion.size = _BufferSize;
    }
    CommandBuffer_->copyBuffer(_SrcBuffer, _DstBuffer, CopyRegion);
  }

  CommandBuffer_->end();

  //  コピーが終わるまで待機
  {
    vk::SubmitInfo SubmitInfo;
    {
      SubmitInfo.commandBufferCount = 1;
      SubmitInfo.pCommandBuffers = &CommandBuffer_.get();
    }

    SubmitAndWait(SubmitInfo, vk::QueueFlagBits::eTransfer);
  }
}

void CDevice::WriteMemory(BufferData* _pDstBuffer, const void* _pSrcBuffer, vdl::uint _BufferSize)const
{
  ::memcpy(_pDstBuffer->pData, _pSrcBuffer, _BufferSize);
}
