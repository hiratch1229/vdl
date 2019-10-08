#include "CDeviceContext.hpp"

#include <vdl/Engine.hpp>
#include <vdl/Device/Vulkan/CDevice.hpp>
#include <vdl/SwapChain/Vulkan/CSwapChain.hpp>
#include <vdl/TextureManager/ITextureManager.hpp>
#include <vdl/BufferManager/IBufferManager.hpp>
#include <vdl/ShaderManager/IShaderManager.hpp>

#include <vdl/Topology/Vulkan/Topology.hpp>
#include <vdl/Scissor/Vulkan/Scissor.hpp>
#include <vdl/Viewport/Vulkan/Viewport.hpp>
#include <vdl/Buffer/Vulkan/CBuffer.hpp>
#include <vdl/Texture/Vulkan/CTexture.hpp>

#include <vdl/Instance/Instance.hpp>
#include <vdl/Constants/Constants.hpp>

#include <vdl/Vertex.hpp>

#include <ImGui/imgui.h>

namespace
{
  inline constexpr vk::ShaderStageFlagBits Cast(ShaderType _Type)
  {
    switch (_Type)
    {
    case ShaderType::eVertexShader:
      return vk::ShaderStageFlagBits::eVertex;
    case ShaderType::eHullShader:
      return vk::ShaderStageFlagBits::eTessellationControl;
    case ShaderType::eDomainShader:
      return vk::ShaderStageFlagBits::eTessellationEvaluation;
    case ShaderType::eGeometryShader:
      return vk::ShaderStageFlagBits::eGeometry;
    case ShaderType::ePixelShader:
      return vk::ShaderStageFlagBits::eFragment;
    case ShaderType::eComputeShader:
      return vk::ShaderStageFlagBits::eCompute;
    default: assert(false);
    }

    return vk::ShaderStageFlagBits::eVertex;
  }

  inline constexpr vk::DescriptorType Cast(DescriptorType _Type)
  {
    switch (_Type)
    {
    case DescriptorType::eSampler:
      return vk::DescriptorType::eSampler;
    case DescriptorType::eShaderResouce:
      return vk::DescriptorType::eSampledImage;
    case DescriptorType::eConstantBuffer:
      return vk::DescriptorType::eUniformBuffer;
    case DescriptorType::eUnorderedAccessTexture:
      return vk::DescriptorType::eStorageImage;
    case DescriptorType::eUnorderedAccessBuffer:
      return vk::DescriptorType::eStorageBuffer;
    default: assert(false);
    }

    return vk::DescriptorType::eSampler;
  }

  inline constexpr int GetPerCount(DescriptorType _Type)
  {
    switch (_Type)
    {
    case DescriptorType::eSampler:
      return Constants::kMaxSamplerNum;
    case DescriptorType::eShaderResouce:
      return Constants::kMaxShaderResourceNum;
    case DescriptorType::eConstantBuffer:
      return Constants::kMaxConstantBufferNum;
    case DescriptorType::eUnorderedAccessTexture:
    case DescriptorType::eUnorderedAccessBuffer:
      return Constants::kMaxTextureBatchNum;
    default: assert(false);
    }

    return 0;
  }

  inline vk::ClearColorValue Cast(const vdl::ColorF& _Color)
  {
    return std::array<float, 4>({ _Color.Red, _Color.Blue, _Color.Green, _Color.Alpha });
  }

  inline vk::PolygonMode Cast(vdl::FillModeType _Type)
  {
    switch (_Type)
    {
    case vdl::FillModeType::eWireframe:
      return vk::PolygonMode::eLine;
    case vdl::FillModeType::eSolid:
      return vk::PolygonMode::eFill;
    default: assert(false);
    }

    return vk::PolygonMode::eFill;
  }

  inline vk::CullModeFlags Cast(vdl::CullModeType _Type)
  {
    switch (_Type)
    {
    case vdl::CullModeType::eNone:
      return vk::CullModeFlagBits::eNone;
    case vdl::CullModeType::eFront:
      return vk::CullModeFlagBits::eFront;
    case vdl::CullModeType::eBack:
      return vk::CullModeFlagBits::eBack;
    default: assert(false);
    }

    return vk::CullModeFlagBits::eNone;
  }

  inline vk::CompareOp Cast(vdl::ComparisonFuncType _Type)
  {
    switch (_Type)
    {
    case vdl::ComparisonFuncType::eNever:
      return vk::CompareOp::eNever;
    case vdl::ComparisonFuncType::eLess:
      return vk::CompareOp::eLess;
    case vdl::ComparisonFuncType::eEqual:
      return vk::CompareOp::eEqual;
    case vdl::ComparisonFuncType::eLessEqual:
      return vk::CompareOp::eLessOrEqual;
    case vdl::ComparisonFuncType::eGreater:
      return vk::CompareOp::eGreater;
    case vdl::ComparisonFuncType::eNotEqual:
      return vk::CompareOp::eNotEqual;
    case vdl::ComparisonFuncType::eGreaterEqual:
      return vk::CompareOp::eGreaterOrEqual;
    case vdl::ComparisonFuncType::eAlways:
      return vk::CompareOp::eAlways;
    default: assert(false);
    }

    return vk::CompareOp::eNever;
  }

  inline vk::StencilOp Cast(vdl::StencilOpType _Type)
  {
    switch (_Type)
    {
    case vdl::StencilOpType::eKeep:
      return vk::StencilOp::eKeep;
    case vdl::StencilOpType::eZero:
      return vk::StencilOp::eZero;
    case vdl::StencilOpType::eRreplace:
      return vk::StencilOp::eReplace;
    case vdl::StencilOpType::eIncrSat:
      return vk::StencilOp::eIncrementAndWrap;
    case vdl::StencilOpType::eDecrSat:
      return vk::StencilOp::eDecrementAndWrap;
    case vdl::StencilOpType::eInvert:
      return vk::StencilOp::eInvert;
    case vdl::StencilOpType::eIncr:
      return vk::StencilOp::eIncrementAndClamp;
    case vdl::StencilOpType::eDecr:
      return vk::StencilOp::eIncrementAndClamp;
    default: assert(false);
    }

    return vk::StencilOp::eKeep;
  }

  inline vk::StencilOpState Cast(const vdl::DepthStencilOpState& _StencilOpState)
  {
    return { Cast(_StencilOpState.StencilFailOp), Cast(_StencilOpState.StencilPassOp),
      Cast(_StencilOpState.StencilDepthFailOp), Cast(_StencilOpState.StencilFunc), 0, 0, 1 };
  }

  inline vk::BlendFactor Cast(vdl::BlendType _Type)
  {
    switch (_Type)
    {
    case vdl::BlendType::eZero:
      return vk::BlendFactor::eZero;
    case vdl::BlendType::eOne:
      return vk::BlendFactor::eOne;
    case vdl::BlendType::eSrcColor:
      return vk::BlendFactor::eSrcColor;
    case vdl::BlendType::eInvSrcColor:
      return vk::BlendFactor::eOneMinusSrcColor;
    case vdl::BlendType::eSrcAlpha:
      return vk::BlendFactor::eSrcAlpha;
    case vdl::BlendType::eInvSrcAlpha:
      return vk::BlendFactor::eOneMinusSrcAlpha;
    case vdl::BlendType::eDestAlpha:
      return vk::BlendFactor::eDstAlpha;
    case vdl::BlendType::eInvDestAlpha:
      return vk::BlendFactor::eOneMinusDstAlpha;
    case vdl::BlendType::eDestColor:
      return vk::BlendFactor::eOneMinusDstColor;
    case vdl::BlendType::eInvDestColor:
      return vk::BlendFactor::eOneMinusDstColor;
    case vdl::BlendType::eSrcAlphaSat:
      return vk::BlendFactor::eSrcAlphaSaturate;
    case vdl::BlendType::eSrc1Color:
      return vk::BlendFactor::eSrc1Color;
    case vdl::BlendType::eInvSrc1Color:
      return vk::BlendFactor::eOneMinusSrc1Color;
    case vdl::BlendType::eSrc1Alpha:
      return vk::BlendFactor::eSrc1Alpha;
    case vdl::BlendType::eInvSrc1Alpha:
      return vk::BlendFactor::eOneMinusSrc1Alpha;
    default: assert(false);
    }

    return vk::BlendFactor::eZero;
  }

  inline vk::BlendOp Cast(vdl::BlendOpType _Type)
  {
    switch (_Type)
    {
    case vdl::BlendOpType::eAdd:
      return vk::BlendOp::eAdd;
    case vdl::BlendOpType::eSubtract:
      return vk::BlendOp::eSubtract;
    case vdl::BlendOpType::eRevSubtract:
      return vk::BlendOp::eReverseSubtract;
    case vdl::BlendOpType::eMin:
      return vk::BlendOp::eMin;
    case vdl::BlendOpType::eMax:
      return vk::BlendOp::eMax;
    default: assert(false);
    }

    return vk::BlendOp::eAdd;
  }

