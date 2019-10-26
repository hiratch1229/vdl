#include "CDeviceContext.hpp"

#include <vdl/Engine.hpp>
#include <vdl/Window/IWindow.hpp>
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
    case DescriptorType::eTexture:
      return vk::DescriptorType::eSampledImage;
    case DescriptorType::eBuffer:
      return vk::DescriptorType::eStorageBuffer;
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
    case DescriptorType::eTexture:
      return Constants::kMaxShaderResourceNum;
    case DescriptorType::eBuffer:
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

  inline vk::ClearColorValue Cast(const vdl::ColorF & _Color)
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

  inline vk::StencilOpState Cast(const vdl::DepthStencilOpState & _StencilOpState)
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

  inline vk::Filter GetMag(vdl::FilterType _Type)
  {
    switch (_Type)
    {
    case vdl::FilterType::eMinMagMipPoint:
    case vdl::FilterType::eMinMagPointMipLinear:
    case vdl::FilterType::eMinLinearMagMipPoint:
    case vdl::FilterType::eMinLinearMagPointMipLinear:
    case vdl::FilterType::eComparisonMinMagMipPoint:
    case vdl::FilterType::eComparisonMinMagPointMipLinear:
    case vdl::FilterType::eComparisonMinLinearMagMipPoint:
    case vdl::FilterType::eComparisonMinLinearMagPointMipLinear:
      return vk::Filter::eNearest;
    case vdl::FilterType::eMinPointMagLinearMipPoint:
    case vdl::FilterType::eMinPointMagMipLinear:
    case vdl::FilterType::eMinMagLinearMipPoint:
    case vdl::FilterType::eMinMagMipLinear:
    case vdl::FilterType::eComparisonMinPointMagLinearMipPoint:
    case vdl::FilterType::eComparisonMinPointMagMipLinear:
    case vdl::FilterType::eComparisonMinMagLinearMipPoint:
    case vdl::FilterType::eComparisonMinMagMipLinear:
      return vk::Filter::eLinear;
    case vdl::FilterType::eAnisotropic:
    case vdl::FilterType::eComparisonAnisotropic:
      break;
    default: assert(false);
    }

    return vk::Filter::eNearest;
  }

  inline vk::Filter GetMin(vdl::FilterType _Type)
  {
    switch (_Type)
    {
    case vdl::FilterType::eMinMagMipPoint:
    case vdl::FilterType::eMinMagPointMipLinear:
    case vdl::FilterType::eMinPointMagLinearMipPoint:
    case vdl::FilterType::eMinPointMagMipLinear:
    case vdl::FilterType::eComparisonMinMagMipPoint:
    case vdl::FilterType::eComparisonMinMagPointMipLinear:
    case vdl::FilterType::eComparisonMinPointMagLinearMipPoint:
    case vdl::FilterType::eComparisonMinPointMagMipLinear:
      return vk::Filter::eNearest;
    case vdl::FilterType::eMinLinearMagMipPoint:
    case vdl::FilterType::eMinLinearMagPointMipLinear:
    case vdl::FilterType::eMinMagLinearMipPoint:
    case vdl::FilterType::eMinMagMipLinear:
    case vdl::FilterType::eComparisonMinLinearMagMipPoint:
    case vdl::FilterType::eComparisonMinLinearMagPointMipLinear:
    case vdl::FilterType::eComparisonMinMagLinearMipPoint:
    case vdl::FilterType::eComparisonMinMagMipLinear:
      return vk::Filter::eLinear;
    case vdl::FilterType::eAnisotropic:
    case vdl::FilterType::eComparisonAnisotropic:
      break;
    default: assert(false);
    }

    return vk::Filter::eNearest;
  }

  inline vk::SamplerMipmapMode GetMimap(vdl::FilterType _Type)
  {
    switch (_Type)
    {
    case vdl::FilterType::eMinMagMipPoint:
    case vdl::FilterType::eMinPointMagLinearMipPoint:
    case vdl::FilterType::eMinLinearMagMipPoint:
    case vdl::FilterType::eMinMagLinearMipPoint:
    case vdl::FilterType::eComparisonMinMagMipPoint:
    case vdl::FilterType::eComparisonMinPointMagLinearMipPoint:
    case vdl::FilterType::eComparisonMinLinearMagMipPoint:
    case vdl::FilterType::eComparisonMinMagLinearMipPoint:
      return vk::SamplerMipmapMode::eNearest;
    case vdl::FilterType::eMinMagPointMipLinear:
    case vdl::FilterType::eMinPointMagMipLinear:
    case vdl::FilterType::eMinLinearMagPointMipLinear:
    case vdl::FilterType::eMinMagMipLinear:
    case vdl::FilterType::eComparisonMinMagPointMipLinear:
    case vdl::FilterType::eComparisonMinPointMagMipLinear:
    case vdl::FilterType::eComparisonMinLinearMagPointMipLinear:
    case vdl::FilterType::eComparisonMinMagMipLinear:
      return vk::SamplerMipmapMode::eLinear;
    case vdl::FilterType::eAnisotropic:
    case vdl::FilterType::eComparisonAnisotropic:
      break;
    default: assert(false);
    }

    return vk::SamplerMipmapMode::eNearest;
  }

  inline bool isAnisotropic(vdl::FilterType _Type)
  {
    return _Type == vdl::FilterType::eAnisotropic || _Type == vdl::FilterType::eComparisonAnisotropic;
  }

  inline bool enableComparison(vdl::FilterType _Type)
  {
    return _Type == vdl::FilterType::eComparisonMinMagMipPoint
      || _Type == vdl::FilterType::eComparisonMinMagPointMipLinear
      || _Type == vdl::FilterType::eComparisonMinPointMagLinearMipPoint
      || _Type == vdl::FilterType::eComparisonMinPointMagMipLinear
      || _Type == vdl::FilterType::eComparisonMinLinearMagMipPoint
      || _Type == vdl::FilterType::eComparisonMinLinearMagPointMipLinear
      || _Type == vdl::FilterType::eComparisonMinMagLinearMipPoint
      || _Type == vdl::FilterType::eComparisonMinMagMipLinear
      || _Type == vdl::FilterType::eComparisonAnisotropic;
  }

  inline vk::SamplerAddressMode Cast(vdl::AddressModeType _Type)
  {
    switch (_Type)
    {
    case vdl::AddressModeType::eWrap:
      return vk::SamplerAddressMode::eRepeat;
    case vdl::AddressModeType::eMirror:
      return vk::SamplerAddressMode::eMirroredRepeat;
    case vdl::AddressModeType::eClamp:
      return vk::SamplerAddressMode::eClampToEdge;
    case vdl::AddressModeType::eBorder:
      return vk::SamplerAddressMode::eClampToBorder;
    case vdl::AddressModeType::eMirrorOnce:
      return vk::SamplerAddressMode::eMirrorClampToEdge;
    default: assert(false);
    }

    return vk::SamplerAddressMode::eRepeat;
  }

  inline vk::BorderColor Cast(vdl::BorderColorType _Type)
  {
    switch (_Type)
    {
    case vdl::BorderColorType::eTransparent:
      return vk::BorderColor::eFloatTransparentBlack;
    case vdl::BorderColorType::eBlack:
      return vk::BorderColor::eFloatOpaqueBlack;
    case vdl::BorderColorType::eWhite:
      return vk::BorderColor::eFloatOpaqueWhite;
    default: assert(false);
    }

    return vk::BorderColor::eFloatTransparentBlack;
  }

  struct DescriptorImageData
  {
    vk::DescriptorImageInfo Info;
    vdl::uint Bind;
  };

  struct DescriptorBufferData
  {
    vk::DescriptorBufferInfo Info;
    vdl::uint Bind;
  };
}

