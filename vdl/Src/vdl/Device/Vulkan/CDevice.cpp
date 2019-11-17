#include "CDevice.hpp"

#include <vdl/Format/Vulkan/Format.hpp>
#include <vdl/Shader/Vulkan/CShader.hpp>
#include <vdl/Misc/Misc.hpp>

#include <vdl/Format/Format.hpp>
#include <vdl/Constants/Constants.hpp>

#include <vdl/Image.hpp>
#include <vdl/Macro.hpp>

#undef min
#undef max
#pragma warning(disable:26495)
#include <ThirdParty/glslang/SPIRV/GlslangToSpv.h>
#include <ThirdParty/glslang/StandAlone/DirStackFileIncluder.h>
#include <ThirdParty/glslang/StandAlone/ResourceLimits.hpp>
#pragma warning(default:26495)

#include <vdl/DetectMemoryLeak.hpp>

#include <sstream>

namespace
{
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
          LayoutSet = GetDescriptorLayoutOffset(Type_, DescriptorType::eSampler);
        }
        //  Texture
        else if (_pSymbol->getQualifier().layoutFormat == glslang::TLayoutFormat::ElfNone)
        {
          LayoutSet = GetDescriptorLayoutOffset(Type_, DescriptorType::eTexture);
        }
        //  RWTexture
        else
        {
          assert(Type_ == ShaderType::eComputeShader);
          LayoutSet = GetDescriptorLayoutOffset(Type_, DescriptorType::eUnorderedAccessTexture);
        }
      }
      else if (Type == glslang::TBasicType::EbtBlock)
      {
        const glslang::TQualifier& Qualifier = _pSymbol->getQualifier();

        //  ConstantBuffer
        if (Qualifier.storage == glslang::TStorageQualifier::EvqUniform)
        {
          LayoutSet = GetDescriptorLayoutOffset(Type_, DescriptorType::eConstantBuffer);
        }
        //  StructuredBuffer
        else
        {
          switch (Qualifier.declaredBuiltIn)
          {
          case glslang::TBuiltInVariable::EbvRWStructuredBuffer:
            assert(Type_ == ShaderType::eComputeShader);
            LayoutSet = GetDescriptorLayoutOffset(ShaderType::eComputeShader, DescriptorType::eUnorderedAccessBuffer);
            break;
          case glslang::TBuiltInVariable::EbvStructuredBuffer:
            LayoutSet = GetDescriptorLayoutOffset(Type_, DescriptorType::eBuffer);
            break;
          default: assert(false);
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

  constexpr EShMessages kControls = static_cast<EShMessages>(EShMsgCascadingErrors | EShMsgReadHlsl
    | EShMsgSpvRules | EShMsgKeepUncalled | EShMsgVulkanRules);

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

  inline void ComplineShader(std::vector<vdl::uint>* _pCode, ShaderType _Type, const char* _Source, vdl::uint /*_DataSize*/, const char* _EntryPoint)
  {
    const EShLanguage Stage = Cast(_Type);

    bool Result = true;

    glslang::InitializeProcess();

    //  シェーダーファイルのコンパイル
    glslang::TShader Shader(Stage);
    {
      Shader.setStrings(&_Source, 1);
      Shader.setEntryPoint(_EntryPoint);
      Shader.setEnvInput(glslang::EShSourceHlsl, Stage, glslang::EShClientVulkan, 110);
      Shader.setEnvClient(glslang::EShClientVulkan, glslang::EShTargetVulkan_1_1);
      Shader.setEnvTarget(glslang::EShTargetSpv, glslang::EShTargetSpv_1_3);

      Result = Shader.parse(&glslang::DefaultTBuiltInResource, 110, false, kControls);
      _ASSERT_EXPR_A(Result, "コンパイル失敗");
    }

    //  シェーダーファイルのリンク
    glslang::TProgram Program;
    {
      Program.addShader(&Shader);

      Result = Program.link(kControls);
      _ASSERT_EXPR_A(Result, "シェーダーファイルのリンクに失敗");
    }

    IntermTraverser IntermTraverser(_Type);
    Program.buildReflection();

    glslang::TIntermediate* pIntermediate = Program.getIntermediate(Stage);
    pIntermediate->getTreeRoot()->traverse(&IntermTraverser);

    glslang::GlslangToSpv(*pIntermediate, *_pCode);

    glslang::FinalizeProcess();
  }

  inline void ComplineShader(std::vector<vdl::uint>* _pCode, ShaderType _Type, const char* _FilePath, const char* _EntryPoint)
  {
    const EShLanguage Stage = Cast(_Type);

    bool Result = true;

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

    glslang::InitializeProcess();

    const char* pData = Data.data();
    const int DataSize = static_cast<int>(Data.size());

    //  シェーダーファイルのコンパイル
    glslang::TShader Shader(Stage);
    {
      Shader.setStringsWithLengthsAndNames(&pData, &DataSize, &_FilePath, 1);
      Shader.setEntryPoint(_EntryPoint);
      Shader.setEnvInput(glslang::EShSourceHlsl, Stage, glslang::EShClientVulkan, 110);
      Shader.setEnvClient(glslang::EShClientVulkan, glslang::EShTargetVulkan_1_1);
      Shader.setEnvTarget(glslang::EShTargetSpv, glslang::EShTargetSpv_1_3);

      DirStackFileIncluder Includer;
      Result = Shader.parse(&glslang::DefaultTBuiltInResource, 110, false, kControls, Includer);
      _ASSERT_EXPR_A(Result, "コンパイル失敗");
    }

    //  シェーダーファイルのリンク
    glslang::TProgram Program;
    {
      Program.addShader(&Shader);

      Result = Program.link(kControls);
      _ASSERT_EXPR_A(Result, "シェーダーファイルのリンクに失敗");
    }

    IntermTraverser IntermTraverser(_Type);
    Program.buildReflection();

    glslang::TIntermediate* pIntermediate = Program.getIntermediate(Stage);
    pIntermediate->getTreeRoot()->traverse(&IntermTraverser);

    glslang::GlslangToSpv(*pIntermediate, *_pCode);

    glslang::FinalizeProcess();
  }

  inline vk::IndexType Cast(IndexType _Type)
  {
    switch (_Type)
    {
    case IndexType::eUint16:
      return vk::IndexType::eUint16;
    case IndexType::eUint32:
      return vk::IndexType::eUint32;
    default: assert(false);
    }

    return vk::IndexType::eUint16;
  }

  // デバッグ時のメッセージコールバック
  VkBool32 DebugMessageCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT, uint64_t, size_t, int32_t, const char* pLayerPrefix, const char* pMsg, void*)
  {
    std::string Message = pMsg;
    {
      constexpr const char* kIgnoreMessages[2] =
      {
       " [ UNASSIGNED-CoreValidation-Shader-InterfaceTypeMismatch ] ",  //  glslangによるエラー
       " [ UNASSIGNED-CoreValidation-Shader-OutputNotConsumed ] ",      //  glslangによるエラー
      };

      for (auto& IgnoreMessage : kIgnoreMessages)
      {
        std::string Str = IgnoreMessage;

        if (std::equal(Str.cbegin(), Str.cend(), std::begin(Message)))
        {
          return false;
        }
      }
    }

    VkBool32 Result = true;
    {
      std::stringstream Buf;
      if (flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT)
      {
        Buf << "INFORMATION:";
        Result = false;
      }
      else if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
      {
        Buf << "WARNING:";
      }
      else if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
      {
        Buf << "PERFORMANCE_WARNING:";
        Result = false;
      }
      else if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
      {
        Buf << "ERROR:";
      }
      else if (flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT)
      {
        Buf << "DEBUG:";
        Result = false;
      }

      Buf << "[" << pLayerPrefix << "] :" << Message.c_str() << std::endl;
      Message = Buf.str();
    }

    // デバッグウィンドウに出力
    ::OutputDebugStringA(Message.c_str());
    assert(!Result);

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
      InstanceInfo.enabledExtensionCount = static_cast<vdl::uint>(vdl::Macro::ArraySize(Extensions));
      InstanceInfo.ppEnabledExtensionNames = Extensions;
#if defined DEBUG | _DEBUG
      InstanceInfo.enabledLayerCount = static_cast<vdl::uint>(vdl::Macro::ArraySize(Layers));
      InstanceInfo.ppEnabledLayerNames = Layers;
#endif
    }

    Instance_ = vk::createInstanceUnique(InstanceInfo);
    assert(Instance_);
  }

  //  物理デバイスの取得
  {
    std::vector<vk::PhysicalDevice> PhysicalDevices = Instance_->enumeratePhysicalDevices();
    assert(!PhysicalDevices.empty());

    //  TODO:MultiGPU
    PhysicalDevice_ = PhysicalDevices[0];
  }

  GraphicsQueueIndex_ = FindQueue(vk::QueueFlagBits::eGraphics | vk::QueueFlagBits::eCompute);
  assert(GraphicsQueueIndex_ != UINT_MAX);
  ComputeQueueIndex_ = FindQueue(vk::QueueFlagBits::eCompute, vk::QueueFlagBits::eGraphics);

  //  論理デバイスの作成
  {
    const char* Extensions[] = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };

    constexpr float GraphicsQueuePriorities[] = { 1.0f, 0.5f };
    constexpr float ComputeQueuePriorities[] = { 0.5f };

    std::vector<vk::DeviceQueueCreateInfo> DeviceQueueInfos(1);
    {
      vdl::uint GraphicsQueueCount = 1;

      if (ComputeQueueIndex_ == UINT_MAX)
      {
        ++GraphicsQueueCount;
        ComputeQueueIndex_ = GraphicsQueueIndex_;
      }
      else
      {
        vk::DeviceQueueCreateInfo DeviceQueueInfo;
        {
          DeviceQueueInfo.queueFamilyIndex = ComputeQueueIndex_;
          DeviceQueueInfo.queueCount = 1;
          DeviceQueueInfo.pQueuePriorities = ComputeQueuePriorities;
        }

        DeviceQueueInfos.emplace_back(std::move(DeviceQueueInfo));
      }

      DeviceQueueInfos[0].queueFamilyIndex = GraphicsQueueIndex_;
      DeviceQueueInfos[0].queueCount = GraphicsQueueCount;
      DeviceQueueInfos[0].pQueuePriorities = GraphicsQueuePriorities;
    }

    vk::PhysicalDeviceFeatures EnabledFeatures = PhysicalDevice_.getFeatures();
    {
      EnabledFeatures.largePoints = false;
      EnabledFeatures.shaderTessellationAndGeometryPointSize = false;
    }

    vk::DeviceCreateInfo DeviceInfo;
    {
      DeviceInfo.queueCreateInfoCount = static_cast<vdl::uint>(DeviceQueueInfos.size());
      DeviceInfo.pQueueCreateInfos = DeviceQueueInfos.data();
      DeviceInfo.enabledLayerCount = static_cast<vdl::uint>(vdl::Macro::ArraySize(Layers));
      DeviceInfo.ppEnabledLayerNames = Layers;
      DeviceInfo.enabledExtensionCount = static_cast<vdl::uint>(vdl::Macro::ArraySize(Extensions));
      DeviceInfo.ppEnabledExtensionNames = Extensions;
      DeviceInfo.pEnabledFeatures = &EnabledFeatures;
    }

    VkDevice_ = PhysicalDevice_.createDeviceUnique(DeviceInfo);
    assert(VkDevice_);
  }

  //  コマンドプールの作成
  {
    vk::CommandPoolCreateInfo CommandPoolInfo;
    {
      CommandPoolInfo.queueFamilyIndex = GraphicsQueueIndex_;
      CommandPoolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    }

    CommandPool_ = VkDevice_->createCommandPoolUnique(CommandPoolInfo);
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

    CommandBuffer_ = std::move(VkDevice_->allocateCommandBuffersUnique(CommandBufferAllocateInfo).front());
  }

  //  キューの取得
  {
    GraphicsQueue_ = VkDevice_->getQueue(GraphicsQueueIndex_, 0);
    assert(GraphicsQueue_);

    ComputeQueue_ = VkDevice_->getQueue(ComputeQueueIndex_, (GraphicsQueueIndex_ == ComputeQueueIndex_ ? 1 : 0));
    assert(ComputeQueue_);
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

void CDevice::CreateVertexBuffer(IBuffer** _ppVertexBuffer, vdl::uint _BufferSize)
{
  assert(_ppVertexBuffer);

  CVertexBuffer* pVertexBuffer = new CVertexBuffer;
  CreateBuffer(&pVertexBuffer->BufferData, _BufferSize, vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

  (*_ppVertexBuffer) = std::move(pVertexBuffer);
}

void CDevice::CreateVertexBuffer(IBuffer** _ppVertexBuffer, const void* _Vertices, vdl::uint _BufferSize)
{
  assert(_ppVertexBuffer);

  CVertexBuffer* pVertexBuffer = new CVertexBuffer;
  CreateBuffer(&pVertexBuffer->BufferData, _BufferSize, vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal);

  BufferData StagingBuffer;
  CreateStagingBuffer(&StagingBuffer, _Vertices, _BufferSize);
  CopyBuffer(StagingBuffer.Buffer.get(), pVertexBuffer->BufferData.Buffer.get(), _BufferSize);

  (*_ppVertexBuffer) = std::move(pVertexBuffer);
}

void CDevice::CreateInstanceBuffer(IBuffer** _ppInstanceBuffer, vdl::uint _BufferSize)
{
  assert(_ppInstanceBuffer);

  CInstanceBuffer* pInstanceBuffer = new CInstanceBuffer;
  pInstanceBuffer->BufferSize = _BufferSize * kInstanceBufferSizeMultiple;
  CreateBuffer(&pInstanceBuffer->BufferData, pInstanceBuffer->BufferSize, vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

  (*_ppInstanceBuffer) = std::move(pInstanceBuffer);
}

void CDevice::CreateIndexBuffer(IBuffer** _ppIndexBuffer, vdl::uint _BufferSize, IndexType _IndexType)
{
  assert(_ppIndexBuffer);

  CIndexBuffer* pIndexBuffer = new CIndexBuffer;
  pIndexBuffer->IndexType = Cast(_IndexType);
  CreateBuffer(&pIndexBuffer->BufferData, _BufferSize, vk::BufferUsageFlagBits::eIndexBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

  (*_ppIndexBuffer) = std::move(pIndexBuffer);
}

void CDevice::CreateIndexBuffer(IBuffer** _ppIndexBuffer, const void* _Indices, vdl::uint _BufferSize, IndexType _IndexType)
{
  assert(_ppIndexBuffer);

  CIndexBuffer* pIndexBuffer = new CIndexBuffer;
  pIndexBuffer->IndexType = Cast(_IndexType);
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
  CreateBuffer(&pConstantBuffer->BufferData, pConstantBuffer->BufferSize, vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

  (*_ppConstantBuffer) = std::move(pConstantBuffer);
}

void CDevice::CreateUnorderedAccessBuffer(IBuffer** _ppUnorderedAccessBuffer, vdl::uint, vdl::uint _BufferSize, const void* _Buffer)
{
  assert(_ppUnorderedAccessBuffer);

  CUnordererdAccessBuffer* pUnorderedAccessBuffer = new CUnordererdAccessBuffer;
  pUnorderedAccessBuffer->BufferSize = _BufferSize;
  CreateBuffer(&pUnorderedAccessBuffer->BufferData, pUnorderedAccessBuffer->BufferSize, vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal);

  if (_Buffer)
  {
    BufferData StagingBuffer;
    CreateStagingBuffer(&StagingBuffer, _Buffer, pUnorderedAccessBuffer->BufferSize);
    CopyBuffer(StagingBuffer.Buffer.get(), pUnorderedAccessBuffer->BufferData.Buffer.get(), pUnorderedAccessBuffer->BufferSize);
  }

  (*_ppUnorderedAccessBuffer) = std::move(pUnorderedAccessBuffer);
}

void CDevice::CreateTexture(ITexture** _ppTexture, const vdl::Image& _Image)
{
  assert(_ppTexture);

  constexpr vk::Format kTextureFormat = Cast(Constants::kTextureFormat);

  CTexture* pTexture = new CTexture;
  pTexture->TextureSize = _Image.GetTextureSize();

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
  CreateImage(&pTexture->TextureData, ImageInfo);

  vk::ImageSubresourceRange SubresourceRange;
  {
    SubresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    SubresourceRange.baseMipLevel = 0;
    SubresourceRange.levelCount = 1;
    SubresourceRange.baseArrayLayer = 0;
    SubresourceRange.layerCount = 1;
  }

  //  メモリのコピー
  {
    BufferData StagingBuffer;
    CreateStagingBuffer(&StagingBuffer, _Image.Buffer(), _Image.BufferSize());

    vk::BufferImageCopy CopyRegion;
    {
      CopyRegion.bufferOffset = 0;
      CopyRegion.bufferRowLength = 0;
      CopyRegion.bufferImageHeight = 0;
      CopyRegion.imageSubresource.aspectMask = SubresourceRange.aspectMask;
      CopyRegion.imageSubresource.mipLevel = 0;
      CopyRegion.imageSubresource.baseArrayLayer = 0;
      CopyRegion.imageSubresource.layerCount = 1;
      CopyRegion.imageOffset = { 0, 0, 0 };
      CopyRegion.imageExtent = ImageInfo.extent;
    }

    CommandBuffer_->begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
    pTexture->TextureData.SetImageLayout(CommandBuffer_.get(), vk::ImageLayout::eTransferDstOptimal, SubresourceRange);
    CommandBuffer_->copyBufferToImage(StagingBuffer.Buffer.get(), pTexture->TextureData.Image.get(), vk::ImageLayout::eTransferDstOptimal, CopyRegion);
    pTexture->TextureData.SetImageLayout(CommandBuffer_.get(), vk::ImageLayout::eShaderReadOnlyOptimal, SubresourceRange);
    CommandBuffer_->end();


    vk::SubmitInfo SubmitInfo;
    {
      SubmitInfo.commandBufferCount = 1;
      SubmitInfo.pCommandBuffers = &CommandBuffer_.get();
    }
    SubmitAndWait(SubmitInfo);
  }

  CreateImageView(&pTexture->TextureData, kTextureFormat, SubresourceRange, vk::ImageViewType::e2D);

  (*_ppTexture) = std::move(pTexture);
}

void CDevice::CreateCubeTexture(ITexture** _ppTexture, const std::array<vdl::Image, 6>& _Images)
{
  assert(_ppTexture);

  constexpr vk::Format kTextureFormat = Cast(Constants::kTextureFormat);

  CCubeTexture* pCubeTexture = new CCubeTexture;
  pCubeTexture->TextureSize = _Images[0].GetTextureSize();

  vk::ImageCreateInfo ImageInfo;
  {
    ImageInfo.imageType = vk::ImageType::e2D;
    ImageInfo.format = kTextureFormat;
    ImageInfo.extent = { pCubeTexture->TextureSize.x, pCubeTexture->TextureSize.y, 1 };
    ImageInfo.mipLevels = 1;
    ImageInfo.arrayLayers = 6;
    ImageInfo.samples = vk::SampleCountFlagBits::e1;
    ImageInfo.tiling = vk::ImageTiling::eOptimal;
    ImageInfo.usage = vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst;
    ImageInfo.sharingMode = vk::SharingMode::eExclusive;
    ImageInfo.initialLayout = vk::ImageLayout::eUndefined;
    ImageInfo.flags = vk::ImageCreateFlagBits::eCubeCompatible;
  }
  CreateImage(&pCubeTexture->TextureData, ImageInfo);

  vk::ImageSubresourceRange SubresourceRange;
  {
    SubresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    SubresourceRange.baseMipLevel = 0;
    SubresourceRange.levelCount = 1;
    SubresourceRange.baseArrayLayer = 0;
    SubresourceRange.layerCount = 6;
  }

  //  メモリのコピー
  {
    const vdl::uint BufferSize = _Images[0].BufferSize();

    BufferData StagingBuffer;
    CreateStagingBuffer(&StagingBuffer, nullptr, BufferSize * 6);

    std::array<vk::BufferImageCopy, 6> CopyRegions;
    {
      for (vdl::uint i = 0; i < 6; ++i)
      {
        CopyRegions[i].bufferOffset = i * BufferSize;
        CopyRegions[i].bufferRowLength = 0;
        CopyRegions[i].bufferImageHeight = 0;
        CopyRegions[i].imageSubresource.aspectMask = SubresourceRange.aspectMask;
        CopyRegions[i].imageSubresource.mipLevel = 0;
        CopyRegions[i].imageSubresource.baseArrayLayer = i;
        CopyRegions[i].imageSubresource.layerCount = 1;
        CopyRegions[i].imageOffset = { 0, 0, 0 };
        CopyRegions[i].imageExtent = ImageInfo.extent;

        ::memcpy(static_cast<vdl::uint8_t*>(StagingBuffer.pData) + CopyRegions[i].bufferOffset, _Images[i].Buffer(), BufferSize);
      }
    }

    CommandBuffer_->begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
    pCubeTexture->TextureData.SetImageLayout(CommandBuffer_.get(), vk::ImageLayout::eTransferDstOptimal, SubresourceRange);
    CommandBuffer_->copyBufferToImage(StagingBuffer.Buffer.get(), pCubeTexture->TextureData.Image.get(), vk::ImageLayout::eTransferDstOptimal, CopyRegions);
    pCubeTexture->TextureData.SetImageLayout(CommandBuffer_.get(), vk::ImageLayout::eShaderReadOnlyOptimal, SubresourceRange);
    CommandBuffer_->end();

    vk::SubmitInfo SubmitInfo;
    {
      SubmitInfo.commandBufferCount = 1;
      SubmitInfo.pCommandBuffers = &CommandBuffer_.get();
    }
    SubmitAndWait(SubmitInfo);
  }

  CreateImageView(&pCubeTexture->TextureData, kTextureFormat, SubresourceRange, vk::ImageViewType::eCube);

  (*_ppTexture) = std::move(pCubeTexture);
}

void CDevice::CreateRenderTexture(ITexture** _ppRenderTexture, const vdl::uint2& _TextureSize, vdl::FormatType _Format)
{
  assert(_ppRenderTexture);

  CRenderTexture* pRenderTexture = new CRenderTexture;
  pRenderTexture->TextureSize = _TextureSize;
  pRenderTexture->Format = _Format;
  pRenderTexture->VkFormat = Cast(_Format);
  if (!(PhysicalDevice_.getFormatProperties(pRenderTexture->VkFormat).optimalTilingFeatures & vk::FormatFeatureFlagBits::eColorAttachment))
  {
    pRenderTexture->VkFormat = EmulateFormat(_Format);
  }

  vk::ImageCreateInfo ImageInfo;
  {
    ImageInfo.imageType = vk::ImageType::e2D;
    ImageInfo.format = pRenderTexture->VkFormat;
    ImageInfo.extent = { pRenderTexture->TextureSize.x, pRenderTexture->TextureSize.y, 1 };
    ImageInfo.mipLevels = 1;
    ImageInfo.arrayLayers = 1;
    ImageInfo.samples = vk::SampleCountFlagBits::e1;
    ImageInfo.tiling = vk::ImageTiling::eOptimal;
    ImageInfo.usage = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst;
    ImageInfo.sharingMode = vk::SharingMode::eExclusive;
    ImageInfo.initialLayout = vk::ImageLayout::eUndefined;
  }
  CreateImage(&pRenderTexture->TextureData, ImageInfo);

  vk::ImageSubresourceRange SubresourceRange;
  {
    SubresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    SubresourceRange.baseMipLevel = 0;
    SubresourceRange.levelCount = 1;
    SubresourceRange.baseArrayLayer = 0;
    SubresourceRange.layerCount = 1;
  }

  CreateImageView(&pRenderTexture->TextureData, pRenderTexture->VkFormat, SubresourceRange, vk::ImageViewType::e2D);

  //  レイアウトの変更
  {
    CommandBuffer_->begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
    pRenderTexture->TextureData.SetImageLayout(CommandBuffer_.get(), vk::ImageLayout::eColorAttachmentOptimal, SubresourceRange);
    CommandBuffer_->end();

    vk::SubmitInfo SubmitInfo;
    {
      SubmitInfo.commandBufferCount = 1;
      SubmitInfo.pCommandBuffers = &CommandBuffer_.get();
    }

    SubmitAndWait(SubmitInfo);
  }

  (*_ppRenderTexture) = std::move(pRenderTexture);
}

void CDevice::CreateDepthStecilTexture(ITexture** _ppDepthStecilTexture, const vdl::uint2& _TextureSize, vdl::FormatType _Format)
{
  assert(_ppDepthStecilTexture);

  CDepthStencilTexture* pDepthStencilTexture = new CDepthStencilTexture;
  pDepthStencilTexture->TextureSize = _TextureSize;
  pDepthStencilTexture->Format = _Format;
  pDepthStencilTexture->VkFormat = Cast(_Format);
  if (!(PhysicalDevice_.getFormatProperties(pDepthStencilTexture->VkFormat).optimalTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment))
  {
    pDepthStencilTexture->VkFormat = EmulateFormat(_Format);
  }
  pDepthStencilTexture->ImageAspectFlag = vk::ImageAspectFlagBits::eDepth;
  if (hasStencil(pDepthStencilTexture->Format))
  {
    pDepthStencilTexture->ImageAspectFlag |= vk::ImageAspectFlagBits::eStencil;
  }

  vk::ImageCreateInfo ImageInfo;
  {
    ImageInfo.imageType = vk::ImageType::e2D;
    ImageInfo.format = pDepthStencilTexture->VkFormat;
    ImageInfo.extent = { pDepthStencilTexture->TextureSize.x, pDepthStencilTexture->TextureSize.y, 1 };
    ImageInfo.mipLevels = 1;
    ImageInfo.arrayLayers = 1;
    ImageInfo.samples = vk::SampleCountFlagBits::e1;
    ImageInfo.tiling = vk::ImageTiling::eOptimal;
    ImageInfo.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst;
    ImageInfo.sharingMode = vk::SharingMode::eExclusive;
    ImageInfo.initialLayout = vk::ImageLayout::eUndefined;
  }
  CreateImage(&pDepthStencilTexture->TextureData, ImageInfo);

  vk::ImageSubresourceRange SubresourceRange;
  {
    SubresourceRange.aspectMask = pDepthStencilTexture->ImageAspectFlag;
    SubresourceRange.baseMipLevel = 0;
    SubresourceRange.levelCount = 1;
    SubresourceRange.baseArrayLayer = 0;
    SubresourceRange.layerCount = 1;
  }

  CreateImageView(&pDepthStencilTexture->TextureData, pDepthStencilTexture->VkFormat, SubresourceRange, vk::ImageViewType::e2D);

  //  レイアウトの変更
  {
    CommandBuffer_->begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
    pDepthStencilTexture->TextureData.SetImageLayout(CommandBuffer_.get(), vk::ImageLayout::eDepthStencilAttachmentOptimal, SubresourceRange);
    CommandBuffer_->end();

    vk::SubmitInfo SubmitInfo;
    {
      SubmitInfo.commandBufferCount = 1;
      SubmitInfo.pCommandBuffers = &CommandBuffer_.get();
    }

    SubmitAndWait(SubmitInfo);
  }

  (*_ppDepthStecilTexture) = std::move(pDepthStencilTexture);
}

void CDevice::CreateUnorderedAccessTexture(ITexture** _ppUnorderedAccessTexture, const vdl::uint2& _TextureSize, vdl::FormatType _Format)
{
  assert(_ppUnorderedAccessTexture);

  CUnorderedAccessTexture* pUnorderedAccessTexture = new CUnorderedAccessTexture;
  pUnorderedAccessTexture->TextureSize = _TextureSize;
  pUnorderedAccessTexture->Format = _Format;
  vk::Format VkFormat = Cast(pUnorderedAccessTexture->Format);
  if (!(PhysicalDevice_.getFormatProperties(VkFormat).optimalTilingFeatures & vk::FormatFeatureFlagBits::eStorageImage))
  {
    VkFormat = EmulateFormat(_Format);
  }

  vk::ImageCreateInfo ImageInfo;
  {
    ImageInfo.imageType = vk::ImageType::e2D;
    ImageInfo.format = VkFormat;
    ImageInfo.extent = { pUnorderedAccessTexture->TextureSize.x, pUnorderedAccessTexture->TextureSize.y, 1 };
    ImageInfo.mipLevels = 1;
    ImageInfo.arrayLayers = 1;
    ImageInfo.samples = vk::SampleCountFlagBits::e1;
    ImageInfo.tiling = vk::ImageTiling::eOptimal;
    ImageInfo.usage = vk::ImageUsageFlagBits::eStorage | vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst;
    ImageInfo.sharingMode = vk::SharingMode::eExclusive;
    ImageInfo.initialLayout = vk::ImageLayout::eUndefined;
  }
  CreateImage(&pUnorderedAccessTexture->TextureData, ImageInfo);

  vk::ImageSubresourceRange SubresourceRange;
  {
    SubresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    SubresourceRange.baseMipLevel = 0;
    SubresourceRange.levelCount = 1;
    SubresourceRange.baseArrayLayer = 0;
    SubresourceRange.layerCount = 1;
  }

  CreateImageView(&pUnorderedAccessTexture->TextureData, VkFormat, SubresourceRange, vk::ImageViewType::e2D);

  //  レイアウトの変更
  {
    CommandBuffer_->begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
    pUnorderedAccessTexture->TextureData.SetImageLayout(CommandBuffer_.get(), vk::ImageLayout::eShaderReadOnlyOptimal, SubresourceRange);
    CommandBuffer_->end();

    vk::SubmitInfo SubmitInfo;
    {
      SubmitInfo.commandBufferCount = 1;
      SubmitInfo.pCommandBuffers = &CommandBuffer_.get();
    }

    SubmitAndWait(SubmitInfo);
  }

  (*_ppUnorderedAccessTexture) = std::move(pUnorderedAccessTexture);
}

void CDevice::WriteMemory(IBuffer* _pDstBuffer, const void* _pSrcBuffer, vdl::uint _BufferSize)const
{
  switch (_pDstBuffer->GetType())
  {
  case BufferType::eVertexBuffer:
  {
    CVertexBuffer* pVertexBuffer = static_cast<CVertexBuffer*>(_pDstBuffer);
    ::memcpy(pVertexBuffer->BufferData.pData, _pSrcBuffer, _BufferSize);
  }
  break;
  case BufferType::eInstanceBuffer:
  {
    CInstanceBuffer* pInstanceBuffer = static_cast<CInstanceBuffer*>(_pDstBuffer);
    if (pInstanceBuffer->Offset + _BufferSize > pInstanceBuffer->BufferSize)
    {
      pInstanceBuffer->Offset = 0;
    }
    ::memcpy(static_cast<vdl::uint8_t*>(pInstanceBuffer->BufferData.pData) + pInstanceBuffer->Offset, _pSrcBuffer, _BufferSize);

    pInstanceBuffer->PreviousOffset = pInstanceBuffer->Offset;
    pInstanceBuffer->Offset += _BufferSize;
  }
  break;
  case BufferType::eIndexBuffer:
  {
    CIndexBuffer* pIndexBuffer = static_cast<CIndexBuffer*>(_pDstBuffer);
    ::memcpy(pIndexBuffer->BufferData.pData, _pSrcBuffer, _BufferSize);
  }
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
    pHullShader->ShaderData.Module = VkDevice_->createShaderModuleUnique(ShaderModuleInfo);

    (*_ppShader) = std::move(pHullShader);
  }
  break;
  case ShaderType::eDomainShader:
  {
    CDomainShader* pDomainShader = new CDomainShader;
    pDomainShader->ShaderData.EntryPoint = _EntryPoint;
    pDomainShader->ShaderData.Module = VkDevice_->createShaderModuleUnique(ShaderModuleInfo);

    (*_ppShader) = std::move(pDomainShader);
  }
  break;
  case ShaderType::eGeometryShader:
  {
    CGeometryShader* pGeometryShader = new CGeometryShader;
    pGeometryShader->ShaderData.EntryPoint = _EntryPoint;
    pGeometryShader->ShaderData.Module = VkDevice_->createShaderModuleUnique(ShaderModuleInfo);

    (*_ppShader) = std::move(pGeometryShader);
  }
  break;
  case ShaderType::ePixelShader:
  {
    CPixelShader* pPixelShader = new CPixelShader;
    pPixelShader->ShaderData.EntryPoint = _EntryPoint;
    pPixelShader->ShaderData.Module = VkDevice_->createShaderModuleUnique(ShaderModuleInfo);

    (*_ppShader) = std::move(pPixelShader);
  }
  break;
  case ShaderType::eComputeShader:
  {
    CComputeShader* pComputeShader = new CComputeShader;
    pComputeShader->ShaderData.EntryPoint = _EntryPoint;
    pComputeShader->ShaderData.Module = VkDevice_->createShaderModuleUnique(ShaderModuleInfo);

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
    pHullShader->ShaderData.Module = VkDevice_->createShaderModuleUnique(ShaderModuleInfo);

    (*_ppShader) = std::move(pHullShader);
  }
  break;
  case ShaderType::eDomainShader:
  {
    CDomainShader* pDomainShader = new CDomainShader;
    pDomainShader->ShaderData.EntryPoint = _EntryPoint;
    pDomainShader->ShaderData.Module = VkDevice_->createShaderModuleUnique(ShaderModuleInfo);

    (*_ppShader) = std::move(pDomainShader);
  }
  break;
  case ShaderType::eGeometryShader:
  {
    CGeometryShader* pGeometryShader = new CGeometryShader;
    pGeometryShader->ShaderData.EntryPoint = _EntryPoint;
    pGeometryShader->ShaderData.Module = VkDevice_->createShaderModuleUnique(ShaderModuleInfo);

    (*_ppShader) = std::move(pGeometryShader);
  }
  break;
  case ShaderType::ePixelShader:
  {
    CPixelShader* pPixelShader = new CPixelShader;
    pPixelShader->ShaderData.EntryPoint = _EntryPoint;
    pPixelShader->ShaderData.Module = VkDevice_->createShaderModuleUnique(ShaderModuleInfo);

    (*_ppShader) = std::move(pPixelShader);
  }
  break;
  case ShaderType::eComputeShader:
  {
    CComputeShader* pComputeShader = new CComputeShader;
    pComputeShader->ShaderData.EntryPoint = _EntryPoint;
    pComputeShader->ShaderData.Module = VkDevice_->createShaderModuleUnique(ShaderModuleInfo);

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
  pVertexShader->ShaderData.Module = VkDevice_->createShaderModuleUnique(ShaderModuleInfo);

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
  pVertexShader->ShaderData.Module = VkDevice_->createShaderModuleUnique(ShaderModuleInfo);

  (*_ppVertexShader) = std::move(pVertexShader);
}

void CDevice::WaitIdle()
{
  VkDevice_->waitIdle();
}

//--------------------------------------------------

vdl::uint CDevice::FindQueue(const vk::QueueFlags& _QueueFlag, const vk::QueueFlags& _NotFlag, const vk::SurfaceKHR& _Surface)const
{
  std::vector<vk::QueueFamilyProperties> QueueFamilyProperties = PhysicalDevice_.getQueueFamilyProperties();
  const vdl::uint QueueFamiliyNum = static_cast<vdl::uint>(QueueFamilyProperties.size());

  for (vdl::uint QueueFamiliyCount = 0; QueueFamiliyCount < QueueFamiliyNum; ++QueueFamiliyCount)
  {
    if (QueueFamilyProperties[QueueFamiliyCount].queueFlags & _QueueFlag && !(QueueFamilyProperties[QueueFamiliyCount].queueFlags & _NotFlag))
    {
      //  サーフェスのサポートがされていない時は続ける
      if (_Surface && !PhysicalDevice_.getSurfaceSupportKHR(QueueFamiliyCount, _Surface))
      {
        continue;
      }

      return QueueFamiliyCount;
    }
  }

  //_ASSERT_EXPR_A(false, "Not Found.");

  return UINT_MAX;
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

void CDevice::SubmitAndWait(vk::SubmitInfo _SubmitInfo)const
{
  GraphicsQueue_.submit(_SubmitInfo, vk::Fence());
  GraphicsQueue_.waitIdle();
}

//--------------------------------------------------

void CDevice::CreateBuffer(BufferData* _pBuffer, vk::DeviceSize _BufferSize, const vk::BufferUsageFlags& _Usage, const vk::MemoryPropertyFlags& _Properties)const
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

    _pBuffer->Buffer = VkDevice_->createBufferUnique(BufferInfo);
    assert(_pBuffer->Buffer);
  }

  //  メモリの確保
  {
    const vk::MemoryRequirements MemoryRequirement = VkDevice_->getBufferMemoryRequirements(_pBuffer->Buffer.get());

    vk::MemoryAllocateInfo MemoryAllocateInfo;
    {
      MemoryAllocateInfo.allocationSize = MemoryRequirement.size;
      MemoryAllocateInfo.memoryTypeIndex = GetMemoryTypeIndex(MemoryRequirement.memoryTypeBits, _Properties);
    }
    _pBuffer->Memory = VkDevice_->allocateMemoryUnique(MemoryAllocateInfo);
    assert(_pBuffer->Memory);

    VkDevice_->bindBufferMemory(_pBuffer->Buffer.get(), _pBuffer->Memory.get(), 0);
  }

  //  ステージングしない時ポインタを取得する
  if (!(_Properties & vk::MemoryPropertyFlagBits::eDeviceLocal))
  {
    _pBuffer->pData = VkDevice_->mapMemory(_pBuffer->Memory.get(), 0, _BufferSize);
  }
}

void CDevice::CreateStagingBuffer(BufferData* _pStagingBuffer, const void* _Buffer, vdl::uint _BufferSize)const
{
  assert(_pStagingBuffer);

  CreateBuffer(_pStagingBuffer, _BufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

  //  メモリへ書き込み
  if (_Buffer)
  {
    ::memcpy(_pStagingBuffer->pData, _Buffer, _BufferSize);
  }
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

    SubmitAndWait(SubmitInfo);
  }
}

void CDevice::CreateImage(TextureData* _pTexture, const vk::ImageCreateInfo& _ImageInfo)const
{
  assert(_pTexture);

  //  イメージ作成
  _pTexture->Image = VkDevice_->createImageUnique(_ImageInfo);
  assert(_pTexture->Image);

  //  メモリの確保
  {
    vk::MemoryRequirements MemoryRequirement = VkDevice_->getImageMemoryRequirements(_pTexture->Image.get());

    vk::MemoryAllocateInfo MemoryAllocateInfo;
    {
      MemoryAllocateInfo.allocationSize = MemoryRequirement.size;
      MemoryAllocateInfo.memoryTypeIndex = GetMemoryTypeIndex(MemoryRequirement.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal);
    }

    _pTexture->Memory = VkDevice_->allocateMemoryUnique(MemoryAllocateInfo);
    assert(_pTexture->Memory);
    VkDevice_->bindImageMemory(_pTexture->Image.get(), _pTexture->Memory.get(), 0);
  }
}

void CDevice::CreateImageView(TextureData* _pTexture, vk::Format _Format, const vk::ImageSubresourceRange& _SubresourceRange, vk::ImageViewType _ViewType)const
{
  assert(_pTexture);

  vk::ImageViewCreateInfo ImageViewInfo;
  {
    ImageViewInfo.image = _pTexture->Image.get();
    ImageViewInfo.viewType = _ViewType;
    ImageViewInfo.format = _Format;
    ImageViewInfo.components.r = vk::ComponentSwizzle::eR;
    ImageViewInfo.components.g = vk::ComponentSwizzle::eG;
    ImageViewInfo.components.b = vk::ComponentSwizzle::eB;
    ImageViewInfo.components.a = vk::ComponentSwizzle::eA;
    ImageViewInfo.subresourceRange = _SubresourceRange;
  }

  _pTexture->View = VkDevice_->createImageViewUnique(ImageViewInfo);
  assert(_pTexture->View);
}