  inline vk::ColorComponentFlags Cast(vdl::ColorWriteEnableType _Type)
  {
    switch (_Type)
    {
    case vdl::ColorWriteEnableType::eRed:
      return vk::ColorComponentFlagBits::eR;
    case vdl::ColorWriteEnableType::eGreen:
      return vk::ColorComponentFlagBits::eG;
    case vdl::ColorWriteEnableType::eBlue:
      return vk::ColorComponentFlagBits::eB;
    case vdl::ColorWriteEnableType::eAlpha:
      return vk::ColorComponentFlagBits::eA;
    case vdl::ColorWriteEnableType::eAll:
      return vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
    default: assert(false);
    }

    return vk::ColorComponentFlagBits::eR;
  }
}

void CDeviceContext::Initialize()
{
  pSwapChain_ = static_cast<CSwapChain*>(Engine::Get<ISwapChain>());
  pTextureManager_ = Engine::Get<ITextureManager>();
  pBufferManager_ = Engine::Get<IBufferManager>();
  pShaderManager_ = Engine::Get<IShaderManager>();

  CDevice* pDevice = static_cast<CDevice*>(Engine::Get<IDevice>());
  VkDevice_ = pDevice->GetDevice();
  GraphicsQueue_ = pDevice->GetGraphicsQueue();
  ComputeQueue_ = pDevice->GetComputeQueue();

  //  入力レイアウトの作成
  {
    vdl::uint Offset;
    vdl::uint Location;
    vdl::uint BindDescriptionCount;

    auto SetVertexInputAttibuteDescription = [&Offset, &Location, &BindDescriptionCount]
    (InputLayout* _pInputLayout, vk::Format _Format, vdl::uint _Size)->void
    {
      vk::VertexInputAttributeDescription& VertexInputAttributeDescription = _pInputLayout->VertexInputAttributeDescriptions[Location];
      {
        VertexInputAttributeDescription.binding = _pInputLayout->VertexInputBindingDescriptions[BindDescriptionCount].binding;
        VertexInputAttributeDescription.location = Location;
        VertexInputAttributeDescription.format = _Format;
        VertexInputAttributeDescription.offset = Offset;
      }

      Offset += _Size;
      ++Location;
    };

    //  None
    {
      InputLayout InputLayout;

      InputLayouts_.insert(std::make_pair(vdl::InputLayoutType::eNone, std::move(InputLayout)));
    }

    //  Texture
    {
      Offset = Location = BindDescriptionCount = 0;

      InputLayout InputLayout;
      {
        InputLayout.VertexInputBindingDescriptions.resize(2);
        InputLayout.VertexInputAttributeDescriptions.resize(9);

        //  頂点データ
        {
          InputLayout.VertexInputBindingDescriptions[0].binding = 0;
          InputLayout.VertexInputBindingDescriptions[0].stride = sizeof(vdl::TextureVertex);
          InputLayout.VertexInputBindingDescriptions[0].inputRate = vk::VertexInputRate::eVertex;

          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32Sfloat, sizeof(vdl::TextureVertex::Position));
          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32Sfloat, sizeof(vdl::TextureVertex::Texcoord));
        }

        ++BindDescriptionCount;
        Offset = 0;

        //  インスタンスデータ
        {
          InputLayout.VertexInputBindingDescriptions[1].binding = 1;
          InputLayout.VertexInputBindingDescriptions[1].stride = sizeof(TextureInstanceData);
          InputLayout.VertexInputBindingDescriptions[1].inputRate = vk::VertexInputRate::eInstance;

          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32B32A32Sfloat, sizeof(TextureInstanceData::NDCTransform) / 4);
          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32B32A32Sfloat, sizeof(TextureInstanceData::NDCTransform) / 4);
          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32B32A32Sfloat, sizeof(TextureInstanceData::NDCTransform) / 4);
          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32B32A32Sfloat, sizeof(TextureInstanceData::NDCTransform) / 4);
          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32Sfloat, sizeof(TextureInstanceData::TexcoordScale));
          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32Sfloat, sizeof(TextureInstanceData::TexcoordTranslate));
          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32B32A32Sfloat, sizeof(TextureInstanceData::Color));
        }
      }

      InputLayouts_.insert(std::make_pair(vdl::InputLayoutType::eTexture, std::move(InputLayout)));
    }

    //  StaticMesh
    {
      Offset = Location = BindDescriptionCount = 0;

      InputLayout InputLayout;
      {
        InputLayout.VertexInputBindingDescriptions.resize(2);
        InputLayout.VertexInputAttributeDescriptions.resize(9);

        //  頂点データ
        {
          InputLayout.VertexInputBindingDescriptions[0].binding = 0;
          InputLayout.VertexInputBindingDescriptions[0].stride = sizeof(vdl::StaticMeshVertex);
          InputLayout.VertexInputBindingDescriptions[0].inputRate = vk::VertexInputRate::eVertex;

          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32B32Sfloat, sizeof(vdl::StaticMeshVertex::Position));
          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32B32Sfloat, sizeof(vdl::StaticMeshVertex::Normal));
          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32B32Sfloat, sizeof(vdl::StaticMeshVertex::Tangent));
          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32Sfloat, sizeof(vdl::StaticMeshVertex::Texcoord));
        }

        ++BindDescriptionCount;
        Offset = 0;

        //  インスタンスデータ
        {
          InputLayout.VertexInputBindingDescriptions[1].binding = 1;
          InputLayout.VertexInputBindingDescriptions[1].stride = sizeof(StaticMeshInstanceData);
          InputLayout.VertexInputBindingDescriptions[1].inputRate = vk::VertexInputRate::eInstance;

          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32B32A32Sfloat, sizeof(StaticMeshInstanceData::World) / 4);
          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32B32A32Sfloat, sizeof(StaticMeshInstanceData::World) / 4);
          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32B32A32Sfloat, sizeof(StaticMeshInstanceData::World) / 4);
          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32B32A32Sfloat, sizeof(StaticMeshInstanceData::World) / 4);
          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32B32A32Sfloat, sizeof(StaticMeshInstanceData::Color));        }
      }

      InputLayouts_.insert(std::make_pair(vdl::InputLayoutType::eStaticMesh, std::move(InputLayout)));
    }

    //  SkinnedMesh
    {
      Offset = Location = BindDescriptionCount = 0;

      InputLayout InputLayout;
      {
        InputLayout.VertexInputBindingDescriptions.resize(2);
        InputLayout.VertexInputAttributeDescriptions.resize(11);

        //  頂点データ
        {
          InputLayout.VertexInputBindingDescriptions[0].binding = 0;
          InputLayout.VertexInputBindingDescriptions[0].stride = sizeof(vdl::SkinnedMeshVertex);
          InputLayout.VertexInputBindingDescriptions[0].inputRate = vk::VertexInputRate::eVertex;

          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32B32Sfloat, sizeof(vdl::SkinnedMeshVertex::Position));
          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32B32Sfloat, sizeof(vdl::SkinnedMeshVertex::Normal));
          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32B32Sfloat, sizeof(vdl::SkinnedMeshVertex::Tangent));
          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32Sfloat, sizeof(vdl::SkinnedMeshVertex::Texcoord));
          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32B32A32Sfloat, sizeof(vdl::SkinnedMeshVertex::BoneWeights));
          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32B32A32Uint, sizeof(vdl::SkinnedMeshVertex::BoneIndices));
        }

        ++BindDescriptionCount;
        Offset = 0;

        //  インスタンスデータ
        {
          InputLayout.VertexInputBindingDescriptions[1].binding = 1;
          InputLayout.VertexInputBindingDescriptions[1].stride = sizeof(StaticMeshInstanceData);
          InputLayout.VertexInputBindingDescriptions[1].inputRate = vk::VertexInputRate::eInstance;

          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32B32A32Sfloat, sizeof(StaticMeshInstanceData::World) / 4);
          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32B32A32Sfloat, sizeof(StaticMeshInstanceData::World) / 4);
          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32B32A32Sfloat, sizeof(StaticMeshInstanceData::World) / 4);
          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32B32A32Sfloat, sizeof(StaticMeshInstanceData::World) / 4);
          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32B32A32Sfloat, sizeof(StaticMeshInstanceData::Color));        }
      }

      InputLayouts_.insert(std::make_pair(vdl::InputLayoutType::eSkinnedMesh, std::move(InputLayout)));
    }

    //  GUI
    {
      Offset = Location = BindDescriptionCount = 0;

      InputLayout InputLayout;
      {
        InputLayout.VertexInputBindingDescriptions.resize(1);
        InputLayout.VertexInputAttributeDescriptions.resize(3);

        //  頂点データ
        {
          InputLayout.VertexInputBindingDescriptions[0].binding = 0;
          InputLayout.VertexInputBindingDescriptions[0].stride = sizeof(ImDrawVert);
          InputLayout.VertexInputBindingDescriptions[0].inputRate = vk::VertexInputRate::eVertex;

          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32Sfloat, sizeof(ImDrawVert::pos));
          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32Sfloat, sizeof(ImDrawVert::uv));
          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR8G8B8A8Unorm, sizeof(ImDrawVert::col));
        }

        InputLayouts_.insert(std::make_pair(vdl::InputLayoutType::eGUI, std::move(InputLayout)));
      }
    }
  }

  //  グラフィックス用オブジェクトの作成
  {
    //  デスクリプタセットレイアウトの作成
    {
      std::array<std::vector<vk::DescriptorSetLayoutBinding>, kGraphicsDescriptorLayoutNum> LayoutBindings;

      for (vdl::uint i = 0; i < kGraphicsDescriptorTypeNum; ++i)
      {
        const vdl::uint PerCountNum = GetPerCount(static_cast<DescriptorType>(i));
        const vk::DescriptorType Type = Cast(static_cast<DescriptorType>(i));
        for (vdl::uint j = 0; j < kGraphicsShaderStageNum; ++j)
        {
          std::vector<vk::DescriptorSetLayoutBinding>& LayoutBining = LayoutBindings[i * kGraphicsShaderStageNum + j];
          LayoutBining.resize(PerCountNum);
          for (vdl::uint k = 0; k < PerCountNum; ++k)
          {
            LayoutBining[k].binding = k;
            LayoutBining[k].descriptorType = Type;
            LayoutBining[k].descriptorCount = 1;
            LayoutBining[k].stageFlags = Cast(static_cast<ShaderType>(j));
            LayoutBining[k].pImmutableSamplers = nullptr;
          }
        }
      }

      vk::DescriptorSetLayoutCreateInfo DescriptorSetLayoutInfo;
      for (vdl::uint i = 0; i < kGraphicsDescriptorLayoutNum; ++i)
      {
        DescriptorSetLayoutInfo.bindingCount = static_cast<vdl::uint>(LayoutBindings[i].size());
        DescriptorSetLayoutInfo.pBindings = LayoutBindings[i].data();

        GraphicsDescriptorLayouts_[i] = VkDevice_.createDescriptorSetLayoutUnique(DescriptorSetLayoutInfo);
        assert(GraphicsDescriptorLayouts_[i]);
      }
    }

    //  パイプラインキャッシュの作成
    {
      GraphicsPipelineCache_ = VkDevice_.createPipelineCacheUnique(vk::PipelineCacheCreateInfo());
      assert(GraphicsPipelineCache_);
    }

    //  コマンドバッファの作成
    {
      vk::CommandPoolCreateInfo CommandPoolInfo;
      {
        CommandPoolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
        CommandPoolInfo.queueFamilyIndex = pDevice->GetGraphicsQueueIndex();
      }

      GraphicsCommandPool_ = VkDevice_.createCommandPoolUnique(CommandPoolInfo);
      assert(GraphicsCommandPool_);

      vk::CommandBufferAllocateInfo CommandBufferInfo;
      {
        CommandBufferInfo.commandPool = GraphicsCommandPool_.get();
        CommandBufferInfo.level = vk::CommandBufferLevel::ePrimary;
        CommandBufferInfo.commandBufferCount = kGraphicsCommandBufferNum;
      }

      std::vector<vk::UniqueCommandBuffer> GraphicsCommandBuffers = VkDevice_.allocateCommandBuffersUnique(CommandBufferInfo);
      assert(!GraphicsCommandBuffers.empty());

      for (vdl::uint i = 0; i < kGraphicsCommandBufferNum; ++i)
      {
        GraphicsCommandBuffers_[i] = std::move(GraphicsCommandBuffers[i]);
      }
    }

    GraphicsPipelines_[GraphicsCommandBufferIndex_].resize(1);
  }

  //  コンピュート用オブジェクトの作成
  {
    //  デスクリプタセットレイアウトの作成
    {
      std::array<std::vector<vk::DescriptorSetLayoutBinding>, kComputeDescriptorLayoutNum> LayoutBindings;

      for (vdl::uint i = 0; i < kComputeDescriptorTypeNum; ++i)
      {
        const vdl::uint PerCount = GetPerCount(static_cast<DescriptorType>(i));
        const vk::DescriptorType Type = Cast(static_cast<DescriptorType>(i));

        std::vector<vk::DescriptorSetLayoutBinding>& LayoutBining = LayoutBindings[i];
        LayoutBining.resize(PerCount);

        for (vdl::uint j = 0; j < PerCount; ++j)
        {
          LayoutBining[j].binding = j;
          LayoutBining[j].descriptorType = Type;
          LayoutBining[j].descriptorCount = 1;
          LayoutBining[j].stageFlags = vk::ShaderStageFlagBits::eCompute;
          LayoutBining[j].pImmutableSamplers = nullptr;
        }
      }

      vk::DescriptorSetLayoutCreateInfo DescriptorSetLayoutInfo;
      for (vdl::uint i = 0; i < kComputeDescriptorTypeNum; ++i)
      {
        DescriptorSetLayoutInfo.bindingCount = static_cast<vdl::uint>(LayoutBindings[i].size());
        DescriptorSetLayoutInfo.pBindings = LayoutBindings[i].data();

        ComputeDescriptorLayouts_[i] = VkDevice_.createDescriptorSetLayoutUnique(DescriptorSetLayoutInfo);
        assert(ComputeDescriptorLayouts_[i]);
      }
    }

    //  パイプラインキャッシュの作成
    {
      ComputePipelineCache_ = VkDevice_.createPipelineCacheUnique(vk::PipelineCacheCreateInfo());
      assert(ComputePipelineCache_);
    }

    //  コマンドバッファの作成
    {
      vk::CommandPoolCreateInfo CommandPoolInfo;
      {
        CommandPoolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
        CommandPoolInfo.queueFamilyIndex = pDevice->GetComputeQueueIndex();
      }

      ComputeCommandPool_ = VkDevice_.createCommandPoolUnique(CommandPoolInfo);
      assert(ComputeCommandPool_);

      vk::CommandBufferAllocateInfo CommandBufferInfo;
      {
        CommandBufferInfo.commandPool = ComputeCommandPool_.get();
        CommandBufferInfo.level = vk::CommandBufferLevel::ePrimary;
        CommandBufferInfo.commandBufferCount = 1;
      }

      ComputeCommandBuffer_ = std::move(VkDevice_.allocateCommandBuffersUnique(CommandBufferInfo).front());
      assert(ComputeCommandBuffer_);
    }
  }

  //  パイプラインレイアウトの作成
  {
    std::array<vk::DescriptorSetLayout, kGraphicsDescriptorLayoutNum + kComputeDescriptorTypeNum> DescriptorSetLayouts;
    {
      for (vdl::uint i = 0; i < kGraphicsDescriptorLayoutNum; ++i)
      {
        DescriptorSetLayouts[i] = GraphicsDescriptorLayouts_[i].get();
      }
      for (vdl::uint i = 0; i < kComputeDescriptorTypeNum; ++i)
      {
        DescriptorSetLayouts[kGraphicsDescriptorLayoutNum + i] = ComputeDescriptorLayouts_[i].get();
      }
    }

    vk::PipelineLayoutCreateInfo PipelineLayoutCreateInfo;
    {
      PipelineLayoutCreateInfo.setLayoutCount = static_cast<vdl::uint>(DescriptorSetLayouts.size());
      PipelineLayoutCreateInfo.pSetLayouts = DescriptorSetLayouts.data();
    }

    PipelineLayout_ = VkDevice_.createPipelineLayoutUnique(PipelineLayoutCreateInfo);
    assert(PipelineLayout_);
  }
}