void CDeviceContext::Initialize()
{
  pWindow_ = Engine::Get<IWindow>();
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
          InputLayout.VertexInputBindingDescriptions[0].stride = sizeof(vdl::Vertex2D);
          InputLayout.VertexInputBindingDescriptions[0].inputRate = vk::VertexInputRate::eVertex;

          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32Sfloat, sizeof(vdl::Vertex2D::Position));
          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32Sfloat, sizeof(vdl::Vertex2D::Texcoord));
        }

        ++BindDescriptionCount;
        Offset = 0;

        //  インスタンスデータ
        {
          InputLayout.VertexInputBindingDescriptions[1].binding = 1;
          InputLayout.VertexInputBindingDescriptions[1].stride = sizeof(Instance2D);
          InputLayout.VertexInputBindingDescriptions[1].inputRate = vk::VertexInputRate::eInstance;

          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32B32A32Sfloat, sizeof(Instance2D::NDCTransform.r[0]));
          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32B32A32Sfloat, sizeof(Instance2D::NDCTransform.r[1]));
          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32B32A32Sfloat, sizeof(Instance2D::NDCTransform.r[2]));
          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32B32A32Sfloat, sizeof(Instance2D::NDCTransform.r[3]));
          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32Sfloat, sizeof(Instance2D::TexcoordScale));
          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32Sfloat, sizeof(Instance2D::TexcoordTranslate));
          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32B32A32Sfloat, sizeof(Instance2D::Color));
        }
      }

      InputLayouts_.insert(std::make_pair(vdl::InputLayoutType::eTexture, std::move(InputLayout)));
    }

    //  Mesh
    {
      Offset = Location = BindDescriptionCount = 0;

      InputLayout InputLayout;
      {
        InputLayout.VertexInputBindingDescriptions.resize(2);
        InputLayout.VertexInputAttributeDescriptions.resize(11);

        //  頂点データ
        {
          InputLayout.VertexInputBindingDescriptions[0].binding = 0;
          InputLayout.VertexInputBindingDescriptions[0].stride = sizeof(vdl::Vertex3D);
          InputLayout.VertexInputBindingDescriptions[0].inputRate = vk::VertexInputRate::eVertex;

          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32B32Sfloat, sizeof(vdl::Vertex3D::Position));
          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32B32Sfloat, sizeof(vdl::Vertex3D::Normal));
          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32B32Sfloat, sizeof(vdl::Vertex3D::Tangent));
          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32Sfloat, sizeof(vdl::Vertex3D::Texcoord));
          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32B32A32Sfloat, sizeof(vdl::Vertex3D::BoneWeights));
          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32B32A32Uint, sizeof(vdl::Vertex3D::BoneIndices));
        }

        ++BindDescriptionCount;
        Offset = 0;

        //  インスタンスデータ
        {
          InputLayout.VertexInputBindingDescriptions[1].binding = 1;
          InputLayout.VertexInputBindingDescriptions[1].stride = sizeof(Instance3D);
          InputLayout.VertexInputBindingDescriptions[1].inputRate = vk::VertexInputRate::eInstance;

          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32B32A32Sfloat, sizeof(Instance3D::World.r[0]));
          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32B32A32Sfloat, sizeof(Instance3D::World.r[1]));
          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32B32A32Sfloat, sizeof(Instance3D::World.r[2]));
          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32B32A32Sfloat, sizeof(Instance3D::World.r[3]));
          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32B32A32Sfloat, sizeof(Instance3D::Color));        }
      }

      InputLayouts_.insert(std::make_pair(vdl::InputLayoutType::eMesh, std::move(InputLayout)));
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
          std::vector<vk::DescriptorSetLayoutBinding>& LayoutBining = LayoutBindings[GetDescriptorLayoutOffset(static_cast<ShaderType>(j), static_cast<DescriptorType>(i))];
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

    //  デスクリプタプールの作成
    {
      std::array<vk::DescriptorPoolSize, kGraphicsDescriptorTypeNum> TypeCounts;
      {
        auto SetTypeCount = [&TypeCounts, this](DescriptorType _Type)->void
        {
          TypeCounts[static_cast<vdl::uint>(_Type)].type = Cast(_Type);
          TypeCounts[static_cast<vdl::uint>(_Type)].descriptorCount = GetPerCount(_Type);
        };

        //  テクスチャ
        SetTypeCount(DescriptorType::eTexture);
        //  バッファ
        SetTypeCount(DescriptorType::eBuffer);
        //  サンプラー
        SetTypeCount(DescriptorType::eSampler);
        //  定数バッファ
        SetTypeCount(DescriptorType::eConstantBuffer);
      }

      vk::DescriptorPoolCreateInfo DescriptorPoolInfo;
      {
        DescriptorPoolInfo.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
        DescriptorPoolInfo.maxSets = kGraphicsDescriptorPoolMaxSet;
        DescriptorPoolInfo.poolSizeCount = static_cast<vdl::uint>(TypeCounts.size());
        DescriptorPoolInfo.pPoolSizes = TypeCounts.data();
      }

      GraphicsDescriptorPool_ = VkDevice_.createDescriptorPoolUnique(DescriptorPoolInfo);
      assert(GraphicsDescriptorPool_);
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

    //  同期オブジェクトの作成
    {
      vk::FenceCreateInfo FenceInfo;
      vk::SemaphoreCreateInfo SemaphoreInfo;
      for (auto& GraphicsSyncState : GraphicsSyncStates_)
      {
        GraphicsSyncState.Fence = VkDevice_.createFenceUnique(FenceInfo);
        assert(GraphicsSyncState.Fence);
        FenceInfo = vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled);
        GraphicsSyncState.Semaphore = VkDevice_.createSemaphoreUnique(SemaphoreInfo);
        assert(GraphicsSyncState.Semaphore);
      }
    }

    GraphicsReserveDatas_[GraphicsCommandBufferIndex_].Pipelines.resize(1);
  }

  //  コンピュート用オブジェクトの作成
  {
    //  デスクリプタセットレイアウトの作成
    {
      std::array<std::vector<vk::DescriptorSetLayoutBinding>, kComputeDescriptorTypeNum> LayoutBindings;

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

    //  デスクリプタプールの作成
    {
      std::array<vk::DescriptorPoolSize, kComputeDescriptorTypeNum> TypeCounts;
      {
        auto SetTypeCount = [&TypeCounts, this](DescriptorType _Type)->void
        {
          TypeCounts[static_cast<vdl::uint>(_Type)].type = Cast(_Type);
          TypeCounts[static_cast<vdl::uint>(_Type)].descriptorCount = GetPerCount(_Type);
        };

        //  テクスチャ
        SetTypeCount(DescriptorType::eTexture);
        //  バッファ
        SetTypeCount(DescriptorType::eBuffer);
        //  サンプラー
        SetTypeCount(DescriptorType::eSampler);
        //  定数バッファ
        SetTypeCount(DescriptorType::eConstantBuffer);
        //  順不同アクセスバッファ
        SetTypeCount(DescriptorType::eUnorderedAccessBuffer);
        //  順不同アクセスバッファ
        SetTypeCount(DescriptorType::eUnorderedAccessTexture);
      }

      vk::DescriptorPoolCreateInfo DescriptorPoolInfo;
      {
        DescriptorPoolInfo.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
        DescriptorPoolInfo.maxSets = kComputeDescriptorPoolMaxSet;
        DescriptorPoolInfo.poolSizeCount = static_cast<vdl::uint>(TypeCounts.size());
        DescriptorPoolInfo.pPoolSizes = TypeCounts.data();
      }

      ComputeDescriptorPool_ = VkDevice_.createDescriptorPoolUnique(DescriptorPoolInfo);
      assert(ComputeDescriptorPool_);
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
        CommandBufferInfo.commandBufferCount = kComputeCommandBufferNum;
      }

      std::vector<vk::UniqueCommandBuffer> ComputeCommandBuffers = VkDevice_.allocateCommandBuffersUnique(CommandBufferInfo);
      assert(!ComputeCommandBuffers.empty());

      for (vdl::uint i = 0; i < kComputeCommandBufferNum; ++i)
      {
        ComputeCommandBuffers_[i] = std::move(ComputeCommandBuffers[i]);
      }
    }

    //  同期オブジェクトの作成
    {
      vk::FenceCreateInfo FenceInfo;
      vk::SemaphoreCreateInfo SemaphoreInfo;
      for (auto& ComputeSyncState : ComputeSyncStates_)
      {
        ComputeSyncState.Fence = VkDevice_.createFenceUnique(FenceInfo);
        assert(ComputeSyncState.Fence);
        FenceInfo = vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled);
        ComputeSyncState.Semaphore = VkDevice_.createSemaphoreUnique(SemaphoreInfo);
        assert(ComputeSyncState.Semaphore);
      }
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

vdl::Matrix CDeviceContext::GetNDCTransform(const vdl::float2& _DestLeftTop, const vdl::float2& _DestSize, const vdl::Radian& _Angle, const vdl::float2& _WindowSize)const
{
  return vdl::Matrix(DirectX::XMMatrixAffineTransformation2D(DirectX::XMVectorSet(_DestSize.x, _DestSize.y, 0.0f, 0.0f), DirectX::XMVectorZero(),
    _Angle, DirectX::XMVectorSet(_DestLeftTop.x + _DestSize.x * 0.5f, _DestLeftTop.y + _DestSize.y * 0.5f, 0.0f, 0.0f))
    * vdl::Matrix::Scale({ 2.0f / _WindowSize.x, -2.0f / _WindowSize.y, 1.0f }) * vdl::Matrix::Translate({ -1.0f, 1.0f, 0.0f }));
}

#pragma region GraphicsPipeline
#define SetGraphicsState(GraphicsCommandType, StateName)\
GraphicsStateChangeFlags_.Set(GraphicsCommandType);\
\
CurrentGraphicsState_.StateName = _##StateName;

#define SetGraphicsShaderStates(GraphicsCommandType, ShaderType, ShaderState)\
ShaderState##s& Current##ShaderState##s = CurrentGraphicsState_.ShaderState##s[static_cast<vdl::uint>(ShaderType)];\
\
if (const vdl::uint RequiredSize = _StartSlot + _##ShaderState##Num;\
  Current##ShaderState##s.size() < RequiredSize)\
{\
  Current##ShaderState##s.resize(static_cast<size_t>(RequiredSize));\
}\
GraphicsStateChangeFlags_.Set(GraphicsCommandType);\
\
for (vdl::uint ShaderState##Count = 0; ShaderState##Count < _##ShaderState##Num; ++ShaderState##Count)\
{\
  Current##ShaderState##s[_StartSlot + ShaderState##Count] = _##ShaderState##s[ShaderState##Count];\
}

void CDeviceContext::SetVertexBuffer(const VertexBuffer& _VertexBuffer)
{
  SetGraphicsState(GraphicsCommandType::eSetVertexBuffer, VertexBuffer)
}

void CDeviceContext::SetInstanceBuffer(const InstanceBuffer& _InstanceBuffer)
{
  SetGraphicsState(GraphicsCommandType::eSetInstanceBuffer, InstanceBuffer)
}

void CDeviceContext::SetIndexBuffer(const IndexBuffer& _IndexBuffer)
{
  SetGraphicsState(GraphicsCommandType::eSetIndexBuffer, IndexBuffer)
}

void CDeviceContext::SetInputLayout(vdl::InputLayoutType _InputLayout)
{
  SetGraphicsState(GraphicsCommandType::eSetInputLayout, InputLayout)
}

void CDeviceContext::SetTopology(vdl::TopologyType _Topology)
{
  SetGraphicsState(GraphicsCommandType::eSetTopology, Topology)
}

void CDeviceContext::SetScissor(const vdl::Scissor & _Scissor)
{
  SetGraphicsState(GraphicsCommandType::eSetScissor, Scissor)
}

void CDeviceContext::SetViewport(const vdl::Viewport & _Viewport)
{
  SetGraphicsState(GraphicsCommandType::eSetViewport, Viewport)
}

void CDeviceContext::SetRenderTextures(const vdl::RenderTextures & _RenderTextures, const vdl::DepthStencilTexture & _DepthStenilTexture)
{
  vdl::OutputManager OutputManager = { _RenderTextures, _DepthStenilTexture };

  std::vector<vdl::OutputManager>& OutputManagers = GraphicsReserveDatas_[GraphicsCommandBufferIndex_].OutputManagers;

  if (OutputManagers.empty() || OutputManagers.back() != OutputManager)
  {
    OutputManagers.emplace_back(std::move(OutputManager));
    GraphicsStateChangeFlags_.Set(GraphicsCommandType::eSetRenderTextures);
  }
}

void CDeviceContext::SetBlendState(const vdl::BlendState & _BlendState)
{
  SetGraphicsState(GraphicsCommandType::eSetBlendState, BlendState)
}

void CDeviceContext::SetDepthStencilState(const vdl::DepthStencilState & _DepthStencilState)
{
  SetGraphicsState(GraphicsCommandType::eSetDepthStencilState, DepthStencilState)
}

void CDeviceContext::SetRasterizerState(const vdl::RasterizerState & _RasterizerState)
{
  SetGraphicsState(GraphicsCommandType::eSetRasterizerState, RasterizerState)
}

void CDeviceContext::VSSetShader(const vdl::VertexShader & _VertexShader)
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

void CDeviceContext::HSSetShader(const vdl::HullShader & _HullShader)
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

void CDeviceContext::DSSetShader(const vdl::DomainShader & _DomainShader)
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

void CDeviceContext::GSSetShader(const vdl::GeometryShader & _GeometryShader)
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

void CDeviceContext::PSSetShader(const vdl::PixelShader & _PixelShader)
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
#define SetComputeState(ComputeCommandType, StateName)\
ComputeStateChangeFlags_.Set(ComputeCommandType);\
\
CurrentComputeState_.StateName = _##StateName;

#define SetComputeShaderStates(ComputeCommandType, ShaderState)\
ComputeStateChangeFlags_.Set(ComputeCommandType);\
\
ShaderState##s& Current##ShaderState##s = CurrentComputeState_.ShaderState##s;\
\
if (const vdl::uint RequiredSize = _StartSlot + _##ShaderState##Num;\
  Current##ShaderState##s.size() < RequiredSize)\
{\
  Current##ShaderState##s.resize(static_cast<size_t>(RequiredSize));\
}\
\
for (vdl::uint ShaderState##Count = 0; ShaderState##Count < _##ShaderState##Num; ++ShaderState##Count)\
{\
  Current##ShaderState##s[_StartSlot + ShaderState##Count] = _##ShaderState##s[ShaderState##Count];\
}

void CDeviceContext::CSSetShader(const vdl::ComputeShader& _ComputeShader)
{
  SetComputeState(ComputeCommandType::eSetComputeShader, ComputeShader)
}

void CDeviceContext::CSSetShaderResources(vdl::uint _StartSlot, vdl::uint _ShaderResourceNum, const vdl::ShaderResource _ShaderResources[])
{
  SetComputeShaderStates(ComputeCommandType::eSetShaderResource, ShaderResource)
}

void CDeviceContext::CSSetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[])
{
  SetComputeShaderStates(ComputeCommandType::eSetShaderResource, Sampler)
}

void CDeviceContext::CSSetConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[])
{
  SetComputeShaderStates(ComputeCommandType::eSetConstantBuffer, ConstantBuffer)
}

void CDeviceContext::CSSetUnorderedAccessObjects(vdl::uint _StartSlot, vdl::uint _UnorderedAccessObjectNum, const vdl::UnorderedAccessObject _UnorderedAccessObjects[])
{
  SetComputeShaderStates(ComputeCommandType::eSetUnorderedAccessObject, UnorderedAccessObject)
}

#undef SetComputeState
#pragma endregion

void CDeviceContext::ClearRenderTexture(const vdl::RenderTexture& _RenderTexture, const vdl::ColorF& _ClearColor)
{
  BeginGraphicsCommandBuffer();

  const vk::CommandBuffer& CurrentGraphicsCommandBuffer = GetCurrentGraphicsCommandBuffer();

  const vdl::ID TextureID = _RenderTexture.GetID();
  CRenderTexture* pRenderTexture = (TextureID ? static_cast<CRenderTexture*>(pTextureManager_->GetTexture(TextureID)) : pSwapChain_->GetRenderTexture());
  {
    std::unordered_map<vdl::ID, vdl::Texture>& ClearTextures = GraphicsReserveDatas_[GraphicsCommandBufferIndex_].ClearTextures;

    if (ClearTextures.find(TextureID) == ClearTextures.end())
    {
      ClearTextures.insert(std::make_pair(TextureID, _RenderTexture));
    }
  }

  const vk::ImageSubresourceRange SubresourceRange = { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 };
  pRenderTexture->SetImageLayout(CurrentGraphicsCommandBuffer, vk::ImageLayout::eTransferDstOptimal, SubresourceRange);
  CurrentGraphicsCommandBuffer.clearColorImage(pRenderTexture->Image.get(), vk::ImageLayout::eTransferDstOptimal, Cast(_ClearColor), SubresourceRange);
}

void CDeviceContext::ClearDepthStencilTexture(const vdl::DepthStencilTexture& _DepthStencilTexture, float _ClearDepth, vdl::uint _ClearStencil)
{
  BeginGraphicsCommandBuffer();

  const vk::CommandBuffer& CurrentGraphicsCommandBuffer = GetCurrentGraphicsCommandBuffer();

  const vdl::ID TextureID = _DepthStencilTexture.GetID();
  CDepthStencilTexture* pDepthStencilTexture = (TextureID ? static_cast<CDepthStencilTexture*>(pTextureManager_->GetTexture(TextureID)) : pSwapChain_->GetDepthStencilTexture());
  {
    std::unordered_map<vdl::ID, vdl::Texture>& ClearTextures = GraphicsReserveDatas_[GraphicsCommandBufferIndex_].ClearTextures;

    if (ClearTextures.find(TextureID) == ClearTextures.end())
    {
      ClearTextures.insert(std::make_pair(TextureID, _DepthStencilTexture));
    }
  }

  const vk::ImageSubresourceRange SubresourceRange = { pDepthStencilTexture->ImageAspectFlag, 0, 1, 0, 1 };
  pDepthStencilTexture->SetImageLayout(CurrentGraphicsCommandBuffer, vk::ImageLayout::eTransferDstOptimal, SubresourceRange);
  CurrentGraphicsCommandBuffer.clearDepthStencilImage(pDepthStencilTexture->Image.get(), vk::ImageLayout::eTransferDstOptimal, { _ClearDepth, _ClearStencil }, SubresourceRange);
}

void CDeviceContext::ClearUnorderedAccessTexture(const vdl::UnorderedAccessTexture& _UnorderedAccessTexture, const vdl::ColorF& _ClearColor)
{
  assert(!_UnorderedAccessTexture.isEmpty());

  constexpr vk::ImageLayout kImageLayout = vk::ImageLayout::eTransferDstOptimal;

  BeginGraphicsCommandBuffer();

  const vk::CommandBuffer& CurrentGraphicsCommandBuffer = GetCurrentGraphicsCommandBuffer();
  const vk::ImageSubresourceRange SubresourceRange = { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 };

  const vdl::ID TextureID = _UnorderedAccessTexture.GetID();
  CUnorderedAccessTexture* pUnorderedAccessTexture = static_cast<CUnorderedAccessTexture*>(pTextureManager_->GetTexture(TextureID));
  {
    std::unordered_map<vdl::ID, vdl::Texture>& ClearTextures = GraphicsReserveDatas_[GraphicsCommandBufferIndex_].ClearTextures;

    if (ClearTextures.find(TextureID) == ClearTextures.end())
    {
      ClearTextures.insert(std::make_pair(TextureID, _UnorderedAccessTexture));
    }
  }

  pUnorderedAccessTexture->SetImageLayout(CurrentGraphicsCommandBuffer, vk::ImageLayout::eTransferDstOptimal, SubresourceRange);
  CurrentGraphicsCommandBuffer.clearColorImage(pUnorderedAccessTexture->Image.get(), kImageLayout, Cast(_ClearColor), SubresourceRange);
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
  const vk::CommandBuffer& CurrentCommandBuffer = GetCurrentComputeCommandBuffer();
  ComputeReserveData& CurrentReserveData = ComputeReserveDatas_[ComputeCommandBufferIndex_];

  CurrentCommandBuffer.reset(vk::CommandBufferResetFlags());
  CurrentCommandBuffer.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));

  //  シェーダーリソースのバインド
  {
    const ShaderResources& ShaderResources = CurrentComputeState_.ShaderResources;

    std::vector<DescriptorImageData> TextureDatas;
    std::vector<DescriptorBufferData> UnorderedAccessBufferDatas;

    //  データの読み込み
    {
      const vdl::uint ShaderResourceNum = static_cast<vdl::uint>(ShaderResources.size());
      for (vdl::uint ShaderResourceCount = 0; ShaderResourceCount < ShaderResourceNum; ++ShaderResourceCount)
      {
        const vdl::ShaderResource& ShaderResource = ShaderResources[ShaderResourceCount];

        //  Texture
        if (std::get_if<vdl::Texture>(&ShaderResource))
        {
          const vdl::Texture& Texture = std::get<vdl::Texture>(ShaderResource);
          if (!Texture.isEmpty())
          {
            constexpr vk::ImageLayout kImageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

            CTexture* pTexture = static_cast<CTexture*>(pTextureManager_->GetTexture(Texture.GetID()));
            if (pTexture->CurrentLayout != kImageLayout)
            {
              if (pTexture->GetType() == TextureType::eDepthStencilTexture)
              {
                pTexture->SetImageLayout(CurrentCommandBuffer, kImageLayout, { static_cast<CDepthStencilTexture*>(pTexture)->ImageAspectFlag, 0, 1, 0, 1 });
              }
              else
              {
                pTexture->SetImageLayout(CurrentCommandBuffer, kImageLayout, { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 });
              }
            }

            DescriptorImageData& ImageData = TextureDatas.emplace_back();
            {
              ImageData.Info.imageView = pTexture->View.get();
              ImageData.Info.imageLayout = kImageLayout;
              ImageData.Bind = ShaderResourceCount;
            }
          }
        }
        //  UnorderedAccessBuffer
        else if (std::get_if<vdl::Detail::UnorderedAccessBufferData>(&ShaderResource))
        {
          const vdl::Detail::UnorderedAccessBufferData& UnorderedAccessBuffer = std::get<vdl::Detail::UnorderedAccessBufferData>(ShaderResource);

          if (!UnorderedAccessBuffer.isEmpty())
          {
            assert(pBufferManager_->GetBuffer(UnorderedAccessBuffer.GetID())->GetType() == BufferType::eUnorderedAccessBuffer);

            CUnordererdAccessBuffer* pUnorderedAccessBuffer = static_cast<CUnordererdAccessBuffer*>(pBufferManager_->GetBuffer(UnorderedAccessBuffer.GetID()));
            DescriptorBufferData& BufferData = UnorderedAccessBufferDatas.emplace_back();
            {
              BufferData.Info.buffer = pUnorderedAccessBuffer->BufferData.Buffer.get();
              BufferData.Info.offset = 0;
              BufferData.Info.range = pUnorderedAccessBuffer->BufferSize;
              BufferData.Bind = ShaderResourceCount;
            }
          }
        }
      }
    }

    bool isEmpty = true;

    if (!TextureDatas.empty())
    {
      constexpr DescriptorType kDescriptorType = DescriptorType::eTexture;

      vk::DescriptorSetAllocateInfo DescriptorSetAllocateInfo;
      {
        DescriptorSetAllocateInfo.descriptorPool = ComputeDescriptorPool_.get();
        DescriptorSetAllocateInfo.descriptorSetCount = 1;
        DescriptorSetAllocateInfo.pSetLayouts = &ComputeDescriptorLayouts_[static_cast<vdl::uint>(kDescriptorType)].get();
      }

      vk::UniqueDescriptorSet DescriptorSet = std::move(VkDevice_.allocateDescriptorSetsUnique(DescriptorSetAllocateInfo).front());
      assert(DescriptorSet);

      std::vector<vk::WriteDescriptorSet> WriteDescriptorSets;
      {
        for (auto& TextureData : TextureDatas)
        {
          vk::WriteDescriptorSet& WriteDescriptorSet = WriteDescriptorSets.emplace_back();
          {
            WriteDescriptorSet.dstSet = DescriptorSet.get();
            WriteDescriptorSet.dstBinding = TextureData.Bind;
            WriteDescriptorSet.dstArrayElement = 0;
            WriteDescriptorSet.descriptorCount = 1;
            WriteDescriptorSet.descriptorType = Cast(kDescriptorType);
            WriteDescriptorSet.pImageInfo = &TextureData.Info;
          }
        }
      }

      VkDevice_.updateDescriptorSets(WriteDescriptorSets, nullptr);

      CurrentCommandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eCompute, PipelineLayout_.get(), GetDescriptorLayoutOffset(ShaderType::eComputeShader, kDescriptorType), DescriptorSet.get(), nullptr);

      CurrentReserveData.DescriptorSets.emplace_back(std::move(DescriptorSet));

      isEmpty = false;
    }

    if (!UnorderedAccessBufferDatas.empty())
    {
      constexpr DescriptorType kDescriptorType = DescriptorType::eBuffer;

      vk::DescriptorSetAllocateInfo DescriptorSetAllocateInfo;
      {
        DescriptorSetAllocateInfo.descriptorPool = ComputeDescriptorPool_.get();
        DescriptorSetAllocateInfo.descriptorSetCount = 1;
        DescriptorSetAllocateInfo.pSetLayouts = &ComputeDescriptorLayouts_[static_cast<vdl::uint>(kDescriptorType)].get();
      }

      vk::UniqueDescriptorSet DescriptorSet = std::move(VkDevice_.allocateDescriptorSetsUnique(DescriptorSetAllocateInfo).front());
      assert(DescriptorSet);

      std::vector<vk::WriteDescriptorSet> WriteDescriptorSets;
      {
        for (auto& UnorderedAccessBufferData : UnorderedAccessBufferDatas)
        {
          vk::WriteDescriptorSet& WriteDescriptorSet = WriteDescriptorSets.emplace_back();
          {
            WriteDescriptorSet.dstSet = DescriptorSet.get();
            WriteDescriptorSet.dstBinding = UnorderedAccessBufferData.Bind;
            WriteDescriptorSet.dstArrayElement = 0;
            WriteDescriptorSet.descriptorCount = 1;
            WriteDescriptorSet.descriptorType = Cast(kDescriptorType);
            WriteDescriptorSet.pBufferInfo = &UnorderedAccessBufferData.Info;
          }
        }
      }

      VkDevice_.updateDescriptorSets(WriteDescriptorSets, nullptr);

      CurrentCommandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eCompute, PipelineLayout_.get(), GetDescriptorLayoutOffset(ShaderType::eComputeShader, kDescriptorType), DescriptorSet.get(), nullptr);

      CurrentReserveData.DescriptorSets.emplace_back(std::move(DescriptorSet));

      isEmpty = false;
    }

    if (!isEmpty)
    {
      CurrentReserveData.ShaderResources = CurrentComputeState_.ShaderResources;
    }
  }

  //  サンプラーのバインド
  {
    const Samplers& Samplers = CurrentComputeState_.Samplers;

    std::vector<DescriptorImageData> SamplerDatas;

    //  データの読み込み
    {
      const vdl::uint SamplerNum = static_cast<vdl::uint>(Samplers.size());
      for (vdl::uint SamplerCount = 0; SamplerCount < SamplerNum; ++SamplerCount)
      {
        DescriptorImageData& ImageData = SamplerDatas.emplace_back();
        {
          ImageData.Info.sampler = GetSampler(Samplers[SamplerCount]);
          ImageData.Info.imageLayout = vk::ImageLayout::eUndefined;
          ImageData.Bind = SamplerCount;
        }
      }
    }

    if (!SamplerDatas.empty())
    {
      constexpr DescriptorType kDescriptorType = DescriptorType::eSampler;

      vk::DescriptorSetAllocateInfo DescriptorSetAllocateInfo;
      {
        DescriptorSetAllocateInfo.descriptorPool = GraphicsDescriptorPool_.get();
        DescriptorSetAllocateInfo.descriptorSetCount = 1;
        DescriptorSetAllocateInfo.pSetLayouts = &ComputeDescriptorLayouts_[static_cast<vdl::uint>(kDescriptorType)].get();
      }

      vk::UniqueDescriptorSet DescriptorSet = std::move(VkDevice_.allocateDescriptorSetsUnique(DescriptorSetAllocateInfo).front());

      std::vector<vk::WriteDescriptorSet> WriteDescriptorSets;
      {
        for (auto& SamplerData : SamplerDatas)
        {
          vk::WriteDescriptorSet& WriteDescriptorSet = WriteDescriptorSets.emplace_back();
          {
            WriteDescriptorSet.dstSet = DescriptorSet.get();
            WriteDescriptorSet.dstBinding = SamplerData.Bind;
            WriteDescriptorSet.dstArrayElement = 0;
            WriteDescriptorSet.descriptorCount = 1;
            WriteDescriptorSet.descriptorType = Cast(DescriptorType::eSampler);
            WriteDescriptorSet.pImageInfo = &SamplerData.Info;
          }
        }
      }

      VkDevice_.updateDescriptorSets(WriteDescriptorSets, nullptr);

      CurrentCommandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eCompute, PipelineLayout_.get(), GetDescriptorLayoutOffset(ShaderType::eComputeShader, kDescriptorType), DescriptorSet.get(), nullptr);

      CurrentReserveData.DescriptorSets.emplace_back(std::move(DescriptorSet));
    }
  }

  //  定数バッファのバインド
  {
    const ConstantBuffers& ConstantBuffers = CurrentComputeState_.ConstantBuffers;

    std::vector<DescriptorBufferData> ConstantBufferDatas;

    //  データの読み込み
    {
      const vdl::uint ConstantBufferNum = static_cast<vdl::uint>(ConstantBuffers.size());
      for (vdl::uint ConstantBufferCount = 0; ConstantBufferCount < ConstantBufferNum; ++ConstantBufferCount)
      {
        assert(pBufferManager_->GetBuffer(ConstantBuffers[ConstantBufferCount].GetID())->GetType() == BufferType::eCopyConstantBuffer);
        CCopyConstantBuffer* pConstantBuffer = static_cast<CCopyConstantBuffer*>(pBufferManager_->GetBuffer(ConstantBuffers[ConstantBufferCount].GetID()));

        DescriptorBufferData& BufferData = ConstantBufferDatas.emplace_back();
        {
          BufferData.Info.buffer = pConstantBuffer->ParentBuffer;
          BufferData.Info.offset = pConstantBuffer->Offset;
          BufferData.Info.range = pConstantBuffer->BufferSize;
          BufferData.Bind = ConstantBufferCount;
        }
      }
    }

    if (!ConstantBufferDatas.empty())
    {
      constexpr DescriptorType kDescriptorType = DescriptorType::eConstantBuffer;

      vk::DescriptorSetAllocateInfo DescriptorSetAllocateInfo;
      {
        DescriptorSetAllocateInfo.descriptorPool = ComputeDescriptorPool_.get();
        DescriptorSetAllocateInfo.descriptorSetCount = 1;
        DescriptorSetAllocateInfo.pSetLayouts = &ComputeDescriptorLayouts_[static_cast<vdl::uint>(kDescriptorType)].get();
      }

      vk::UniqueDescriptorSet DescriptorSet = std::move(VkDevice_.allocateDescriptorSetsUnique(DescriptorSetAllocateInfo).front());

      std::vector<vk::WriteDescriptorSet> WriteDescriptorSets;
      {
        for (auto& ConstantBufferData : ConstantBufferDatas)
        {
          vk::WriteDescriptorSet& WriteDescriptorSet = WriteDescriptorSets.emplace_back();
          {
            WriteDescriptorSet.dstSet = DescriptorSet.get();
            WriteDescriptorSet.dstBinding = ConstantBufferData.Bind;
            WriteDescriptorSet.dstArrayElement = 0;
            WriteDescriptorSet.descriptorCount = 1;
            WriteDescriptorSet.descriptorType = Cast(DescriptorType::eConstantBuffer);
            WriteDescriptorSet.pBufferInfo = &ConstantBufferData.Info;
          }
        }
      }

      VkDevice_.updateDescriptorSets(WriteDescriptorSets, nullptr);

      CurrentCommandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eCompute, PipelineLayout_.get(), GetDescriptorLayoutOffset(ShaderType::eComputeShader, kDescriptorType), DescriptorSet.get(), nullptr);

      CurrentReserveData.DescriptorSets.emplace_back(std::move(DescriptorSet));

      CurrentReserveData.ConstantBuffers = ConstantBuffers;
    }
  }

  //  順不同アクセスオブジェクトのバインド
  {
    const UnorderedAccessObjects& UnorderedAccessObjects = CurrentComputeState_.UnorderedAccessObjects;

    std::vector<DescriptorImageData> UnorderedAccessTextureDatas;
    std::vector<DescriptorBufferData> UnorderedAccessBufferDatas;

    //  データの読み込み
    {
      const vdl::uint UnorderedAccessObjectNum = static_cast<vdl::uint>(UnorderedAccessObjects.size());
      for (vdl::uint UnorderedAccessObjectCount = 0; UnorderedAccessObjectCount < UnorderedAccessObjectNum; ++UnorderedAccessObjectCount)
      {
        const vdl::UnorderedAccessObject& UnorderedAccessObject = UnorderedAccessObjects[UnorderedAccessObjectCount];

        //  UnorderedAccessTexture
        if (std::get_if<vdl::UnorderedAccessTexture>(&UnorderedAccessObject))
        {
          const vdl::UnorderedAccessTexture& UnorderedAccessTexture = std::get<vdl::UnorderedAccessTexture>(UnorderedAccessObject);
          if (!UnorderedAccessTexture.isEmpty())
          {
            constexpr vk::ImageLayout kImageLayout = vk::ImageLayout::eGeneral;

            assert(pTextureManager_->GetTexture(UnorderedAccessTexture.GetID())->GetType() == TextureType::eUnorderedAccessTexture);
            CTexture* pTexture = static_cast<CTexture*>(pTextureManager_->GetTexture(UnorderedAccessTexture.GetID()));
            if (pTexture->CurrentLayout != kImageLayout)
            {
              pTexture->SetImageLayout(CurrentCommandBuffer, kImageLayout, { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 });
            }

            DescriptorImageData& ImageData = UnorderedAccessTextureDatas.emplace_back();
            {
              ImageData.Info.imageView = pTexture->View.get();
              ImageData.Info.imageLayout = kImageLayout;
              ImageData.Bind = UnorderedAccessObjectCount;
            }
          }
        }
        //  UnorderedAccessBuffer
        else if (std::get_if<vdl::Detail::UnorderedAccessBufferData>(&UnorderedAccessObject))
        {
          const vdl::Detail::UnorderedAccessBufferData& UnorderedAccessBuffer = std::get<vdl::Detail::UnorderedAccessBufferData>(UnorderedAccessObject);

          if (!UnorderedAccessBuffer.isEmpty())
          {
            assert(pBufferManager_->GetBuffer(UnorderedAccessBuffer.GetID())->GetType() == BufferType::eUnorderedAccessBuffer);

            CUnordererdAccessBuffer* pUnorderedAccessBuffer = static_cast<CUnordererdAccessBuffer*>(pBufferManager_->GetBuffer(UnorderedAccessBuffer.GetID()));
            DescriptorBufferData& BufferData = UnorderedAccessBufferDatas.emplace_back();
            {
              BufferData.Info.buffer = pUnorderedAccessBuffer->BufferData.Buffer.get();
              BufferData.Info.offset = 0;
              BufferData.Info.range = pUnorderedAccessBuffer->BufferSize;
              BufferData.Bind = UnorderedAccessObjectCount;
            }
          }
        }
      }
    }

    bool isEmpty = true;

    if (!UnorderedAccessTextureDatas.empty())
    {
      constexpr DescriptorType kDescriptorType = DescriptorType::eUnorderedAccessTexture;

      vk::DescriptorSetAllocateInfo DescriptorSetAllocateInfo;
      {
        DescriptorSetAllocateInfo.descriptorPool = ComputeDescriptorPool_.get();
        DescriptorSetAllocateInfo.descriptorSetCount = 1;
        DescriptorSetAllocateInfo.pSetLayouts = &ComputeDescriptorLayouts_[static_cast<vdl::uint>(kDescriptorType)].get();
      }

      vk::UniqueDescriptorSet DescriptorSet = std::move(VkDevice_.allocateDescriptorSetsUnique(DescriptorSetAllocateInfo).front());
      assert(DescriptorSet);

      std::vector<vk::WriteDescriptorSet> WriteDescriptorSets;
      {
        for (auto& UnorderedAccessTextureData : UnorderedAccessTextureDatas)
        {
          vk::WriteDescriptorSet& WriteDescriptorSet = WriteDescriptorSets.emplace_back();
          {
            WriteDescriptorSet.dstSet = DescriptorSet.get();
            WriteDescriptorSet.dstBinding = UnorderedAccessTextureData.Bind;
            WriteDescriptorSet.dstArrayElement = 0;
            WriteDescriptorSet.descriptorCount = 1;
            WriteDescriptorSet.descriptorType = Cast(kDescriptorType);
            WriteDescriptorSet.pImageInfo = &UnorderedAccessTextureData.Info;
          }
        }
      }

      VkDevice_.updateDescriptorSets(WriteDescriptorSets, nullptr);

      CurrentCommandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eCompute, PipelineLayout_.get(), GetDescriptorLayoutOffset(ShaderType::eComputeShader, kDescriptorType), DescriptorSet.get(), nullptr);

      CurrentReserveData.DescriptorSets.emplace_back(std::move(DescriptorSet));

      isEmpty = false;
    }

    if (!UnorderedAccessBufferDatas.empty())
    {
      constexpr DescriptorType kDescriptorType = DescriptorType::eUnorderedAccessBuffer;

      vk::DescriptorSetAllocateInfo DescriptorSetAllocateInfo;
      {
        DescriptorSetAllocateInfo.descriptorPool = ComputeDescriptorPool_.get();
        DescriptorSetAllocateInfo.descriptorSetCount = 1;
        DescriptorSetAllocateInfo.pSetLayouts = &ComputeDescriptorLayouts_[static_cast<vdl::uint>(kDescriptorType)].get();
      }

      vk::UniqueDescriptorSet DescriptorSet = std::move(VkDevice_.allocateDescriptorSetsUnique(DescriptorSetAllocateInfo).front());
      assert(DescriptorSet);

      std::vector<vk::WriteDescriptorSet> WriteDescriptorSets;
      {
        for (auto& UnorderedAccessBufferData : UnorderedAccessBufferDatas)
        {
          vk::WriteDescriptorSet& WriteDescriptorSet = WriteDescriptorSets.emplace_back();
          {
            WriteDescriptorSet.dstSet = DescriptorSet.get();
            WriteDescriptorSet.dstBinding = UnorderedAccessBufferData.Bind;
            WriteDescriptorSet.dstArrayElement = 0;
            WriteDescriptorSet.descriptorCount = 1;
            WriteDescriptorSet.descriptorType = Cast(kDescriptorType);
            WriteDescriptorSet.pBufferInfo = &UnorderedAccessBufferData.Info;
          }
        }
      }

      VkDevice_.updateDescriptorSets(WriteDescriptorSets, nullptr);

      CurrentCommandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eCompute, PipelineLayout_.get(), GetDescriptorLayoutOffset(ShaderType::eComputeShader, kDescriptorType), DescriptorSet.get(), nullptr);

      CurrentReserveData.DescriptorSets.emplace_back(std::move(DescriptorSet));

      isEmpty = false;
    }

    if (!isEmpty)
    {
      CurrentReserveData.UnorderedAccessObjects = CurrentComputeState_.UnorderedAccessObjects;
    }
  }

  //  パイプラインのバインド
  {
    assert(pShaderManager_->GetShader(CurrentComputeState_.ComputeShader.GetID())->GetType() == ShaderType::eComputeShader);
    const CComputeShader* pComputeShader = static_cast<CComputeShader*>(pShaderManager_->GetShader(CurrentComputeState_.ComputeShader.GetID()));

    vk::PipelineShaderStageCreateInfo PipelineShaderStageInfo;
    {
      PipelineShaderStageInfo.stage = vk::ShaderStageFlagBits::eCompute;
      PipelineShaderStageInfo.module = pComputeShader->ShaderData.Module.get();
      PipelineShaderStageInfo.pName = pComputeShader->ShaderData.EntryPoint;
    }

    vk::ComputePipelineCreateInfo ComputePipelineInfo;
    {
      ComputePipelineInfo.flags = (LastComputePipeline_ == VK_NULL_HANDLE ? vk::PipelineCreateFlagBits::eAllowDerivatives : vk::PipelineCreateFlagBits::eAllowDerivatives | vk::PipelineCreateFlagBits::eDerivative);
      ComputePipelineInfo.stage = PipelineShaderStageInfo;
      ComputePipelineInfo.layout = PipelineLayout_.get();
      ComputePipelineInfo.basePipelineHandle = LastComputePipeline_;
      ComputePipelineInfo.basePipelineIndex = (LastComputePipeline_ == VK_NULL_HANDLE ? 0 : -1);
    }

    CurrentReserveData.Pipeline = VkDevice_.createComputePipelineUnique(ComputePipelineCache_.get(), ComputePipelineInfo);
    assert(CurrentReserveData.Pipeline);

    LastComputePipeline_ = CurrentReserveData.Pipeline.get();
    CurrentCommandBuffer.bindPipeline(vk::PipelineBindPoint::eCompute, LastComputePipeline_);
  }

  CurrentCommandBuffer.dispatch(_ThreadGroupX, _ThreadGroupY, _ThreadGroupZ);

  CurrentCommandBuffer.end();

  std::vector<vk::Semaphore> WaitSemaphores;
  {
    if (LastSemaphore_)
    {
      WaitSemaphores.push_back(LastSemaphore_);
    }
    if (const vk::Semaphore PresentSemaphore = pSwapChain_->GetSemaphore())
    {
      WaitSemaphores.push_back(PresentSemaphore);
    }
  }

  const vdl::uint WaitSemaphoreNum = static_cast<vdl::uint>(WaitSemaphores.size());
  std::vector<vk::PipelineStageFlags> PipelineStageFlags(WaitSemaphoreNum, vk::PipelineStageFlagBits::eBottomOfPipe);

  const vk::Semaphore& CurrentSemaphore = GetCurrentComputeSemaphore();

  vk::SubmitInfo SubmitInfo;
  {
    SubmitInfo.waitSemaphoreCount = WaitSemaphoreNum;
    SubmitInfo.pWaitSemaphores = WaitSemaphores.data();
    SubmitInfo.pWaitDstStageMask = PipelineStageFlags.data();
    SubmitInfo.commandBufferCount = 1;
    SubmitInfo.pCommandBuffers = &CurrentCommandBuffer;
    SubmitInfo.signalSemaphoreCount = 1;
    SubmitInfo.pSignalSemaphores = &CurrentSemaphore;
  }

  ComputeQueue_.submit(SubmitInfo, GetCurrentComputeFence());

  LastSemaphore_ = CurrentSemaphore;

  const vdl::uint CurrentComputeCommandBufferIndex = ComputeCommandBufferIndex_;

  ++ComputeCommandBufferIndex_ %= kComputeCommandBufferNum;
  WaitFence(GetCurrentComputeFence());

  ComputeReserveDatas_[ComputeCommandBufferIndex_].Clear();
  ComputeStateChangeFlags_.Clear();
}

void CDeviceContext::Flush()
{
  const vk::CommandBuffer& CurrentCommandBuffer = GetCurrentGraphicsCommandBuffer();

  switch (GraphicsCommandBufferState_)
  {
  case CDeviceContext::CommandBufferState::eIdle:
    return;
  case CDeviceContext::CommandBufferState::eBeginRenderPass:
    CurrentCommandBuffer.endRenderPass();
  case CDeviceContext::CommandBufferState::eBegin:
    CurrentCommandBuffer.end();
    break;
  default: assert(false);
  }

  std::vector<vk::Semaphore> WaitSemaphores;
  {
    if (LastSemaphore_)
    {
      WaitSemaphores.push_back(LastSemaphore_);
    }
    if (const vk::Semaphore PresentSemaphore = pSwapChain_->GetSemaphore())
    {
      WaitSemaphores.push_back(PresentSemaphore);
    }
  }

  const vk::Semaphore& CurrentSemaphore = GetCurrentGraphicsSemaphore();
  const vdl::uint WaitSemaphoreNum = static_cast<vdl::uint>(WaitSemaphores.size());
  std::vector<vk::PipelineStageFlags> PipelineStageFlags(WaitSemaphoreNum, vk::PipelineStageFlagBits::eBottomOfPipe);

  vk::SubmitInfo SubmitInfo;
  {
    SubmitInfo.waitSemaphoreCount = WaitSemaphoreNum;
    SubmitInfo.pWaitSemaphores = WaitSemaphores.data();
    SubmitInfo.pWaitDstStageMask = PipelineStageFlags.data();
    SubmitInfo.commandBufferCount = 1;
    SubmitInfo.pCommandBuffers = &CurrentCommandBuffer;
    SubmitInfo.signalSemaphoreCount = 1;
    SubmitInfo.pSignalSemaphores = &CurrentSemaphore;
  }

  GraphicsQueue_.submit(SubmitInfo, GetCurrentGraphicsFence());

  LastSemaphore_ = CurrentSemaphore;

  const vdl::uint CurrentGraphicsCommandBufferIndex = GraphicsCommandBufferIndex_;

  ++GraphicsCommandBufferIndex_ %= kGraphicsCommandBufferNum;
  WaitFence(GetCurrentGraphicsFence());
  GetCurrentGraphicsCommandBuffer().reset(vk::CommandBufferResetFlags());

  GraphicsReserveDatas_[GraphicsCommandBufferIndex_].Clear();
  GraphicsReserveDatas_[GraphicsCommandBufferIndex_].Pipelines.resize(1);
  GraphicsReserveDatas_[GraphicsCommandBufferIndex_].Pipelines[0] = std::move(GraphicsReserveDatas_[CurrentGraphicsCommandBufferIndex].Pipelines.back());

  GraphicsCommandBufferState_ = CommandBufferState::eIdle;
}