#pragma region GraphicsPipeline
#define SetGraphicsState(GraphicsCommandType, StateName)\
if (!GraphicsStateChangeFlags_.Has(GraphicsCommandType) && CurrentGraphicsState_.StateName != _##StateName)\
{\
  GraphicsStateChangeFlags_.Set(GraphicsCommandType);\
}\
\
CurrentGraphicsState_.StateName = _##StateName;

#define SetGraphicsShaderStates(GraphicsCommandType, ShaderType, ShaderState)\
ShaderState##s& Current##ShaderState##s = CurrentGraphicsState_.ShaderState##s[static_cast<vdl::uint>(ShaderType)];\
\
if (const vdl::uint RequiredSize = _StartSlot + _##ShaderState##Num;\
  Current##ShaderState##s.size() < RequiredSize)\
{\
  Current##ShaderState##s.resize(static_cast<size_t>(RequiredSize));\
  GraphicsStateChangeFlags_.Set(GraphicsCommandType);\
}\
\
if (!GraphicsStateChangeFlags_.Has(GraphicsCommandType))\
{\
  for (vdl::uint ShaderState##Count = 0; ShaderState##Count < _##ShaderState##Num; ++ShaderState##Count)\
  {\
    auto& Current##ShaderState = Current##ShaderState##s[_StartSlot + ShaderState##Count];\
    const auto& ShaderState = _##ShaderState##s[ShaderState##Count];\
    \
    if (Current##ShaderState != ShaderState)\
    {\
      GraphicsStateChangeFlags_.Set(GraphicsCommandType);\
      break;\
    }\
  }\
}\
\
for (vdl::uint ShaderState##Count = 0; ShaderState##Count < _##ShaderState##Num; ++ShaderState##Count)\
{\
  Current##ShaderState##s[_StartSlot + ShaderState##Count] = _##ShaderState##s[ShaderState##Count];\
}

void CDeviceContext::SetVertexBuffer(const IBuffer* _pVertexBuffer)
{
  SetGraphicsState(GraphicsCommandType::eSetVertexBuffer, pVertexBuffer)
}

void CDeviceContext::SetInstanceBuffer(const IBuffer* _pInstanceBuffer)
{
  SetGraphicsState(GraphicsCommandType::eSetInstanceBuffer, pInstanceBuffer)
}

void CDeviceContext::SetIndexBuffer(const IBuffer* _pIndexBuffer)
{
  SetGraphicsState(GraphicsCommandType::eSetIndexBuffer, pIndexBuffer)
}

void CDeviceContext::SetInputLayout(vdl::InputLayoutType _InputLayout)
{
  SetGraphicsState(GraphicsCommandType::eSetInputLayout, InputLayout)
}

void CDeviceContext::SetTopology(vdl::TopologyType _Topology)
{
  SetGraphicsState(GraphicsCommandType::eSetTopology, Topology)
}

void CDeviceContext::SetScissor(const vdl::Scissor& _Scissor)
{
  SetGraphicsState(GraphicsCommandType::eSetScissor, Scissor)
}

void CDeviceContext::SetViewport(const vdl::Viewport& _Viewport)
{
  SetGraphicsState(GraphicsCommandType::eSetViewport, Viewport)
}

void CDeviceContext::SetRenderTextures(const vdl::RenderTextures& _RenderTextures, const vdl::DepthStencilTexture& _DepthStenilTexture)
{
  BeginGraphicsCommandBuffer();

  CurrentOutputManagers_[GraphicsCommandBufferIndex_] = { _RenderTextures, _DepthStenilTexture };

  RenderPassData& RenderPassData = RenderPassDatas_[GraphicsCommandBufferIndex_];

  const vdl::uint2 RenderTexture = _RenderTextures[0].GetSize();

  vdl::uint AttachmentCount = 0;
  std::array<vk::ImageView, Constants::kMaxRenderTextureNum + 1> Attachments;
  //  レンダーパスの作成
  {
    std::array<vk::AttachmentDescription, Constants::kMaxRenderTextureNum + 1> AttachmentDescriptions;
    std::array<vk::AttachmentReference, Constants::kMaxRenderTextureNum + 1> AttachmentReferences;
    {
      //  レンダーターゲットの設定
      {
        auto GetRenderTexture = [&](const vdl::RenderTexture& _RenderTexture)->CRenderTexture *
        {
          assert(!_RenderTexture.isEmpty());
          assert(RenderTexture == _RenderTexture.GetSize());

          return static_cast<CRenderTexture*>(pTextureManager_->GetTexture(_RenderTexture.GetID()));
        };
        auto SetAttachment = [&](CRenderTexture* _pRenderTexture)->void
        {
          constexpr vk::ImageLayout kImageLayout = vk::ImageLayout::eColorAttachmentOptimal;

          vk::AttachmentDescription& AttachmentDescription = AttachmentDescriptions[AttachmentCount];
          {
            AttachmentDescription.format = _pRenderTexture->Format;
            AttachmentDescription.samples = vk::SampleCountFlagBits::e1;
            AttachmentDescription.loadOp = vk::AttachmentLoadOp::eLoad;
            AttachmentDescription.storeOp = vk::AttachmentStoreOp::eStore;
            AttachmentDescription.initialLayout = _pRenderTexture->CurrentLayout;
            _pRenderTexture->CurrentLayout = AttachmentDescription.finalLayout = kImageLayout;
          }

          vk::AttachmentReference& AttachmentReference = AttachmentReferences[AttachmentCount];
          {
            AttachmentReference.attachment = AttachmentCount;
            AttachmentReference.layout = kImageLayout;
          }

          Attachments[AttachmentCount] = _pRenderTexture->View.get();
        };

        SetAttachment((_RenderTextures[0].isEmpty() ? pSwapChain_->GetRenderTexture() : GetRenderTexture(_RenderTextures[0])));
        ++AttachmentCount;

        for (; AttachmentCount < Constants::kMaxRenderTextureNum; ++AttachmentCount)
        {
          const vdl::RenderTexture RenderTexture = _RenderTextures[AttachmentCount];
          if (RenderTexture.isEmpty())
          {
            break;
          }

          SetAttachment(GetRenderTexture(RenderTexture));
        }
      }

      //  デプスステンシルバッファの設定
      {
        CDepthStencilTexture* pDepthStencilTexture = (_DepthStenilTexture.isEmpty() ? pSwapChain_->GetDepthStencilTexture()
          : static_cast<CDepthStencilTexture*>(pTextureManager_->GetTexture(_DepthStenilTexture.GetID())));

        constexpr vk::ImageLayout kImageLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

        vk::AttachmentDescription& AttachmentDescription = AttachmentDescriptions[AttachmentCount];
        {
          AttachmentDescription.format = pDepthStencilTexture->Format;
          AttachmentDescription.samples = vk::SampleCountFlagBits::e1;
          AttachmentDescription.stencilLoadOp = vk::AttachmentLoadOp::eLoad;
          AttachmentDescription.stencilStoreOp = vk::AttachmentStoreOp::eStore;
          AttachmentDescription.initialLayout = pDepthStencilTexture->CurrentLayout;
          pDepthStencilTexture->CurrentLayout = AttachmentDescription.finalLayout = kImageLayout;
        }

        vk::AttachmentReference& AttachmentReference = AttachmentReferences[AttachmentCount];
        {
          AttachmentReference.attachment = AttachmentCount;
          AttachmentReference.layout = kImageLayout;
        }

        Attachments[AttachmentCount] = pDepthStencilTexture->View.get();
        ++AttachmentCount;
      }
    }

    vk::SubpassDescription SubpassDescription;
    {
      SubpassDescription.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
      SubpassDescription.colorAttachmentCount = AttachmentCount - 1;
      SubpassDescription.pColorAttachments = AttachmentReferences.data();
      SubpassDescription.pDepthStencilAttachment = &AttachmentReferences[AttachmentCount];
    }

    vk::RenderPassCreateInfo RenderPassInfo;
    {
      RenderPassInfo.attachmentCount = AttachmentCount;
      RenderPassInfo.pAttachments = AttachmentDescriptions.data();
      RenderPassInfo.subpassCount = 1;
      RenderPassInfo.pSubpasses = &SubpassDescription;
    }

    RenderPassData.RenderPass = VkDevice_.createRenderPassUnique(RenderPassInfo);
    assert(RenderPassData.RenderPass);
  }

  //  フレームバッファの作成
  {
    vk::FramebufferCreateInfo FramebufferInfo;
    {
      FramebufferInfo.renderPass = RenderPassData.RenderPass.get();
      FramebufferInfo.attachmentCount = AttachmentCount;
      FramebufferInfo.pAttachments = Attachments.data();
      FramebufferInfo.width = RenderTexture.x;
      FramebufferInfo.height = RenderTexture.y;
      FramebufferInfo.layers = 1;
    }

    RenderPassData.Framebuffer = VkDevice_.createFramebufferUnique(FramebufferInfo);
    assert(RenderPassData.Framebuffer);
  }

  GetCurrentGraphicsCommandBuffer().beginRenderPass({ RenderPassData.RenderPass.get(), RenderPassData.Framebuffer.get(),
    { { 0, 0 }, { RenderTexture.x, RenderTexture.y } } }, vk::SubpassContents::eInline);
  GraphicsCommandBufferState_ = CommandBufferState::eBeginRenderPass;
}

void CDeviceContext::SetBlendState(const vdl::BlendState& _BlendState)
{
  SetGraphicsState(GraphicsCommandType::eSetBlendState, BlendState)
}

void CDeviceContext::SetDepthStencilState(const vdl::DepthStencilState& _DepthStencilState)
{
  SetGraphicsState(GraphicsCommandType::eSetDepthStencilState, DepthStencilState)
}

void CDeviceContext::SetRasterizerState(const vdl::RasterizerState& _RasterizerState)
{
  SetGraphicsState(GraphicsCommandType::eSetRasterizerState, RasterizerState)

    if (!GraphicsStateChangeFlags_.Has(GraphicsCommandType::eSetScissor) &&
      PreviousGraphicsState_.RasterizerState.ScissorEnable != _RasterizerState.ScissorEnable)
    {
      GraphicsStateChangeFlags_.Set(GraphicsCommandType::eSetScissor);
    }
}

void CDeviceContext::VSSetShader(const vdl::VertexShader& _VertexShader)
{
  SetGraphicsState(GraphicsCommandType::eSetVertexShader, VertexShader)
}

void CDeviceContext::VSSetShaderResources(vdl::uint _StartSlot, vdl::uint _ShaderResourceNum, const vdl::ShaderResource _ShaderResources[])
{
  SetGraphicsShaderStates(GraphicsCommandType::eSetVertexStageShaderResource, ShaderType::eVertexShader, ShaderResource)
}

void CDeviceContext::VSSetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[])
{
  SetGraphicsShaderStates(GraphicsCommandType::eSetVertexStageSampler, ShaderType::eVertexShader, Sampler)
}

void CDeviceContext::VSSetConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[])
{
  SetGraphicsShaderStates(GraphicsCommandType::eSetVertexStageConstantBuffer, ShaderType::eVertexShader, ConstantBuffer)
}

void CDeviceContext::HSSetShader(const vdl::HullShader& _HullShader)
{
  SetGraphicsState(GraphicsCommandType::eSetHullShader, HullShader)
}

void CDeviceContext::HSSetShaderResources(vdl::uint _StartSlot, vdl::uint _ShaderResourceNum, const vdl::ShaderResource _ShaderResources[])
{
  SetGraphicsShaderStates(GraphicsCommandType::eSetHullStageShaderResource, ShaderType::eHullShader, ShaderResource)
}

void CDeviceContext::HSSetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[])
{
  SetGraphicsShaderStates(GraphicsCommandType::eSetHullStageSampler, ShaderType::eHullShader, Sampler)
}

void CDeviceContext::HSSetConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[])
{
  SetGraphicsShaderStates(GraphicsCommandType::eSetHullStageConstantBuffer, ShaderType::eHullShader, ConstantBuffer)
}

void CDeviceContext::DSSetShader(const vdl::DomainShader& _DomainShader)
{
  SetGraphicsState(GraphicsCommandType::eSetDomainShader, DomainShader)
}

void CDeviceContext::DSSetShaderResources(vdl::uint _StartSlot, vdl::uint _ShaderResourceNum, const vdl::ShaderResource _ShaderResources[])
{
  SetGraphicsShaderStates(GraphicsCommandType::eSetDomainStageShaderResource, ShaderType::eDomainShader, ShaderResource)
}

void CDeviceContext::DSSetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[])
{
  SetGraphicsShaderStates(GraphicsCommandType::eSetDomainStageSampler, ShaderType::eDomainShader, Sampler)
}