//--------------------------------------------------

void CDeviceContext::Present()
{
  const vk::CommandBuffer& CurrentCommandBuffer = GetCurrentGraphicsCommandBuffer();

  //  コマンドバッファの状態を開始状態にする
  {
    switch (GraphicsCommandBufferState_)
    {
    case CDeviceContext::CommandBufferState::eIdle:
    {
      CurrentCommandBuffer.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
    }
    break;
    case CDeviceContext::CommandBufferState::eBeginRenderPass:
    {
      CurrentCommandBuffer.endRenderPass();
    }
    break;
    }

    GraphicsCommandBufferState_ = CommandBufferState::eBegin;
  }

  //  バックバッファをPresent可能状態に変更
  pSwapChain_->GetRenderTexture()->SetImageLayout(CurrentCommandBuffer, vk::ImageLayout::ePresentSrcKHR, { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 });

  Flush();

  vk::PresentInfoKHR PresentInfo;
  {
    PresentInfo.waitSemaphoreCount = 1;
    PresentInfo.pWaitSemaphores = &LastSemaphore_;
    PresentInfo.swapchainCount = 1;
    PresentInfo.pSwapchains = &pSwapChain_->GetSwapChain();
    PresentInfo.pImageIndices = &pSwapChain_->GetCurrentBufferIndex();
  }

  vk::Result Result = vk::Result::eSuccess;
  Result = GraphicsQueue_.presentKHR(PresentInfo);
  assert(Result == vk::Result::eSuccess);

  LastSemaphore_ = vk::Semaphore();
}