void CDeviceContext::DSSetConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[])
{
  SetGraphicsShaderStates(GraphicsCommandType::eSetDomainStageConstantBuffer, ShaderType::eDomainShader, ConstantBuffer)
}

void CDeviceContext::GSSetShader(const vdl::GeometryShader& _GeometryShader)
{
  SetGraphicsState(GraphicsCommandType::eSetGeometryShader, GeometryShader)
}

void CDeviceContext::GSSetShaderResources(vdl::uint _StartSlot, vdl::uint _ShaderResourceNum, const vdl::ShaderResource _ShaderResources[])
{
  SetGraphicsShaderStates(GraphicsCommandType::eSetGeometryStageShaderResource, ShaderType::eGeometryShader, ShaderResource)
}

void CDeviceContext::GSSetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[])
{
  SetGraphicsShaderStates(GraphicsCommandType::eSetGeometryStageSampler, ShaderType::eGeometryShader, Sampler)
}

void CDeviceContext::GSSetConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[])
{
  SetGraphicsShaderStates(GraphicsCommandType::eSetGeometryStageConstantBuffer, ShaderType::eGeometryShader, ConstantBuffer)
}

void CDeviceContext::PSSetShader(const vdl::PixelShader& _PixelShader)
{
  SetGraphicsState(GraphicsCommandType::eSetPixelShader, PixelShader)
}

void CDeviceContext::PSSetShaderResources(vdl::uint _StartSlot, vdl::uint _ShaderResourceNum, const vdl::ShaderResource _ShaderResources[])
{
  SetGraphicsShaderStates(GraphicsCommandType::eSetPixelStageShaderResource, ShaderType::ePixelShader, ShaderResource)
}

void CDeviceContext::PSSetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[])
{
  SetGraphicsShaderStates(GraphicsCommandType::eSetPixelStageSampler, ShaderType::ePixelShader, Sampler)
}

void CDeviceContext::PSSetConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[])
{
  SetGraphicsShaderStates(GraphicsCommandType::eSetPixelStageConstantBuffer, ShaderType::ePixelShader, ConstantBuffer)
}

#undef SetGraphicsShaderStates
#undef SetGraphicsState
#pragma endregion

#pragma region ComputePipeline
void CDeviceContext::CSSetShader(const vdl::ComputeShader& _ComputeShader)
{

}

void CDeviceContext::CSSetShaderResources(vdl::uint _StartSlot, vdl::uint _ShaderResourceNum, const vdl::ShaderResource _ShaderResources[])
{

}

void CDeviceContext::CSSetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[])
{

}

void CDeviceContext::CSSetConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[])
{

}

void CDeviceContext::CSSetUnorderedAccessObjects(vdl::uint _StartSlot, vdl::uint _UnorderedAccessObjectNum, const vdl::UnorderedAccessObject _UnorderedAccessObjects[])
{

}
#pragma endregion

void CDeviceContext::ClearRenderTexture(const vdl::RenderTexture& _RenderTexture, const vdl::ColorF& _ClearColor)
{
  constexpr vk::ImageLayout kImageLayout = vk::ImageLayout::eTransferDstOptimal;

  BeginGraphicsCommandBuffer();

  const vk::CommandBuffer& CurrentGraphicsCommandBuffer = GetCurrentGraphicsCommandBuffer();
  const vk::ImageSubresourceRange SubresourceRange = { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 };

  const vdl::ID TextureID = _RenderTexture.GetID();
  CTexture* pTexture = (TextureID ? static_cast<CTexture*>(pTextureManager_->GetTexture(TextureID)) : pSwapChain_->GetRenderTexture());
  {
    if (ClearTextures_.find(TextureID) == ClearTextures_.end())
    {
      ClearTextures_.insert(std::make_pair(TextureID, _RenderTexture));

      pTexture->SetImageLayout(CurrentGraphicsCommandBuffer, kImageLayout, SubresourceRange);
    }
  }

  CurrentGraphicsCommandBuffer.clearColorImage(pTexture->Image.get(), kImageLayout, Cast(_ClearColor), SubresourceRange);
}

void CDeviceContext::ClearDepthStencilTexture(const vdl::DepthStencilTexture& _DepthStencilTexture, float _ClearDepth, vdl::uint _ClearStencil)
{
  constexpr vk::ImageLayout kImageLayout = vk::ImageLayout::eTransferDstOptimal;

  BeginGraphicsCommandBuffer();

  const vk::CommandBuffer& CurrentGraphicsCommandBuffer = GetCurrentGraphicsCommandBuffer();
  const vk::ImageSubresourceRange SubresourceRange = { vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil, 0, 1, 0, 1 };

  const vdl::ID TextureID = _DepthStencilTexture.GetID();
  CTexture* pTexture = (TextureID ? static_cast<CTexture*>(pTextureManager_->GetTexture(TextureID)) : pSwapChain_->GetDepthStencilTexture());
  {
    if (ClearTextures_.find(TextureID) == ClearTextures_.end())
    {
      ClearTextures_.insert(std::make_pair(TextureID, _DepthStencilTexture));

      pTexture->SetImageLayout(CurrentGraphicsCommandBuffer, kImageLayout, SubresourceRange);
    }
  }

  CurrentGraphicsCommandBuffer.clearDepthStencilImage(pTexture->Image.get(), kImageLayout, { _ClearDepth, _ClearStencil }, SubresourceRange);
}