//--------------------------------------------------

void CDeviceContext::BeginGraphicsCommandBuffer()
{
  if (GraphicsCommandBufferState_ == CommandBufferState::eBegin)
  {
    return;
  }

  const vk::CommandBuffer& CurrentGraphicsCommandBuffer = GetCurrentGraphicsCommandBuffer();

  if (GraphicsCommandBufferState_ == CommandBufferState::eIdle)
  {
    CurrentGraphicsCommandBuffer.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
  }
  else if (GraphicsCommandBufferState_ == CommandBufferState::eBeginRenderPass)
  {
    CurrentGraphicsCommandBuffer.endRenderPass();
  }

  GraphicsCommandBufferState_ = CommandBufferState::eBegin;
}

void CDeviceContext::BeginRenderPassGraphicsCommandBuffer()
{
  if (GraphicsCommandBufferState_ == CommandBufferState::eBeginRenderPass)
  {
    return;
  }

  const vk::CommandBuffer& CurrentGraphicsCommandBuffer = GetCurrentGraphicsCommandBuffer();

  if (GraphicsCommandBufferState_ == CommandBufferState::eIdle)
  {
    CurrentGraphicsCommandBuffer.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
    GraphicsCommandBufferState_ = CommandBufferState::eBegin;
  }
  if (GraphicsCommandBufferState_ == CommandBufferState::eBegin)
  {
    vdl::OutputManager& CurrentOutputManager = GraphicsReserveDatas_[GraphicsCommandBufferIndex_].OutputManagers.back();
    RenderPassData& RenderPassData = GraphicsReserveDatas_[GraphicsCommandBufferIndex_].RenderPassDatas.emplace_back();

    vdl::uint2 RenderTextureSize;

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
            assert(RenderTextureSize == _RenderTexture.GetSize());

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

          if (CurrentOutputManager.RenderTextures[0].isEmpty())
          {
            RenderTextureSize = pWindow_->GetWindowSize();
            SetAttachment(pSwapChain_->GetRenderTexture());
          }
          else
          {
            RenderTextureSize = CurrentOutputManager.RenderTextures[0].GetSize();
            SetAttachment(GetRenderTexture(CurrentOutputManager.RenderTextures[0]));
          }

          ++AttachmentCount;

          for (; AttachmentCount < Constants::kMaxRenderTextureNum; ++AttachmentCount)
          {
            const vdl::RenderTexture RenderTexture = CurrentOutputManager.RenderTextures[AttachmentCount];
            if (RenderTexture.isEmpty())
            {
              break;
            }

            SetAttachment(GetRenderTexture(RenderTexture));
          }
        }

        //  デプスステンシルバッファの設定
        {
          CDepthStencilTexture* pDepthStencilTexture = (CurrentOutputManager.DepthStencilTexture.isEmpty() ? pSwapChain_->GetDepthStencilTexture()
            : static_cast<CDepthStencilTexture*>(pTextureManager_->GetTexture(CurrentOutputManager.DepthStencilTexture.GetID())));

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

      GraphicsColorAttachmentCount_ = AttachmentCount - 1;
      vk::SubpassDescription SubpassDescription;
      {
        SubpassDescription.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
        SubpassDescription.colorAttachmentCount = GraphicsColorAttachmentCount_;
        SubpassDescription.pColorAttachments = AttachmentReferences.data();
        SubpassDescription.pDepthStencilAttachment = &AttachmentReferences[GraphicsColorAttachmentCount_];
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
        FramebufferInfo.width = RenderTextureSize.x;
        FramebufferInfo.height = RenderTextureSize.y;
        FramebufferInfo.layers = 1;
      }

      RenderPassData.Framebuffer = VkDevice_.createFramebufferUnique(FramebufferInfo);
      assert(RenderPassData.Framebuffer);
    }

    CurrentGraphicsCommandBuffer.beginRenderPass({ RenderPassData.RenderPass.get(), RenderPassData.Framebuffer.get(),
      { { 0, 0 }, { RenderTextureSize.x, RenderTextureSize.y } } }, vk::SubpassContents::eInline);

    GraphicsStateChangeFlags_.Cancel(GraphicsCommandType::eSetRenderTextures);
  }

  GraphicsCommandBufferState_ = CommandBufferState::eBeginRenderPass;
}

void CDeviceContext::PreprocessingGraphicsCommandBufferDraw()
{
  //  画像レイアウトの変更
  {
    auto ChangeImageLayout = [&](ShaderType _Type)->void
    {
      const ShaderResources& ShaderResources = CurrentGraphicsState_.ShaderResources[static_cast<vdl::uint>(_Type)];

      const vdl::uint ShaderResourceNum = static_cast<vdl::uint>(ShaderResources.size());
      for (vdl::uint ShaderResourceCount = 0; ShaderResourceCount < ShaderResourceNum; ++ShaderResourceCount)
      {
        const vdl::ShaderResource& ShaderResource = ShaderResources[ShaderResourceCount];

        //  Texture
        if (std::get_if<vdl::Texture>(&ShaderResource))
        {
          const vdl::Texture& Texture = std::get<vdl::Texture>(ShaderResource);
          if (!Texture.isEmpty())
          {
            constexpr vk::ImageLayout kImageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

            CTexture* pTexture = static_cast<CTexture*>(pTextureManager_->GetTexture(Texture.GetID()));
            if (pTexture->CurrentLayout != kImageLayout)
            {
              BeginGraphicsCommandBuffer();
              if (pTexture->GetType() == TextureType::eDepthStencilTexture)
              {
                pTexture->SetImageLayout(GetCurrentGraphicsCommandBuffer(), kImageLayout, { static_cast<CDepthStencilTexture*>(pTexture)->ImageAspectFlag, 0, 1, 0, 1 });
              }
              else
              {
                pTexture->SetImageLayout(GetCurrentGraphicsCommandBuffer(), kImageLayout, { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 });
              }
            }
          }
        }
      }
    };

    if (!CurrentGraphicsState_.VertexShader.isEmpty())
    {
      ChangeImageLayout(ShaderType::eVertexShader);
    }
    if (!CurrentGraphicsState_.HullShader.isEmpty())
    {
      ChangeImageLayout(ShaderType::eHullShader);
    }
    if (!CurrentGraphicsState_.DomainShader.isEmpty())
    {
      ChangeImageLayout(ShaderType::eDomainShader);
    }
    if (!CurrentGraphicsState_.GeometryShader.isEmpty())
    {
      ChangeImageLayout(ShaderType::eGeometryShader);
    }
    if (!CurrentGraphicsState_.PixelShader.isEmpty())
    {
      ChangeImageLayout(ShaderType::ePixelShader);
    }
  }

  BeginRenderPassGraphicsCommandBuffer();

  const vk::CommandBuffer& CurrentGraphicsCommandBuffer = GetCurrentGraphicsCommandBuffer();

  //  シザーが無効の時に強制的に設定する
  if (!CurrentGraphicsState_.RasterizerState.ScissorEnable)
  {
    GraphicsStateChangeFlags_.Set(GraphicsCommandType::eSetScissor);
  }

  if (GraphicsStateChangeFlags_.isEmpty())
  {
    return;
  }

  GraphicsReserveData& CurrentGraphicsReserveData = GraphicsReserveDatas_[GraphicsCommandBufferIndex_];

  //  SetPipeline
  for (auto& GraphicsPipelineStateCommand : kGraphicsPipelineStateCommands)
  {
    if (GraphicsStateChangeFlags_.Has(GraphicsPipelineStateCommand))
    {
      std::vector<vk::PipelineShaderStageCreateInfo> PipelineShaderStageInfos;
      {
        if (!CurrentGraphicsState_.VertexShader.isEmpty())
        {
          assert(pShaderManager_->GetShader(CurrentGraphicsState_.VertexShader.GetID())->GetType() == ShaderType::eVertexShader);
          const CVertexShader* pVertexShader = static_cast<CVertexShader*>(pShaderManager_->GetShader(CurrentGraphicsState_.VertexShader.GetID()));

          vk::PipelineShaderStageCreateInfo& PipelineShaderStageInfo = PipelineShaderStageInfos.emplace_back();
          {
            PipelineShaderStageInfo.stage = vk::ShaderStageFlagBits::eVertex;
            PipelineShaderStageInfo.module = pVertexShader->ShaderData.Module.get();
            PipelineShaderStageInfo.pName = pVertexShader->ShaderData.EntryPoint;
          }

          CurrentGraphicsReserveData.VertexShaders.push_back(CurrentGraphicsState_.VertexShader);
        }

        if (!CurrentGraphicsState_.HullShader.isEmpty())
        {
          assert(pShaderManager_->GetShader(CurrentGraphicsState_.HullShader.GetID())->GetType() == ShaderType::eHullShader);
          const CHullShader* pHullShader = static_cast<CHullShader*>(pShaderManager_->GetShader(CurrentGraphicsState_.HullShader.GetID()));

          vk::PipelineShaderStageCreateInfo& PipelineShaderStageInfo = PipelineShaderStageInfos.emplace_back();
          {
            PipelineShaderStageInfo.stage = vk::ShaderStageFlagBits::eTessellationControl;
            PipelineShaderStageInfo.module = pHullShader->ShaderData.Module.get();
            PipelineShaderStageInfo.pName = pHullShader->ShaderData.EntryPoint;
          }

          CurrentGraphicsReserveData.HullShaders.push_back(CurrentGraphicsState_.HullShader);
        }

        if (!CurrentGraphicsState_.DomainShader.isEmpty())
        {
          assert(pShaderManager_->GetShader(CurrentGraphicsState_.DomainShader.GetID())->GetType() == ShaderType::eDomainShader);
          const CDomainShader* pDomainShader = static_cast<CDomainShader*>(pShaderManager_->GetShader(CurrentGraphicsState_.DomainShader.GetID()));

          vk::PipelineShaderStageCreateInfo& PipelineShaderStageInfo = PipelineShaderStageInfos.emplace_back();
          {
            PipelineShaderStageInfo.stage = vk::ShaderStageFlagBits::eTessellationEvaluation;
            PipelineShaderStageInfo.module = pDomainShader->ShaderData.Module.get();
            PipelineShaderStageInfo.pName = pDomainShader->ShaderData.EntryPoint;
          }

          CurrentGraphicsReserveData.DomainShaders.push_back(CurrentGraphicsState_.DomainShader);
        }

        if (!CurrentGraphicsState_.GeometryShader.isEmpty())
        {
          assert(pShaderManager_->GetShader(CurrentGraphicsState_.GeometryShader.GetID())->GetType() == ShaderType::eGeometryShader);
          const CGeometryShader* pGeometryShader = static_cast<CGeometryShader*>(pShaderManager_->GetShader(CurrentGraphicsState_.GeometryShader.GetID()));

          vk::PipelineShaderStageCreateInfo& PipelineShaderStageInfo = PipelineShaderStageInfos.emplace_back();
          {
            PipelineShaderStageInfo.stage = vk::ShaderStageFlagBits::eGeometry;
            PipelineShaderStageInfo.module = pGeometryShader->ShaderData.Module.get();
            PipelineShaderStageInfo.pName = pGeometryShader->ShaderData.EntryPoint;
          }

          CurrentGraphicsReserveData.GeometryShaders.push_back(CurrentGraphicsState_.GeometryShader);
        }

        if (!CurrentGraphicsState_.PixelShader.isEmpty())
        {
          assert(pShaderManager_->GetShader(CurrentGraphicsState_.PixelShader.GetID())->GetType() == ShaderType::ePixelShader);
          const CPixelShader* pPixelShader = static_cast<CPixelShader*>(pShaderManager_->GetShader(CurrentGraphicsState_.PixelShader.GetID()));

          vk::PipelineShaderStageCreateInfo& PipelineShaderStageInfo = PipelineShaderStageInfos.emplace_back();
          {
            PipelineShaderStageInfo.stage = vk::ShaderStageFlagBits::eFragment;
            PipelineShaderStageInfo.module = pPixelShader->ShaderData.Module.get();
            PipelineShaderStageInfo.pName = pPixelShader->ShaderData.EntryPoint;
          }

          CurrentGraphicsReserveData.PixelShaders.push_back(CurrentGraphicsState_.PixelShader);
        }
      }

      vk::PipelineVertexInputStateCreateInfo PipelineVertexInputStateInfo;
      {
        const InputLayout& InputLayout = InputLayouts_[CurrentGraphicsState_.InputLayout];

        PipelineVertexInputStateInfo.vertexBindingDescriptionCount = static_cast<vdl::uint>(InputLayout.VertexInputBindingDescriptions.size());
        PipelineVertexInputStateInfo.pVertexBindingDescriptions = InputLayout.VertexInputBindingDescriptions.data();
        PipelineVertexInputStateInfo.vertexAttributeDescriptionCount = static_cast<vdl::uint>(InputLayout.VertexInputAttributeDescriptions.size());
        PipelineVertexInputStateInfo.pVertexAttributeDescriptions = InputLayout.VertexInputAttributeDescriptions.data();
      }

      vk::PipelineInputAssemblyStateCreateInfo PipelineInputAssemblyStateInfo;
      {
        PipelineInputAssemblyStateInfo.topology = Cast(CurrentGraphicsState_.Topology);
        PipelineInputAssemblyStateInfo.primitiveRestartEnable = false;
      }

      vk::PipelineTessellationStateCreateInfo PipelineTessellationStateInfo;
      {
        PipelineTessellationStateInfo.patchControlPoints = GetPatchControlPoints(CurrentGraphicsState_.Topology);
      }

      vk::PipelineViewportStateCreateInfo PipelineViewportStateInfo;
      {
        PipelineViewportStateInfo.viewportCount = 1;
        PipelineViewportStateInfo.pViewports = nullptr;
        PipelineViewportStateInfo.scissorCount = 1;
        PipelineViewportStateInfo.pScissors = nullptr;
      }

      const vk::PipelineRasterizationStateCreateInfo& PipelineRasterizationStateInfo = GetPipelineRasterizationStateInfo(CurrentGraphicsState_.RasterizerState);

      const vk::PipelineDepthStencilStateCreateInfo& PipelineDepthStencilStateInfo = GetPipelineDepthStencilStateInfo(CurrentGraphicsState_.DepthStencilState);

      std::vector<vk::PipelineColorBlendAttachmentState> PipelineColorBlendAttachmentStates(GraphicsColorAttachmentCount_);
      {
        assert(GraphicsColorAttachmentCount_ <= Constants::kMaxRenderTextureNum);

        if (CurrentGraphicsState_.BlendState.IndependentBlendEnable)
        {
          for (vdl::uint i = 0; i < GraphicsColorAttachmentCount_; ++i)
          {
            PipelineColorBlendAttachmentStates[i] = GetPipelineColorBlendAttachmentState(CurrentGraphicsState_.BlendState.RenderTexture[i]);
          }
        }
        else
        {
          PipelineColorBlendAttachmentStates[0] = GetPipelineColorBlendAttachmentState(CurrentGraphicsState_.BlendState.RenderTexture[0]);
          for (vdl::uint i = 1; i < GraphicsColorAttachmentCount_; ++i)
          {
            PipelineColorBlendAttachmentStates[i] = GetPipelineColorBlendAttachmentState(CurrentGraphicsState_.BlendState.RenderTexture[i]);
          }
        }
      }

      const vk::PipelineMultisampleStateCreateInfo& PipelineMultisampleStateInfo = GetMultisampleStateInfo(CurrentGraphicsState_.BlendState.AlphaToCoverageEnable);

      vk::PipelineColorBlendStateCreateInfo PipelineColorBlendStateInfo;
      {
        PipelineColorBlendStateInfo.logicOpEnable = false;
        PipelineColorBlendStateInfo.logicOp = vk::LogicOp::eNoOp;
        PipelineColorBlendStateInfo.attachmentCount = GraphicsColorAttachmentCount_;
        PipelineColorBlendStateInfo.pAttachments = PipelineColorBlendAttachmentStates.data();
        PipelineColorBlendStateInfo.blendConstants[0] = 1.0f;
        PipelineColorBlendStateInfo.blendConstants[1] = 1.0f;
        PipelineColorBlendStateInfo.blendConstants[2] = 1.0f;
        PipelineColorBlendStateInfo.blendConstants[3] = 1.0f;
      }

      std::array<vk::DynamicState, 2> DynamicStates;
      {
        DynamicStates[0] = vk::DynamicState::eViewport;
        DynamicStates[1] = vk::DynamicState::eScissor;
      }

      vk::PipelineDynamicStateCreateInfo PipelineDynamicStateInfo;
      {
        PipelineDynamicStateInfo.dynamicStateCount = static_cast<vdl::uint>(DynamicStates.size());
        PipelineDynamicStateInfo.pDynamicStates = DynamicStates.data();
      }

      //  パイプラインの作成
      {
        std::vector<vk::UniquePipeline>& CurrentGraphicsPipelines = CurrentGraphicsReserveData.Pipelines;
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
          GraphicsPipelineInfo.renderPass = CurrentGraphicsReserveData.RenderPassDatas.back().RenderPass.get();
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
  {
    //  SetShaderResource
    {
      auto BindShaderResources = [&](ShaderType _Type)->void
      {
        const ShaderResources& ShaderResources = CurrentGraphicsState_.ShaderResources[static_cast<vdl::uint>(_Type)];

        std::vector<DescriptorImageData> TextureDatas;
        std::vector<DescriptorBufferData> UnorderedAccessBufferDatas;

        //  データの読み込み
        {
          const vdl::uint ShaderResourceNum = static_cast<vdl::uint>(ShaderResources.size());
          for (vdl::uint ShaderResourceCount = 0; ShaderResourceCount < ShaderResourceNum; ++ShaderResourceCount)
          {
            const vdl::ShaderResource& ShaderResource = ShaderResources[ShaderResourceCount];

            //  Texture
            if (std::get_if<vdl::Texture>(&ShaderResource))
            {
              const vdl::Texture& Texture = std::get<vdl::Texture>(ShaderResource);
              if (!Texture.isEmpty())
              {
                constexpr vk::ImageLayout kImageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

                CTexture* pTexture = static_cast<CTexture*>(pTextureManager_->GetTexture(Texture.GetID()));

                DescriptorImageData& ImageData = TextureDatas.emplace_back();
                {
                  ImageData.Info.imageView = pTexture->View.get();
                  ImageData.Info.imageLayout = kImageLayout;
                  ImageData.Bind = ShaderResourceCount;
                }
              }
            }
            //  UnorderedAccessBuffer
            else if (std::get_if<vdl::Detail::UnorderedAccessBufferData>(&ShaderResource))
            {
              const vdl::Detail::UnorderedAccessBufferData& UnorderedAccessBuffer = std::get<vdl::Detail::UnorderedAccessBufferData>(ShaderResource);

              if (!UnorderedAccessBuffer.isEmpty())
              {
                assert(pBufferManager_->GetBuffer(UnorderedAccessBuffer.GetID())->GetType() == BufferType::eUnorderedAccessBuffer);

                CUnordererdAccessBuffer* pUnorderedAccessBuffer = static_cast<CUnordererdAccessBuffer*>(pBufferManager_->GetBuffer(UnorderedAccessBuffer.GetID()));
                DescriptorBufferData& BufferData = UnorderedAccessBufferDatas.emplace_back();
                {
                  BufferData.Info.buffer = pUnorderedAccessBuffer->BufferData.Buffer.get();
                  BufferData.Info.offset = 0;
                  BufferData.Info.range = pUnorderedAccessBuffer->BufferSize;
                  BufferData.Bind = ShaderResourceCount;
                }
              }
            }
          }
        }

        bool isEmpty = true;

        if (!TextureDatas.empty())
        {
          constexpr DescriptorType kDescriptorType = DescriptorType::eTexture;

          const vdl::uint DescriptorLayoutIndex = GetDescriptorLayoutOffset(_Type, kDescriptorType);

          vk::DescriptorSetAllocateInfo DescriptorSetAllocateInfo;
          {
            DescriptorSetAllocateInfo.descriptorPool = GraphicsDescriptorPool_.get();
            DescriptorSetAllocateInfo.descriptorSetCount = 1;
            DescriptorSetAllocateInfo.pSetLayouts = &GraphicsDescriptorLayouts_[DescriptorLayoutIndex].get();
          }

          vk::UniqueDescriptorSet DescriptorSet = std::move(VkDevice_.allocateDescriptorSetsUnique(DescriptorSetAllocateInfo).front());
          assert(DescriptorSet);

          std::vector<vk::WriteDescriptorSet> WriteDescriptorSets;
          {
            for (auto& TextureData : TextureDatas)
            {
              vk::WriteDescriptorSet& WriteDescriptorSet = WriteDescriptorSets.emplace_back();
              {
                WriteDescriptorSet.dstSet = DescriptorSet.get();
                WriteDescriptorSet.dstBinding = TextureData.Bind;
                WriteDescriptorSet.dstArrayElement = 0;
                WriteDescriptorSet.descriptorCount = 1;
                WriteDescriptorSet.descriptorType = Cast(kDescriptorType);
                WriteDescriptorSet.pImageInfo = &TextureData.Info;
              }
            }
          }

          VkDevice_.updateDescriptorSets(WriteDescriptorSets, nullptr);

          CurrentGraphicsCommandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, PipelineLayout_.get(), DescriptorLayoutIndex, DescriptorSet.get(), nullptr);

          CurrentGraphicsReserveData.DescriptorSets.emplace_back(std::move(DescriptorSet));

          isEmpty = false;
        }

        if (!UnorderedAccessBufferDatas.empty())
        {
          constexpr DescriptorType kDescriptorType = DescriptorType::eBuffer;

          const vdl::uint DescriptorLayoutIndex = GetDescriptorLayoutOffset(_Type, kDescriptorType);

          vk::DescriptorSetAllocateInfo DescriptorSetAllocateInfo;
          {
            DescriptorSetAllocateInfo.descriptorPool = GraphicsDescriptorPool_.get();
            DescriptorSetAllocateInfo.descriptorSetCount = 1;
            DescriptorSetAllocateInfo.pSetLayouts = &GraphicsDescriptorLayouts_[DescriptorLayoutIndex].get();
          }

          vk::UniqueDescriptorSet DescriptorSet = std::move(VkDevice_.allocateDescriptorSetsUnique(DescriptorSetAllocateInfo).front());
          assert(DescriptorSet);

          std::vector<vk::WriteDescriptorSet> WriteDescriptorSets;
          {
            for (auto& UnorderedAccessBufferData : UnorderedAccessBufferDatas)
            {
              vk::WriteDescriptorSet& WriteDescriptorSet = WriteDescriptorSets.emplace_back();
              {
                WriteDescriptorSet.dstSet = DescriptorSet.get();
                WriteDescriptorSet.dstBinding = UnorderedAccessBufferData.Bind;
                WriteDescriptorSet.dstArrayElement = 0;
                WriteDescriptorSet.descriptorCount = 1;
                WriteDescriptorSet.descriptorType = Cast(kDescriptorType);
                WriteDescriptorSet.pBufferInfo = &UnorderedAccessBufferData.Info;
              }
            }
          }

          VkDevice_.updateDescriptorSets(WriteDescriptorSets, nullptr);

          CurrentGraphicsCommandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, PipelineLayout_.get(), DescriptorLayoutIndex, DescriptorSet.get(), nullptr);

          CurrentGraphicsReserveData.DescriptorSets.emplace_back(std::move(DescriptorSet));

          isEmpty = false;
        }

        if (!isEmpty)
        {
          CurrentGraphicsReserveData.ShaderResources.push_back(ShaderResources);
        }
      };

      if (GraphicsStateChangeFlags_.Has(GraphicsCommandType::eSetVertexStageShaderResource))
      {
        BindShaderResources(ShaderType::eVertexShader);
      }

      if (GraphicsStateChangeFlags_.Has(GraphicsCommandType::eSetHullStageShaderResource))
      {
        BindShaderResources(ShaderType::eHullShader);
      }

      if (GraphicsStateChangeFlags_.Has(GraphicsCommandType::eSetDomainStageShaderResource))
      {
        BindShaderResources(ShaderType::eDomainShader);
      }

      if (GraphicsStateChangeFlags_.Has(GraphicsCommandType::eSetGeometryStageShaderResource))
      {
        BindShaderResources(ShaderType::eGeometryShader);
      }

      if (GraphicsStateChangeFlags_.Has(GraphicsCommandType::eSetPixelStageShaderResource))
      {
        BindShaderResources(ShaderType::ePixelShader);
      }
    }

    //  SetSampler
    {
      auto BindSamplers = [&](ShaderType _Type)->void
      {
        const Samplers& Samplers = CurrentGraphicsState_.Samplers[static_cast<vdl::uint>(_Type)];

        std::vector<DescriptorImageData> SamplerDatas;

        //  データの読み込み
        {
          const vdl::uint SamplerNum = static_cast<vdl::uint>(Samplers.size());
          for (vdl::uint SamplerCount = 0; SamplerCount < SamplerNum; ++SamplerCount)
          {
            DescriptorImageData& ImageData = SamplerDatas.emplace_back();
            {
              ImageData.Info.sampler = GetSampler(Samplers[SamplerCount]);
              ImageData.Info.imageLayout = vk::ImageLayout::eUndefined;
              ImageData.Bind = SamplerCount;
            }
          }
        }

        if (SamplerDatas.empty())
        {
          return;
        }

        constexpr DescriptorType kDescriptorType = DescriptorType::eSampler;

        const vdl::uint DescriptorLayoutIndex = GetDescriptorLayoutOffset(_Type, kDescriptorType);

        vk::DescriptorSetAllocateInfo DescriptorSetAllocateInfo;
        {
          DescriptorSetAllocateInfo.descriptorPool = GraphicsDescriptorPool_.get();
          DescriptorSetAllocateInfo.descriptorSetCount = 1;
          DescriptorSetAllocateInfo.pSetLayouts = &GraphicsDescriptorLayouts_[DescriptorLayoutIndex].get();
        }

        vk::UniqueDescriptorSet DescriptorSet = std::move(VkDevice_.allocateDescriptorSetsUnique(DescriptorSetAllocateInfo).front());

        std::vector<vk::WriteDescriptorSet> WriteDescriptorSets;
        {
          for (auto& SamplerData : SamplerDatas)
          {
            vk::WriteDescriptorSet& WriteDescriptorSet = WriteDescriptorSets.emplace_back();
            {
              WriteDescriptorSet.dstSet = DescriptorSet.get();
              WriteDescriptorSet.dstBinding = SamplerData.Bind;
              WriteDescriptorSet.dstArrayElement = 0;
              WriteDescriptorSet.descriptorCount = 1;
              WriteDescriptorSet.descriptorType = Cast(kDescriptorType);
              WriteDescriptorSet.pImageInfo = &SamplerData.Info;
            }
          }
        }

        VkDevice_.updateDescriptorSets(WriteDescriptorSets, nullptr);

        CurrentGraphicsCommandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, PipelineLayout_.get(), DescriptorLayoutIndex, DescriptorSet.get(), nullptr);

        CurrentGraphicsReserveData.DescriptorSets.emplace_back(std::move(DescriptorSet));
      };

      if (GraphicsStateChangeFlags_.Has(GraphicsCommandType::eSetVertexStageSampler))
      {
        BindSamplers(ShaderType::eVertexShader);
      }

      if (GraphicsStateChangeFlags_.Has(GraphicsCommandType::eSetHullStageSampler))
      {
        BindSamplers(ShaderType::eHullShader);
      }

      if (GraphicsStateChangeFlags_.Has(GraphicsCommandType::eSetDomainStageSampler))
      {
        BindSamplers(ShaderType::eDomainShader);
      }

      if (GraphicsStateChangeFlags_.Has(GraphicsCommandType::eSetGeometryStageSampler))
      {
        BindSamplers(ShaderType::eGeometryShader);
      }

      if (GraphicsStateChangeFlags_.Has(GraphicsCommandType::eSetPixelStageSampler))
      {
        BindSamplers(ShaderType::ePixelShader);
      }
    }

    //  SetConstantBuffer
    {
      auto BindConstantBuffers = [&](ShaderType _Type)
      {
        const ConstantBuffers& ConstantBuffers = CurrentGraphicsState_.ConstantBuffers[static_cast<vdl::uint>(_Type)];

        std::vector<DescriptorBufferData> ConstantBufferDatas;

        //  データの読み込み
        {
          const vdl::uint ConstantBufferNum = static_cast<vdl::uint>(ConstantBuffers.size());
          for (vdl::uint ConstantBufferCount = 0; ConstantBufferCount < ConstantBufferNum; ++ConstantBufferCount)
          {
            assert(pBufferManager_->GetBuffer(ConstantBuffers[ConstantBufferCount].GetID())->GetType() == BufferType::eCopyConstantBuffer);
            CCopyConstantBuffer* pConstantBuffer = static_cast<CCopyConstantBuffer*>(pBufferManager_->GetBuffer(ConstantBuffers[ConstantBufferCount].GetID()));

            DescriptorBufferData& BufferData = ConstantBufferDatas.emplace_back();
            {
              BufferData.Info.buffer = pConstantBuffer->ParentBuffer;
              BufferData.Info.offset = pConstantBuffer->Offset;
              BufferData.Info.range = pConstantBuffer->BufferSize;
              BufferData.Bind = ConstantBufferCount;
            }
          }
        }

        if (ConstantBufferDatas.empty())
        {
          return;
        }

        constexpr DescriptorType kDescriptorType = DescriptorType::eConstantBuffer;

        const vdl::uint DescriptorLayoutIndex = GetDescriptorLayoutOffset(_Type, kDescriptorType);

        vk::DescriptorSetAllocateInfo DescriptorSetAllocateInfo;
        {
          DescriptorSetAllocateInfo.descriptorPool = GraphicsDescriptorPool_.get();
          DescriptorSetAllocateInfo.descriptorSetCount = 1;
          DescriptorSetAllocateInfo.pSetLayouts = &GraphicsDescriptorLayouts_[DescriptorLayoutIndex].get();
        }

        vk::UniqueDescriptorSet DescriptorSet = std::move(VkDevice_.allocateDescriptorSetsUnique(DescriptorSetAllocateInfo).front());

        std::vector<vk::WriteDescriptorSet> WriteDescriptorSets;
        {
          for (auto& ConstantBufferData : ConstantBufferDatas)
          {
            vk::WriteDescriptorSet& WriteDescriptorSet = WriteDescriptorSets.emplace_back();
            {
              WriteDescriptorSet.dstSet = DescriptorSet.get();
              WriteDescriptorSet.dstBinding = ConstantBufferData.Bind;
              WriteDescriptorSet.dstArrayElement = 0;
              WriteDescriptorSet.descriptorCount = 1;
              WriteDescriptorSet.descriptorType = Cast(kDescriptorType);
              WriteDescriptorSet.pBufferInfo = &ConstantBufferData.Info;
            }
          }
        }

        VkDevice_.updateDescriptorSets(WriteDescriptorSets, nullptr);

        CurrentGraphicsCommandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, PipelineLayout_.get(), DescriptorLayoutIndex, DescriptorSet.get(), nullptr);

        CurrentGraphicsReserveData.DescriptorSets.emplace_back(std::move(DescriptorSet));

        CurrentGraphicsReserveData.ConstantBuffers.push_back(ConstantBuffers);
      };

      if (GraphicsStateChangeFlags_.Has(GraphicsCommandType::eSetVertexStageConstantBuffer))
      {
        BindConstantBuffers(ShaderType::eVertexShader);
      }

      if (GraphicsStateChangeFlags_.Has(GraphicsCommandType::eSetHullStageConstantBuffer))
      {
        BindConstantBuffers(ShaderType::eHullShader);
      }

      if (GraphicsStateChangeFlags_.Has(GraphicsCommandType::eSetDomainStageConstantBuffer))
      {
        BindConstantBuffers(ShaderType::eDomainShader);
      }

      if (GraphicsStateChangeFlags_.Has(GraphicsCommandType::eSetGeometryStageConstantBuffer))
      {
        BindConstantBuffers(ShaderType::eGeometryShader);
      }

      if (GraphicsStateChangeFlags_.Has(GraphicsCommandType::eSetPixelStageConstantBuffer))
      {
        BindConstantBuffers(ShaderType::ePixelShader);
      }
    }
  }

  if (GraphicsStateChangeFlags_.Has(GraphicsCommandType::eSetVertexBuffer) && !CurrentGraphicsState_.VertexBuffer.isEmpty())
  {
    assert(pBufferManager_->GetBuffer(CurrentGraphicsState_.VertexBuffer.GetID())->GetType() == BufferType::eVertexBuffer);

    const CVertexBuffer* pVertexBuffer = static_cast<const CVertexBuffer*>(pBufferManager_->GetBuffer(CurrentGraphicsState_.VertexBuffer.GetID()));

    constexpr vk::DeviceSize kOffset = 0;
    CurrentGraphicsCommandBuffer.bindVertexBuffers(0, pVertexBuffer->BufferData.Buffer.get(), kOffset);

    CurrentGraphicsReserveData.VertexBuffers.push_back(CurrentGraphicsState_.VertexBuffer);
  }

  if (GraphicsStateChangeFlags_.Has(GraphicsCommandType::eSetInstanceBuffer) && !CurrentGraphicsState_.InstanceBuffer.isEmpty())
  {
    assert(pBufferManager_->GetBuffer(CurrentGraphicsState_.InstanceBuffer.GetID())->GetType() == BufferType::eInstanceBuffer);

    const CInstanceBuffer* pInstanceBuffer = static_cast<const CInstanceBuffer*>(pBufferManager_->GetBuffer(CurrentGraphicsState_.InstanceBuffer.GetID()));

    CurrentGraphicsCommandBuffer.bindVertexBuffers(1, pInstanceBuffer->BufferData.Buffer.get(), pInstanceBuffer->PreviousOffset);

    CurrentGraphicsReserveData.InstanceBuffers.push_back(CurrentGraphicsState_.InstanceBuffer);
  }

  if (GraphicsStateChangeFlags_.Has(GraphicsCommandType::eSetIndexBuffer) && !CurrentGraphicsState_.IndexBuffer.isEmpty())
  {
    assert(pBufferManager_->GetBuffer(CurrentGraphicsState_.IndexBuffer.GetID())->GetType() == BufferType::eIndexBuffer);

    const CIndexBuffer* pIndexBuffer = static_cast<const CIndexBuffer*>(pBufferManager_->GetBuffer(CurrentGraphicsState_.IndexBuffer.GetID()));

    constexpr vk::DeviceSize kOffset = 0;
    CurrentGraphicsCommandBuffer.bindIndexBuffer(pIndexBuffer->BufferData.Buffer.get(), kOffset, pIndexBuffer->IndexType);

    CurrentGraphicsReserveData.IndexBuffers.push_back(CurrentGraphicsState_.IndexBuffer);
  }

  if (GraphicsStateChangeFlags_.Has(GraphicsCommandType::eSetScissor))
  {
    CurrentGraphicsCommandBuffer.setScissor(0,
      Cast(CurrentGraphicsState_.RasterizerState.ScissorEnable ? CurrentGraphicsState_.Scissor
        : vdl::Scissor(CurrentGraphicsState_.Viewport.LeftTop, CurrentGraphicsState_.Viewport.Size)));
  }

  if (GraphicsStateChangeFlags_.Has(GraphicsCommandType::eSetViewport))
  {
    CurrentGraphicsCommandBuffer.setViewport(0, Cast(CurrentGraphicsState_.Viewport));
  }

  GraphicsStateChangeFlags_.Clear();
}