void CDeviceContext::ClearUnorderedAccessTexture(const vdl::UnorderedAccessTexture& _UnorderedAccessTexture, const vdl::ColorF& _ClearColor)
{
  assert(!_UnorderedAccessTexture.isEmpty());

  constexpr vk::ImageLayout kImageLayout = vk::ImageLayout::eTransferDstOptimal;

  BeginGraphicsCommandBuffer();

  const vk::CommandBuffer& CurrentGraphicsCommandBuffer = GetCurrentGraphicsCommandBuffer();
  const vk::ImageSubresourceRange SubresourceRange = { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 };

  const vdl::ID TextureID = _UnorderedAccessTexture.GetID();
  CTexture* pTexture = static_cast<CTexture*>(pTextureManager_->GetTexture(TextureID));
  {
    if (ClearTextures_.find(TextureID) == ClearTextures_.end())
    {
      ClearTextures_.insert(std::make_pair(TextureID, _UnorderedAccessTexture));

      pTexture->SetImageLayout(CurrentGraphicsCommandBuffer, kImageLayout, SubresourceRange);
    }
  }

  CurrentGraphicsCommandBuffer.clearColorImage(pTexture->Image.get(), kImageLayout, Cast(_ClearColor), SubresourceRange);
}

void CDeviceContext::Draw(vdl::uint _VertexCount, vdl::uint _InstanceCount, vdl::uint _FirstVertex, vdl::uint _FirstInstance)
{
  PreprocessingGraphicsCommandBufferDraw();

  GetCurrentGraphicsCommandBuffer().draw(_VertexCount, _InstanceCount, _FirstVertex, _FirstInstance);
}

void CDeviceContext::DrawIndexed(vdl::uint _IndexCount, vdl::uint _InstanceCount, vdl::uint _FirstIndex, vdl::uint _VertexOffset, vdl::uint _FirstInstance)
{
  PreprocessingGraphicsCommandBufferDraw();

  GetCurrentGraphicsCommandBuffer().drawIndexed(_IndexCount, _InstanceCount, _FirstIndex, _VertexOffset, _FirstInstance);
}

void CDeviceContext::Dispatch(vdl::uint _ThreadGroupX, vdl::uint _ThreadGroupY, vdl::uint _ThreadGroupZ)
{

}

void CDeviceContext::Flush()
{

}

//--------------------------------------------------

std::vector<vk::CommandBuffer> CDeviceContext::GetCommandBuffers()
{

}

void CDeviceContext::Flush(const std::vector<vk::CommandBuffer>& _CommandBuffers, const vk::Semaphore& _SingnalSemaphore)
{

}

//--------------------------------------------------

void CDeviceContext::BeginGraphicsCommandBuffer()
{
  if (GraphicsCommandBufferState_ == CommandBufferState::eBeginRenderPass)
  {
    Flush();
  }

  if (GraphicsCommandBufferState_ == CommandBufferState::eIdle)
  {
    GetCurrentGraphicsCommandBuffer().begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
    GraphicsCommandBufferState_ = CommandBufferState::eBegin;
  }
}