void CDeviceContext::WaitFence(const vk::Fence& _Fence)
{
  //  Fenceが有効な間は完了まで待つ
  while (_Fence)
  {
    if (VkDevice_.waitForFences(_Fence, true, UINT64_MAX) == vk::Result::eSuccess)
    {
      VkDevice_.resetFences(1, &_Fence);
      break;
    }
  }
}

//--------------------------------------------------

const vk::PipelineRasterizationStateCreateInfo& CDeviceContext::GetPipelineRasterizationStateInfo(const vdl::RasterizerState & _RasterizerState)
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
      PipelineRasterizationStateInfo.depthBiasConstantFactor = static_cast<float>(_RasterizerState.DepthBias);
      PipelineRasterizationStateInfo.depthBiasClamp = 0.0f;
      PipelineRasterizationStateInfo.depthBiasSlopeFactor = 0.0f;
      PipelineRasterizationStateInfo.lineWidth = 1.0f;
    }

    RasterizerStates_.insert(std::make_pair(_RasterizerState, std::move(PipelineRasterizationStateInfo)));
  }

  return RasterizerStates_.at(_RasterizerState);
}

const vk::PipelineDepthStencilStateCreateInfo& CDeviceContext::GetPipelineDepthStencilStateInfo(const vdl::DepthStencilState & _DepthStencilState)
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