void CDeviceContext::PreprocessingGraphicsCommandBufferDraw()
{
  assert(GraphicsCommandBufferState_ == CommandBufferState::eBeginRenderPass);

  const vk::CommandBuffer& CurrentGraphicsCommandBuffer = GetCurrentGraphicsCommandBuffer();

  //  SetInstanceBuffer
  {
    PreviousGraphicsState_.pInstanceBuffer = CurrentGraphicsState_.pInstanceBuffer;
    assert(PreviousGraphicsState_.pInstanceBuffer->GetType() == BufferType::eInstanceBuffer);

    const CInstanceBuffer* pInstanceBuffer = static_cast<const CInstanceBuffer*>(PreviousGraphicsState_.pInstanceBuffer);
    CurrentGraphicsCommandBuffer.bindVertexBuffers(1, pInstanceBuffer->BufferData.Buffer.get(), pInstanceBuffer->PreviousOffset);
    GraphicsStateChangeFlags_.Cancel(GraphicsCommandType::eSetInstanceBuffer);
  }

  if (GraphicsStateChangeFlags_.isEmpty())
  {
    return;
  }

  //  SetPipeline
  for (auto& GraphicsPipelineStateCommand : kGraphicsPipelineStateCommands)
  {
    if (GraphicsStateChangeFlags_.Has(GraphicsPipelineStateCommand))
    {
      PreviousGraphicsState_.VertexShader = CurrentGraphicsState_.VertexShader;
      PreviousGraphicsState_.HullShader = CurrentGraphicsState_.HullShader;
      PreviousGraphicsState_.DomainShader = CurrentGraphicsState_.DomainShader;
      PreviousGraphicsState_.GeometryShader = CurrentGraphicsState_.GeometryShader;
      PreviousGraphicsState_.PixelShader = CurrentGraphicsState_.PixelShader;
      std::vector<vk::PipelineShaderStageCreateInfo> PipelineShaderStageInfos;
      {
        if (!PreviousGraphicsState_.VertexShader.isEmpty())
        {
          assert(pShaderManager_->GetShader(PreviousGraphicsState_.VertexShader.GetID())->GetType() == ShaderType::eVertexShader);
          const CVertexShader* pVertexShader = static_cast<CVertexShader*>(pShaderManager_->GetShader(PreviousGraphicsState_.VertexShader.GetID()));

          vk::PipelineShaderStageCreateInfo PipelineShaderStageInfo;
          {
            PipelineShaderStageInfo.stage = vk::ShaderStageFlagBits::eVertex;
            PipelineShaderStageInfo.module = pVertexShader->ShaderData.Module.get();
            PipelineShaderStageInfo.pName = pVertexShader->ShaderData.EntryPoint;
          }

          PipelineShaderStageInfos.emplace_back(std::move(PipelineShaderStageInfo));
        }

        if (!PreviousGraphicsState_.HullShader.isEmpty())
        {
          assert(pShaderManager_->GetShader(PreviousGraphicsState_.HullShader.GetID())->GetType() == ShaderType::eHullShader);
          const CHullShader* pHullShader = static_cast<CHullShader*>(pShaderManager_->GetShader(PreviousGraphicsState_.HullShader.GetID()));

          vk::PipelineShaderStageCreateInfo PipelineShaderStageInfo;
          {
            PipelineShaderStageInfo.stage = vk::ShaderStageFlagBits::eTessellationControl;
            PipelineShaderStageInfo.module = pHullShader->ShaderData.Module.get();
            PipelineShaderStageInfo.pName = pHullShader->ShaderData.EntryPoint;
          }

          PipelineShaderStageInfos.emplace_back(std::move(PipelineShaderStageInfo));
        }

        if (!PreviousGraphicsState_.DomainShader.isEmpty())
        {
          assert(pShaderManager_->GetShader(PreviousGraphicsState_.DomainShader.GetID())->GetType() == ShaderType::eHullShader);
          const CDomainShader* pDomainShader = static_cast<CDomainShader*>(pShaderManager_->GetShader(PreviousGraphicsState_.DomainShader.GetID()));

          vk::PipelineShaderStageCreateInfo PipelineShaderStageInfo;
          {
            PipelineShaderStageInfo.stage = vk::ShaderStageFlagBits::eTessellationEvaluation;
            PipelineShaderStageInfo.module = pDomainShader->ShaderData.Module.get();
            PipelineShaderStageInfo.pName = pDomainShader->ShaderData.EntryPoint;
          }

          PipelineShaderStageInfos.emplace_back(std::move(PipelineShaderStageInfo));
        }

        if (!PreviousGraphicsState_.GeometryShader.isEmpty())
        {
          assert(pShaderManager_->GetShader(PreviousGraphicsState_.GeometryShader.GetID())->GetType() == ShaderType::eGeometryShader);
          const CGeometryShader* pGeometryShader = static_cast<CGeometryShader*>(pShaderManager_->GetShader(PreviousGraphicsState_.GeometryShader.GetID()));

          vk::PipelineShaderStageCreateInfo PipelineShaderStageInfo;
          {
            PipelineShaderStageInfo.stage = vk::ShaderStageFlagBits::eGeometry;
            PipelineShaderStageInfo.module = pGeometryShader->ShaderData.Module.get();
            PipelineShaderStageInfo.pName = pGeometryShader->ShaderData.EntryPoint;
          }

          PipelineShaderStageInfos.emplace_back(std::move(PipelineShaderStageInfo));
        }

        if (!PreviousGraphicsState_.PixelShader.isEmpty())
        {
          assert(pShaderManager_->GetShader(PreviousGraphicsState_.PixelShader.GetID())->GetType() == ShaderType::ePixelShader);
          const CPixelShader* pPixelShader = static_cast<CPixelShader*>(pShaderManager_->GetShader(PreviousGraphicsState_.PixelShader.GetID()));

          vk::PipelineShaderStageCreateInfo PipelineShaderStageInfo;
          {
            PipelineShaderStageInfo.stage = vk::ShaderStageFlagBits::eFragment;
            PipelineShaderStageInfo.module = pPixelShader->ShaderData.Module.get();
            PipelineShaderStageInfo.pName = pPixelShader->ShaderData.EntryPoint;
          }

          PipelineShaderStageInfos.emplace_back(std::move(PipelineShaderStageInfo));
        }
      }

      PreviousGraphicsState_.InputLayout = CurrentGraphicsState_.InputLayout;
      vk::PipelineVertexInputStateCreateInfo PipelineVertexInputStateInfo;
      {
        InputLayout& InputLayout = InputLayouts_[PreviousGraphicsState_.InputLayout];

        PipelineVertexInputStateInfo.vertexBindingDescriptionCount = static_cast<vdl::uint>(InputLayout.VertexInputBindingDescriptions.size());
        PipelineVertexInputStateInfo.pVertexBindingDescriptions = InputLayout.VertexInputBindingDescriptions.data();
        PipelineVertexInputStateInfo.vertexAttributeDescriptionCount = static_cast<vdl::uint>(InputLayout.VertexInputAttributeDescriptions.size());
        PipelineVertexInputStateInfo.pVertexAttributeDescriptions = InputLayout.VertexInputAttributeDescriptions.data();
      }

      PreviousGraphicsState_.Topology = CurrentGraphicsState_.Topology;
      vk::PipelineInputAssemblyStateCreateInfo PipelineInputAssemblyStateInfo;
      {
        PipelineInputAssemblyStateInfo.topology = Cast(PreviousGraphicsState_.Topology);
        PipelineInputAssemblyStateInfo.primitiveRestartEnable = false;
      }

      vk::PipelineTessellationStateCreateInfo PipelineTessellationStateInfo;
      {
        PipelineTessellationStateInfo.patchControlPoints = GetPatchControlPoints(PreviousGraphicsState_.Topology);
      }

      vk::PipelineViewportStateCreateInfo PipelineViewportStateInfo;
      {
        PipelineViewportStateInfo.viewportCount = 1;
        PipelineViewportStateInfo.pViewports = nullptr;
        PipelineViewportStateInfo.scissorCount = 1;
        PipelineViewportStateInfo.pScissors = nullptr;
      }

      PreviousGraphicsState_.RasterizerState = CurrentGraphicsState_.RasterizerState;
      const vk::PipelineRasterizationStateCreateInfo& PipelineRasterizationStateInfo = GetPipelineRasterizationStateInfo(PreviousGraphicsState_.RasterizerState);

      PreviousGraphicsState_.DepthStencilState = CurrentGraphicsState_.DepthStencilState;
      const vk::PipelineDepthStencilStateCreateInfo& PipelineDepthStencilStateInfo = GetPipelineDepthStencilStateInfo(PreviousGraphicsState_.DepthStencilState);

      PreviousGraphicsState_.BlendState = CurrentGraphicsState_.BlendState;
      std::array<vk::PipelineColorBlendAttachmentState, Constants::kMaxRenderTextureNum> PipelineColorBlendAttachmentStates;
      {
        if (PreviousGraphicsState_.BlendState.IndependentBlendEnable)
        {
          for (vdl::uint i = 0; i < Constants::kMaxRenderTextureNum; ++i)
          {
            PipelineColorBlendAttachmentStates[i] = GetPipelineColorBlendAttachmentState(PreviousGraphicsState_.BlendState.RenderTexture[i]);
          }
        }
        else
        {
          PipelineColorBlendAttachmentStates[0] = GetPipelineColorBlendAttachmentState(PreviousGraphicsState_.BlendState.RenderTexture[0]);
          for (vdl::uint i = 1; i < Constants::kMaxRenderTextureNum; ++i)
          {
            PipelineColorBlendAttachmentStates[i] = GetPipelineColorBlendAttachmentState(PreviousGraphicsState_.BlendState.RenderTexture[i]);
          }
        }
      }

      const vk::PipelineMultisampleStateCreateInfo& PipelineMultisampleStateInfo = GetMultisampleStateInfo(PreviousGraphicsState_.BlendState.AlphaToCoverageEnable);

      vk::PipelineColorBlendStateCreateInfo PipelineColorBlendStateInfo;
      {
        PipelineColorBlendStateInfo.logicOpEnable = false;
        PipelineColorBlendStateInfo.logicOp = vk::LogicOp::eNoOp;
        PipelineColorBlendStateInfo.attachmentCount = static_cast<vdl::uint>(PipelineColorBlendAttachmentStates.size());
        PipelineColorBlendStateInfo.pAttachments = PipelineColorBlendAttachmentStates.data();
        PipelineColorBlendStateInfo.blendConstants[0] = 1.0f;
        PipelineColorBlendStateInfo.blendConstants[1] = 1.0f;
        PipelineColorBlendStateInfo.blendConstants[2] = 1.0f;
        PipelineColorBlendStateInfo.blendConstants[3] = 1.0f;
      }

      std::array<vk::DynamicState, 2> DynamicState;
      {
        DynamicState[0] = vk::DynamicState::eViewport;
        DynamicState[1] = vk::DynamicState::eScissor;
      }

      vk::PipelineDynamicStateCreateInfo PipelineDynamicStateInfo;
      {
        PipelineDynamicStateInfo.dynamicStateCount = static_cast<vdl::uint>(DynamicState.size());
        PipelineDynamicStateInfo.pDynamicStates = DynamicState.data();
      }

      //  パイプラインの作成
      {
        std::vector<vk::UniquePipeline>& CurrentGraphicsPipelines = GraphicsPipelines_[GraphicsCommandBufferIndex_];
        const vk::Pipeline& PreviousPipeline = CurrentGraphicsPipelines.back().get();

        vk::GraphicsPipelineCreateInfo GraphicsPipelineInfo;
        {
          GraphicsPipelineInfo.flags = (PreviousPipeline == VK_NULL_HANDLE ? vk::PipelineCreateFlagBits::eAllowDerivatives : vk::PipelineCreateFlagBits::eAllowDerivatives | vk::PipelineCreateFlagBits::eDerivative);
          GraphicsPipelineInfo.stageCount = static_cast<vdl::uint>(PipelineShaderStageInfos.size());
          GraphicsPipelineInfo.pStages = PipelineShaderStageInfos.data();
          GraphicsPipelineInfo.pVertexInputState = &PipelineVertexInputStateInfo;
          GraphicsPipelineInfo.pInputAssemblyState = &PipelineInputAssemblyStateInfo;
          GraphicsPipelineInfo.pTessellationState = &PipelineTessellationStateInfo;
          GraphicsPipelineInfo.pViewportState = &PipelineViewportStateInfo;
          GraphicsPipelineInfo.pRasterizationState = &PipelineRasterizationStateInfo;
          GraphicsPipelineInfo.pMultisampleState = &PipelineMultisampleStateInfo;
          GraphicsPipelineInfo.pDepthStencilState = &PipelineDepthStencilStateInfo;
          GraphicsPipelineInfo.pColorBlendState = &PipelineColorBlendStateInfo;
          GraphicsPipelineInfo.pDynamicState = &PipelineDynamicStateInfo;
          GraphicsPipelineInfo.layout = PipelineLayout_.get();
          GraphicsPipelineInfo.renderPass = RenderPassDatas_[GraphicsCommandBufferIndex_].RenderPass.get();
          GraphicsPipelineInfo.subpass = 0;
          GraphicsPipelineInfo.basePipelineHandle = PreviousPipeline;
          GraphicsPipelineInfo.basePipelineIndex = (PreviousPipeline == VK_NULL_HANDLE ? 0 : -1);
        }

        vk::UniquePipeline Pipeline = VkDevice_.createGraphicsPipelineUnique(GraphicsPipelineCache_.get(), GraphicsPipelineInfo);
        assert(Pipeline);

        CurrentGraphicsCommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, Pipeline.get());

        CurrentGraphicsPipelines.emplace_back(std::move(Pipeline));
      }

      break;
    }
  }

  //  SetDescriptor
  for (auto& GraphicsDescriptorCommand : kGraphicsDescriptorCommands)
  {
    if (GraphicsStateChangeFlags_.Has(GraphicsDescriptorCommand))
    {


      std::vector<vk::DescriptorSet> DescriptorSets;
      //for (auto& DescriptorSet : DescriptorSets_)
      //{
      //  DescriptorSets.push_back(DescriptorSet.get());
      //}

      CurrentGraphicsCommandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, PipelineLayout_.get(), 0, DescriptorSets, nullptr);

      break;
    }
  }

  if (GraphicsStateChangeFlags_.Has(GraphicsCommandType::eSetVertexBuffer))
  {
    PreviousGraphicsState_.pVertexBuffer = CurrentGraphicsState_.pVertexBuffer;
    assert(PreviousGraphicsState_.pVertexBuffer->GetType() == BufferType::eVertexBuffer);

    const CVertexBuffer* pVertexBuffer = static_cast<const CVertexBuffer*>(PreviousGraphicsState_.pVertexBuffer);

    constexpr vk::DeviceSize kOffset = 0;
    CurrentGraphicsCommandBuffer.bindVertexBuffers(0, pVertexBuffer->BufferData.Buffer.get(), kOffset);
  }

  if (GraphicsStateChangeFlags_.Has(GraphicsCommandType::eSetIndexBuffer))
  {
    PreviousGraphicsState_.pIndexBuffer = CurrentGraphicsState_.pIndexBuffer;
    assert(PreviousGraphicsState_.pIndexBuffer->GetType() == BufferType::eVertexBuffer);

    const CIndexBuffer* pIndexBuffer = static_cast<const CIndexBuffer*>(PreviousGraphicsState_.pIndexBuffer);

    constexpr vk::DeviceSize kOffset = 0;
    CurrentGraphicsCommandBuffer.bindIndexBuffer(pIndexBuffer->BufferData.Buffer.get(), kOffset, pIndexBuffer->IndexType);
  }

  if (GraphicsStateChangeFlags_.Has(GraphicsCommandType::eSetScissor))
  {
    PreviousGraphicsState_.Scissor = CurrentGraphicsState_.Scissor;

    CurrentGraphicsCommandBuffer.setScissor(0,
      Cast(CurrentGraphicsState_.RasterizerState.ScissorEnable ? PreviousGraphicsState_.Scissor
        : vdl::Scissor(CurrentGraphicsState_.Viewport.LeftTop, CurrentGraphicsState_.Viewport.Size)));
  }

  if (GraphicsStateChangeFlags_.Has(GraphicsCommandType::eSetViewport))
  {
    PreviousGraphicsState_.Viewport = CurrentGraphicsState_.Viewport;

    CurrentGraphicsCommandBuffer.setViewport(0, Cast(PreviousGraphicsState_.Viewport));
  }

  GraphicsStateChangeFlags_.Clear();
}