const vk::PipelineColorBlendAttachmentState& CDeviceContext::GetPipelineColorBlendAttachmentState(const vdl::RenderTextureBlendState & _RenderTextureBlendState)
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

const vk::Sampler& CDeviceContext::GetSampler(const vdl::Sampler& _Sampler)
{
  if (Samplers_.find(_Sampler) == Samplers_.end())
  {
    vk::SamplerCreateInfo SamplerInfo;
    {
      SamplerInfo.magFilter = GetMag(_Sampler.Filter);
      SamplerInfo.minFilter = GetMin(_Sampler.Filter);
      SamplerInfo.mipmapMode = GetMimap(_Sampler.Filter);
      SamplerInfo.addressModeU = Cast(_Sampler.AddressModeU);
      SamplerInfo.addressModeV = Cast(_Sampler.AddressModeV);;
      SamplerInfo.addressModeW = Cast(_Sampler.AddressModeW);
      SamplerInfo.mipLodBias = 0.0f;
      SamplerInfo.anisotropyEnable = isAnisotropic(_Sampler.Filter);
      SamplerInfo.maxAnisotropy = _Sampler.MaxAnisotropy;
      SamplerInfo.compareEnable = enableComparison(_Sampler.Filter);
      SamplerInfo.compareOp = vk::CompareOp::eAlways;
      SamplerInfo.minLod = 0;
      SamplerInfo.maxLod = FLT_MAX;
      SamplerInfo.borderColor = Cast(_Sampler.BorderColor);
      SamplerInfo.unnormalizedCoordinates = false;
    }

    vk::UniqueSampler Sampler = VkDevice_.createSamplerUnique(SamplerInfo);
    assert(Sampler);

    Samplers_.insert(std::make_pair(_Sampler, std::move(Sampler)));
  }

  return Samplers_.at(_Sampler).get();
}