const vk::PipelineRasterizationStateCreateInfo& CDeviceContext::GetPipelineRasterizationStateInfo(const vdl::RasterizerState& _RasterizerState)
{
  if (RasterizerStates_.find(_RasterizerState) == RasterizerStates_.end())
  {
    vk::PipelineRasterizationStateCreateInfo PipelineRasterizationStateInfo;
    {
      PipelineRasterizationStateInfo.depthClampEnable = _RasterizerState.DepthClipEnable;
      PipelineRasterizationStateInfo.rasterizerDiscardEnable = false;
      PipelineRasterizationStateInfo.polygonMode = Cast(_RasterizerState.FillMode);
      PipelineRasterizationStateInfo.cullMode = Cast(_RasterizerState.CullMode);
      PipelineRasterizationStateInfo.frontFace = (_RasterizerState.FrontCounterClockwise ? vk::FrontFace::eCounterClockwise : vk::FrontFace::eClockwise);
      PipelineRasterizationStateInfo.depthBiasEnable = true;
      PipelineRasterizationStateInfo.depthBiasConstantFactor = _RasterizerState.DepthBias;
      PipelineRasterizationStateInfo.depthBiasClamp = 0.0f;
      PipelineRasterizationStateInfo.depthBiasSlopeFactor = 0.0f;
      PipelineRasterizationStateInfo.lineWidth = 1.0f;
    }

    RasterizerStates_.insert(std::make_pair(_RasterizerState, std::move(PipelineRasterizationStateInfo)));
  }

  return RasterizerStates_.at(_RasterizerState);
}

const vk::PipelineDepthStencilStateCreateInfo& CDeviceContext::GetPipelineDepthStencilStateInfo(const vdl::DepthStencilState& _DepthStencilState)
{
  if (DepthStencilStates_.find(_DepthStencilState) == DepthStencilStates_.end())
  {
    vk::PipelineDepthStencilStateCreateInfo PipelineDepthStencilStateInfo;
    {
      PipelineDepthStencilStateInfo.depthTestEnable = _DepthStencilState.DepthEnable;
      PipelineDepthStencilStateInfo.depthWriteEnable = _DepthStencilState.DepthWriteMask == vdl::DepthWriteMaskType::eAll;
      PipelineDepthStencilStateInfo.depthCompareOp = Cast(_DepthStencilState.DepthFunc);
      PipelineDepthStencilStateInfo.depthBoundsTestEnable = true;
      PipelineDepthStencilStateInfo.stencilTestEnable = _DepthStencilState.StencilEnable;
      PipelineDepthStencilStateInfo.front = Cast(_DepthStencilState.FrontFace);
      PipelineDepthStencilStateInfo.back = Cast(_DepthStencilState.BackFace);
      PipelineDepthStencilStateInfo.front.reference = PipelineDepthStencilStateInfo.back.reference = _DepthStencilState.StencilReadMask;
      PipelineDepthStencilStateInfo.front.writeMask = PipelineDepthStencilStateInfo.back.writeMask = _DepthStencilState.StencilWriteMask;
      PipelineDepthStencilStateInfo.minDepthBounds = 0.0f;
      PipelineDepthStencilStateInfo.maxDepthBounds = 1.0f;
    }

    DepthStencilStates_.insert(std::make_pair(_DepthStencilState, std::move(PipelineDepthStencilStateInfo)));
  }

  return DepthStencilStates_.at(_DepthStencilState);
}

const vk::PipelineColorBlendAttachmentState& CDeviceContext::GetPipelineColorBlendAttachmentState(const vdl::RenderTextureBlendState& _RenderTextureBlendState)
{
  if (RenderTextureBlendStates_.find(_RenderTextureBlendState) == RenderTextureBlendStates_.end())
  {
    vk::PipelineColorBlendAttachmentState PipelineColorBlendAttachmentState;
    {

      PipelineColorBlendAttachmentState.blendEnable = _RenderTextureBlendState.BlendEnable;
      PipelineColorBlendAttachmentState.srcColorBlendFactor = Cast(_RenderTextureBlendState.SrcBlend);
      PipelineColorBlendAttachmentState.dstColorBlendFactor = Cast(_RenderTextureBlendState.DestBlend);
      PipelineColorBlendAttachmentState.colorBlendOp = Cast(_RenderTextureBlendState.BlendOp);
      PipelineColorBlendAttachmentState.srcAlphaBlendFactor = Cast(_RenderTextureBlendState.SrcBlendAlpha);
      PipelineColorBlendAttachmentState.dstAlphaBlendFactor = Cast(_RenderTextureBlendState.DestBlendAlpha);
      PipelineColorBlendAttachmentState.alphaBlendOp = Cast(_RenderTextureBlendState.BlendOpAlpha);
      PipelineColorBlendAttachmentState.colorWriteMask = Cast(_RenderTextureBlendState.RenderTargetWriteMask);
    }

    RenderTextureBlendStates_.insert(std::make_pair(_RenderTextureBlendState, std::move(PipelineColorBlendAttachmentState)));
  }

  return RenderTextureBlendStates_.at(_RenderTextureBlendState);
}

const vk::PipelineMultisampleStateCreateInfo& CDeviceContext::GetMultisampleStateInfo(bool _AlphaToCoverageEnable)
{
  if (MultisampleStates_.find(_AlphaToCoverageEnable) == MultisampleStates_.end())
  {
    vk::PipelineMultisampleStateCreateInfo PipelineMultisampleStateInfo;
    {
      PipelineMultisampleStateInfo.rasterizationSamples = vk::SampleCountFlagBits::e1;
      PipelineMultisampleStateInfo.sampleShadingEnable = false;
      PipelineMultisampleStateInfo.minSampleShading = 0.0f;
      PipelineMultisampleStateInfo.pSampleMask = nullptr;
      PipelineMultisampleStateInfo.alphaToCoverageEnable = _AlphaToCoverageEnable;
      PipelineMultisampleStateInfo.alphaToOneEnable = false;
    }

    MultisampleStates_.insert(std::make_pair(_AlphaToCoverageEnable, std::move(PipelineMultisampleStateInfo)));
  }

  return MultisampleStates_.at(_AlphaToCoverageEnable);
}
