#include "CDeviceContext.hpp"

#include <vdl/Engine.hpp>
#include <vdl/Device/Vulkan/CDevice.hpp>
#include <vdl/Window/IWindow.hpp>
#include <vdl/TextureManager/ITextureManager.hpp>
#include <vdl/BufferManager/IBufferManager.hpp>
#include <vdl/ShaderManager/IShaderManager.hpp>

#include <vdl/Topology/Vulkan/Topology.hpp>
#include <vdl/Scissor/Vulkan/Scissor.hpp>
#include <vdl/Viewport/Vulkan/Viewport.hpp>
#include <vdl/Buffer/Vulkan/CBuffer.hpp>

#include <vdl/Constants/Constants.hpp>

#include <ThirdParty/ImGui/imgui.h>

#include <vdl/Vertex.hpp>
#include <vdl/DetectMemoryLeak.hpp>

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

  inline vk::ClearColorValue Cast(const vdl::Color4F& _Color)
  {
    return std::array<float, 4>({ _Color.Red, _Color.Green, _Color.Blue, _Color.Alpha });
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
      return vk::Filter::eLinear;
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
      return vk::Filter::eLinear;
    default: assert(false);
    }

    return vk::Filter::eNearest;
  }

  inline vk::SamplerMipmapMode GetMipmap(vdl::FilterType _Type)
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
      return vk::SamplerMipmapMode::eLinear;
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
    vk::DescriptorImageInfo Descriptor;
    vdl::uint Bind;
  };

  struct DescriptorBufferData
  {
    vk::DescriptorBufferInfo Descriptor;
    vdl::uint Bind;
  };
}

void CDeviceContext::Initialize()
{
  pDevice_ = Engine::Get<IDevice>();
  pWindow_ = Engine::Get<IWindow>();
  pTextureManager_ = Engine::Get<ITextureManager>();
  pBufferManager_ = Engine::Get<IBufferManager>();
  pShaderManager_ = Engine::Get<IShaderManager>();

  CDevice* pDevice = Cast<CDevice>(pDevice_);
  VkDevice_ = pDevice->GetDevice();
  GraphicsQueue_ = pDevice->GetGraphicsQueue();
  ComputeQueue_ = pDevice->GetComputeQueue();
  SwapChain_.Initialize(pDevice);

  //  ���̓��C�A�E�g�̍쐬
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

        //  ���_�f�[�^
        {
          InputLayout.VertexInputBindingDescriptions[0].binding = 0;
          InputLayout.VertexInputBindingDescriptions[0].stride = sizeof(vdl::Vertex2D);
          InputLayout.VertexInputBindingDescriptions[0].inputRate = vk::VertexInputRate::eVertex;

          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32Sfloat, sizeof(vdl::Vertex2D::Position));
          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32Sfloat, sizeof(vdl::Vertex2D::Texcoord));
        }

        ++BindDescriptionCount;
        Offset = 0;

        //  �C���X�^���X�f�[�^
        {
          InputLayout.VertexInputBindingDescriptions[1].binding = 1;
          InputLayout.VertexInputBindingDescriptions[1].stride = sizeof(vdl::Instance2D);
          InputLayout.VertexInputBindingDescriptions[1].inputRate = vk::VertexInputRate::eInstance;

          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32B32A32Sfloat, sizeof(vdl::Instance2D::NDCTransform.r[0]));
          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32B32A32Sfloat, sizeof(vdl::Instance2D::NDCTransform.r[1]));
          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32B32A32Sfloat, sizeof(vdl::Instance2D::NDCTransform.r[2]));
          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32B32A32Sfloat, sizeof(vdl::Instance2D::NDCTransform.r[3]));
          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32Sfloat, sizeof(vdl::Instance2D::TexcoordScale));
          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32Sfloat, sizeof(vdl::Instance2D::TexcoordTranslate));
          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32B32A32Sfloat, sizeof(vdl::Instance2D::Color));
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

        //  ���_�f�[�^
        {
          InputLayout.VertexInputBindingDescriptions[0].binding = 0;
          InputLayout.VertexInputBindingDescriptions[0].stride = sizeof(vdl::VertexSkinnedMesh);
          InputLayout.VertexInputBindingDescriptions[0].inputRate = vk::VertexInputRate::eVertex;

          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32B32Sfloat, sizeof(vdl::VertexStaticMesh::Position));
          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32B32Sfloat, sizeof(vdl::VertexStaticMesh::Normal));
          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32B32Sfloat, sizeof(vdl::VertexStaticMesh::Tangent));
          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32Sfloat, sizeof(vdl::VertexStaticMesh::Texcoord));
        }

        ++BindDescriptionCount;
        Offset = 0;

        //  �C���X�^���X�f�[�^
        {
          InputLayout.VertexInputBindingDescriptions[1].binding = 1;
          InputLayout.VertexInputBindingDescriptions[1].stride = sizeof(vdl::InstanceStaticMesh);
          InputLayout.VertexInputBindingDescriptions[1].inputRate = vk::VertexInputRate::eInstance;

          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32B32A32Sfloat, sizeof(vdl::InstanceStaticMesh::World.r[0]));
          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32B32A32Sfloat, sizeof(vdl::InstanceStaticMesh::World.r[1]));
          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32B32A32Sfloat, sizeof(vdl::InstanceStaticMesh::World.r[2]));
          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32B32A32Sfloat, sizeof(vdl::InstanceStaticMesh::World.r[3]));
          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32B32A32Sfloat, sizeof(vdl::InstanceStaticMesh::Color));        }
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

        //  ���_�f�[�^
        {
          InputLayout.VertexInputBindingDescriptions[0].binding = 0;
          InputLayout.VertexInputBindingDescriptions[0].stride = sizeof(vdl::VertexSkinnedMesh);
          InputLayout.VertexInputBindingDescriptions[0].inputRate = vk::VertexInputRate::eVertex;

          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32B32Sfloat, sizeof(vdl::VertexSkinnedMesh::Position));
          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32B32Sfloat, sizeof(vdl::VertexSkinnedMesh::Normal));
          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32B32Sfloat, sizeof(vdl::VertexSkinnedMesh::Tangent));
          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32Sfloat, sizeof(vdl::VertexSkinnedMesh::Texcoord));
          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32B32A32Sfloat, sizeof(vdl::VertexSkinnedMesh::BoneWeights));
          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32B32A32Uint, sizeof(vdl::VertexSkinnedMesh::BoneIndices));
        }

        ++BindDescriptionCount;
        Offset = 0;

        //  �C���X�^���X�f�[�^
        {
          InputLayout.VertexInputBindingDescriptions[1].binding = 1;
          InputLayout.VertexInputBindingDescriptions[1].stride = sizeof(vdl::InstanceSkinnedMesh);
          InputLayout.VertexInputBindingDescriptions[1].inputRate = vk::VertexInputRate::eInstance;

          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32B32A32Sfloat, sizeof(vdl::InstanceSkinnedMesh::World.r[0]));
          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32B32A32Sfloat, sizeof(vdl::InstanceSkinnedMesh::World.r[1]));
          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32B32A32Sfloat, sizeof(vdl::InstanceSkinnedMesh::World.r[2]));
          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32B32A32Sfloat, sizeof(vdl::InstanceSkinnedMesh::World.r[3]));
          SetVertexInputAttibuteDescription(&InputLayout, vk::Format::eR32G32B32A32Sfloat, sizeof(vdl::InstanceSkinnedMesh::Color));
        }
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

        //  ���_�f�[�^
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

  //  �O���t�B�b�N�X�p�I�u�W�F�N�g�̍쐬
  {
    //  �f�X�N���v�^�Z�b�g���C�A�E�g�̍쐬
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

    //  �f�X�N���v�^�v�[���̍쐬
    {
      std::array<vk::DescriptorPoolSize, kGraphicsDescriptorTypeNum> TypeCounts;
      {
        auto SetTypeCount = [&TypeCounts, this](DescriptorType _Type)->void
        {
          TypeCounts[static_cast<vdl::uint>(_Type)].type = Cast(_Type);
          TypeCounts[static_cast<vdl::uint>(_Type)].descriptorCount = GetPerCount(_Type) * kDescriptorMultipleNum;
        };

        //  �e�N�X�`��
        SetTypeCount(DescriptorType::eTexture);
        //  �o�b�t�@
        SetTypeCount(DescriptorType::eBuffer);
        //  �T���v���[
        SetTypeCount(DescriptorType::eSampler);
        //  �萔�o�b�t�@
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

    //  �p�C�v���C���L���b�V���̍쐬
    {
      GraphicsPipelineCache_ = VkDevice_.createPipelineCacheUnique(vk::PipelineCacheCreateInfo());
      assert(GraphicsPipelineCache_);
    }

    //  �R�}���h�o�b�t�@�̍쐬
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
        CommandBufferInfo.commandBufferCount = Constants::kGraphicsCommandBufferNum;
      }

      std::vector<vk::UniqueCommandBuffer> GraphicsCommandBuffers = VkDevice_.allocateCommandBuffersUnique(CommandBufferInfo);
      assert(!GraphicsCommandBuffers.empty());

      for (vdl::uint i = 0; i < Constants::kGraphicsCommandBufferNum; ++i)
      {
        GraphicsCommandBuffers_[i] = std::move(GraphicsCommandBuffers[i]);
      }
    }

    //  �����I�u�W�F�N�g�̍쐬
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

  //  �R���s���[�g�p�I�u�W�F�N�g�̍쐬
  {
    //  �f�X�N���v�^�Z�b�g���C�A�E�g�̍쐬
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

    //  �f�X�N���v�^�v�[���̍쐬
    {
      std::array<vk::DescriptorPoolSize, kComputeDescriptorTypeNum> TypeCounts;
      {
        auto SetTypeCount = [&TypeCounts, this](DescriptorType _Type)->void
        {
          TypeCounts[static_cast<vdl::uint>(_Type)].type = Cast(_Type);
          TypeCounts[static_cast<vdl::uint>(_Type)].descriptorCount = GetPerCount(_Type) * kDescriptorMultipleNum;
        };

        //  �e�N�X�`��
        SetTypeCount(DescriptorType::eTexture);
        //  �o�b�t�@
        SetTypeCount(DescriptorType::eBuffer);
        //  �T���v���[
        SetTypeCount(DescriptorType::eSampler);
        //  �萔�o�b�t�@
        SetTypeCount(DescriptorType::eConstantBuffer);
        //  ���s���A�N�Z�X�o�b�t�@
        SetTypeCount(DescriptorType::eUnorderedAccessBuffer);
        //  ���s���A�N�Z�X�o�b�t�@
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

    //  �p�C�v���C���L���b�V���̍쐬
    {
      ComputePipelineCache_ = VkDevice_.createPipelineCacheUnique(vk::PipelineCacheCreateInfo());
      assert(ComputePipelineCache_);
    }

    //  �R�}���h�o�b�t�@�̍쐬
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
        CommandBufferInfo.commandBufferCount = Constants::kComputeCommandBufferNum;
      }

      std::vector<vk::UniqueCommandBuffer> ComputeCommandBuffers = VkDevice_.allocateCommandBuffersUnique(CommandBufferInfo);
      assert(!ComputeCommandBuffers.empty());

      for (vdl::uint i = 0; i < Constants::kComputeCommandBufferNum; ++i)
      {
        ComputeCommandBuffers_[i] = std::move(ComputeCommandBuffers[i]);
      }
    }

    //  �����I�u�W�F�N�g�̍쐬
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

    //  �ۑ��f�[�^�̏�����
    {
      ComputeReserveData& ComputeReserveData = GetCurrentComputeReserveData();
      ComputeReserveData.Pipelines.resize(1);
      ComputeReserveData.TextureDescriptorSets.resize(1);
      ComputeReserveData.BufferDescriptorSets.resize(1);
      ComputeReserveData.SamplerDescriptorSets.resize(1);
      ComputeReserveData.ConstantBufferDescriptorSets.resize(1);
      ComputeReserveData.UnorderedAccessTextureDescriptorSets.resize(1);
      ComputeReserveData.UnorderedAccessBufferDescriptorSets.resize(1);
    }
  }

  //  �p�C�v���C�����C�A�E�g�̍쐬
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

  //  �O���t�B�b�N�X�p�p�C�v���C���X�e�[�g�̐ݒ�
  {
    DynamicStates_[0] = vk::DynamicState::eViewport;
    DynamicStates_[1] = vk::DynamicState::eScissor;
    PipelineDynamicStateInfo_.dynamicStateCount = static_cast<vdl::uint>(DynamicStates_.size());
    PipelineDynamicStateInfo_.pDynamicStates = DynamicStates_.data();
    PipelineColorBlendStateInfo_.logicOpEnable = false;
    PipelineColorBlendStateInfo_.logicOp = vk::LogicOp::eNoOp;
    PipelineColorBlendStateInfo_.pAttachments = PipelineColorBlendAttachmentStates_.data();
    PipelineColorBlendStateInfo_.blendConstants[0] = 1.0f;
    PipelineColorBlendStateInfo_.blendConstants[1] = 1.0f;
    PipelineColorBlendStateInfo_.blendConstants[2] = 1.0f;
    PipelineColorBlendStateInfo_.blendConstants[3] = 1.0f;
    PipelineViewportStateInfo_.viewportCount = 1;
    PipelineViewportStateInfo_.pViewports = nullptr;
    PipelineViewportStateInfo_.scissorCount = 1;
    PipelineViewportStateInfo_.pScissors = nullptr;

    GraphicsPipelineInfo_.pViewportState = &PipelineViewportStateInfo_;
    GraphicsPipelineInfo_.pColorBlendState = &PipelineColorBlendStateInfo_;
    GraphicsPipelineInfo_.pDynamicState = &PipelineDynamicStateInfo_;
    GraphicsPipelineInfo_.layout = PipelineLayout_.get();
    GraphicsPipelineInfo_.subpass = 0;
  }
}

void CDeviceContext::SetRenderTextures(const vdl::RenderTextures& _RenderTextures, const vdl::DepthStencilTexture& _DepthStenilTexture)
{
  vdl::OutputManager OutputManager = { _RenderTextures, _DepthStenilTexture };

  GetCurrentGraphicsReserveData().OutputManagers.emplace_back(std::move(OutputManager));
  isChangeGraphicsPipelineState_ = true;
}

void CDeviceContext::ClearRenderTexture(const vdl::RenderTexture& _RenderTexture, const vdl::Color4F& _ClearColor)
{
  assert(!_RenderTexture.isEmpty());

  constexpr vk::ImageLayout kImageLayout = vk::ImageLayout::eTransferDstOptimal;

  BeginGraphicsCommandBuffer();

  const vk::CommandBuffer& CurrentGraphicsCommandBuffer = GetCurrentGraphicsCommandBuffer();

  const vdl::ID TextureID = _RenderTexture.GetID();
  CRenderTexture* pRenderTexture = GetVkRenderTexture(_RenderTexture);

  const vk::ImageSubresourceRange SubresourceRange = { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 };
  if (pRenderTexture->TextureData.CurrentLayout != kImageLayout)
  {
    pRenderTexture->TextureData.SetImageLayout(CurrentGraphicsCommandBuffer, kImageLayout, SubresourceRange);
  }
  CurrentGraphicsCommandBuffer.clearColorImage(pRenderTexture->TextureData.Image.get(), vk::ImageLayout::eTransferDstOptimal, Cast(_ClearColor), SubresourceRange);

  //  Clear����܂Ő�����ۏ�
  {
    std::unordered_map<vdl::ID, Texture>& ClearTextures = GraphicsReserveDatas_[GraphicsCommandBufferIndex_].ClearTextures;

    if (ClearTextures.find(TextureID) == ClearTextures.end())
    {
      ClearTextures.insert(std::make_pair(TextureID, _RenderTexture));
    }
  }
}

void CDeviceContext::ClearDepthStencilTexture(const vdl::DepthStencilTexture& _DepthStencilTexture, float _ClearDepth, vdl::uint _ClearStencil)
{
  assert(!_DepthStencilTexture.isEmpty());

  constexpr vk::ImageLayout kImageLayout = vk::ImageLayout::eTransferDstOptimal;

  BeginGraphicsCommandBuffer();

  const vk::CommandBuffer& CurrentGraphicsCommandBuffer = GetCurrentGraphicsCommandBuffer();

  const vdl::ID TextureID = _DepthStencilTexture.GetID();
  CDepthStencilTexture* pDepthStencilTexture = Cast<CDepthStencilTexture>(pTextureManager_->GetTexture(TextureID));

  const vk::ImageSubresourceRange SubresourceRange = { pDepthStencilTexture->ImageAspectFlag, 0, 1, 0, 1 };
  if (pDepthStencilTexture->TextureData.CurrentLayout != kImageLayout)
  {
    pDepthStencilTexture->TextureData.SetImageLayout(CurrentGraphicsCommandBuffer, kImageLayout, SubresourceRange);
  }
  CurrentGraphicsCommandBuffer.clearDepthStencilImage(pDepthStencilTexture->TextureData.Image.get(), vk::ImageLayout::eTransferDstOptimal, { _ClearDepth, _ClearStencil }, SubresourceRange);

  //  Clear����܂Ő�����ۏ�
  {
    std::unordered_map<vdl::ID, Texture>& ClearTextures = GraphicsReserveDatas_[GraphicsCommandBufferIndex_].ClearTextures;

    if (ClearTextures.find(TextureID) == ClearTextures.end())
    {
      ClearTextures.insert(std::make_pair(TextureID, _DepthStencilTexture));
    }
  }
}

void CDeviceContext::ClearUnorderedAccessTexture(const vdl::UnorderedAccessTexture& _UnorderedAccessTexture, const vdl::Color4F& _ClearColor)
{
  assert(!_UnorderedAccessTexture.isEmpty());

  constexpr vk::ImageLayout kImageLayout = vk::ImageLayout::eTransferDstOptimal;

  BeginGraphicsCommandBuffer();

  const vk::CommandBuffer& CurrentGraphicsCommandBuffer = GetCurrentGraphicsCommandBuffer();

  const vdl::ID TextureID = _UnorderedAccessTexture.GetID();
  CUnorderedAccessTexture* pUnorderedAccessTexture = Cast<CUnorderedAccessTexture>(pTextureManager_->GetTexture(TextureID));

  const vk::ImageSubresourceRange SubresourceRange = { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 };
  if (pUnorderedAccessTexture->TextureData.CurrentLayout != kImageLayout)
  {
    pUnorderedAccessTexture->TextureData.SetImageLayout(CurrentGraphicsCommandBuffer, kImageLayout, SubresourceRange);
  }
  CurrentGraphicsCommandBuffer.clearColorImage(pUnorderedAccessTexture->TextureData.Image.get(), kImageLayout, Cast(_ClearColor), SubresourceRange);

  //  Clear����܂Ő�����ۏ�
  {
    std::unordered_map<vdl::ID, Texture>& ClearTextures = GraphicsReserveDatas_[GraphicsCommandBufferIndex_].ClearTextures;

    if (ClearTextures.find(TextureID) == ClearTextures.end())
    {
      ClearTextures.insert(std::make_pair(TextureID, _UnorderedAccessTexture));
    }
  }
}

void CDeviceContext::Execute(const BaseGraphicsCommandList& _GraphicsCommandList)
{
  //  �f�[�^��ۗL���鎖�Ń��t�@�����X�J�E���g�̈ێ�
  GraphicsCommandListDatas_[GraphicsCommandBufferIndex_] = _GraphicsCommandList;

  BeginGraphicsCommandBuffer();

  const vk::CommandBuffer& CurrentGraphicsCommandBuffer = GetCurrentGraphicsCommandBuffer();
  GraphicsReserveData& CurrentGraphicsReserveData = GetCurrentGraphicsReserveData();

  //  TODO:C++20�ŏ�������
  auto BindShaderResources = [&](const ShaderResources& _ShaderResources, ShaderType _Type)->void
  {
    const vdl::uint ShaderResourceNum = static_cast<vdl::uint>(_ShaderResources.size());
    if (ShaderResourceNum == 0)
    {
      return;
    }

    std::vector<DescriptorImageData> TextureDatas;
    std::vector<DescriptorBufferData> UnorderedAccessBufferDatas;

    //  �f�[�^�̓ǂݍ���
    {
      for (vdl::uint ShaderResourceCount = 0; ShaderResourceCount < ShaderResourceNum; ++ShaderResourceCount)
      {
        const vdl::ShaderResource& ShaderResource = _ShaderResources[ShaderResourceCount];

        ITexture* pTexture = nullptr;
        {
          if (const vdl::Texture* pShaderResource = ShaderResource.GetIf<vdl::Texture>())
          {
            if (!pShaderResource->isEmpty())
            {
              pTexture = pTextureManager_->GetTexture(pShaderResource->GetID());
            }
          }
          else if (const vdl::CubeTexture* pShaderResource = ShaderResource.GetIf<vdl::CubeTexture>())
          {
            if (!pShaderResource->isEmpty())
            {
              pTexture = pTextureManager_->GetTexture(pShaderResource->GetID());
            }
          }
        }

        //  Texture
        if (pTexture)
        {
          DescriptorImageData& ImageData = TextureDatas.emplace_back();
          {
            ImageData.Descriptor = GetTextureDescriptor(pTexture, CurrentGraphicsCommandBuffer);
            ImageData.Bind = ShaderResourceCount;
          }
        }
        //  UnorderedAccessBuffer
        else if (const vdl::Detail::UnorderedAccessBufferData* pShaderResource = ShaderResource.GetIf<vdl::Detail::UnorderedAccessBufferData>())
        {
          if (pShaderResource->isEmpty())
          {
            DescriptorBufferData& BufferData = UnorderedAccessBufferDatas.emplace_back();
            {
              BufferData.Descriptor = GetUnorderedAccessBufferDescriptor(*pShaderResource);
              BufferData.Bind = ShaderResourceCount;
            }
          }
        }
      }
    }

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
            WriteDescriptorSet.pImageInfo = &TextureData.Descriptor;
          }
        }
      }

      VkDevice_.updateDescriptorSets(WriteDescriptorSets, nullptr);

      CurrentGraphicsCommandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, PipelineLayout_.get(), DescriptorLayoutIndex, DescriptorSet.get(), nullptr);

      CurrentGraphicsReserveData.DescriptorSets.emplace_back(std::move(DescriptorSet));
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
            WriteDescriptorSet.pBufferInfo = &UnorderedAccessBufferData.Descriptor;
          }
        }
      }

      VkDevice_.updateDescriptorSets(WriteDescriptorSets, nullptr);

      CurrentGraphicsCommandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, PipelineLayout_.get(), DescriptorLayoutIndex, DescriptorSet.get(), nullptr);

      CurrentGraphicsReserveData.DescriptorSets.emplace_back(std::move(DescriptorSet));
    }
  };

  //  TODO:C++20�ŏ�������
  auto BindSamplers = [&](const Samplers& _Samplers, ShaderType _Type)->void
  {
    const vdl::uint SamplerNum = static_cast<vdl::uint>(_Samplers.size());
    if (SamplerNum == 0)
    {
      return;
    }

    std::vector<DescriptorImageData> SamplerDatas;

    //  �f�[�^�̓ǂݍ���
    {
      for (vdl::uint SamplerCount = 0; SamplerCount < SamplerNum; ++SamplerCount)
      {
        DescriptorImageData& ImageData = SamplerDatas.emplace_back();
        {
          ImageData.Descriptor = GetSamplerDescriptor(_Samplers[SamplerCount]);
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
          WriteDescriptorSet.pImageInfo = &SamplerData.Descriptor;
        }
      }
    }

    VkDevice_.updateDescriptorSets(WriteDescriptorSets, nullptr);

    CurrentGraphicsCommandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, PipelineLayout_.get(), DescriptorLayoutIndex, DescriptorSet.get(), nullptr);

    CurrentGraphicsReserveData.DescriptorSets.emplace_back(std::move(DescriptorSet));
  };

  //  TODO:C++20�ŏ�������
  auto BindConstantBuffers = [&](const ConstantBuffers& _ConstantBuffers, ShaderType _Type)
  {
    const vdl::uint ConstantBufferNum = static_cast<vdl::uint>(_ConstantBuffers.size());
    if (ConstantBufferNum == 0)
    {
      return;
    }

    std::vector<DescriptorBufferData> ConstantBufferDatas;

    //  �f�[�^�̓ǂݍ���
    {
      for (vdl::uint ConstantBufferCount = 0; ConstantBufferCount < ConstantBufferNum; ++ConstantBufferCount)
      {
        const vdl::Detail::ConstantBufferData& ConstantBuffer = _ConstantBuffers[ConstantBufferCount];
        if (ConstantBuffer.isEmpty())
        {
          continue;
        }

        DescriptorBufferData& BufferData = ConstantBufferDatas.emplace_back();
        {
          BufferData.Descriptor = GetConstantBufferDescriptor(ConstantBuffer);
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
          WriteDescriptorSet.pBufferInfo = &ConstantBufferData.Descriptor;
        }
      }
    }

    VkDevice_.updateDescriptorSets(WriteDescriptorSets, nullptr);

    CurrentGraphicsCommandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, PipelineLayout_.get(), DescriptorLayoutIndex, DescriptorSet.get(), nullptr);

    CurrentGraphicsReserveData.DescriptorSets.emplace_back(std::move(DescriptorSet));
  };

  CInstanceBuffer* pInstanceBuffer = nullptr;
  bool ScissorEnable = true;
  vdl::Viewport Viewport;
  std::vector<vk::PipelineShaderStageCreateInfo> PipelineShaderStageInfos;

  //  PipelineShaderStageInfos�ɑ��݂��Ă��邩�m�F
  auto FindShader = [&](vk::ShaderStageFlagBits _ShaderStage)
  {
    const vdl::uint ShaderStageNum = static_cast<vdl::uint>(PipelineShaderStageInfos.size());

    for (vdl::uint i = 0; i < ShaderStageNum; ++i)
    {
      if (PipelineShaderStageInfos[i].stage == _ShaderStage)
      {
        return i;
      }
    }

    return ShaderStageNum;
  };

  //  DrawCommand�̑O����
  auto PreDrawCommand = [&](vdl::uint _Index)
  {
    BeginRenderPassGraphicsCommandBuffer();

    if (isChangeGraphicsPipelineState_)
    {
      std::vector<vk::UniquePipeline>& CurrentGraphicsPipelines = CurrentGraphicsReserveData.Pipelines;
      const vk::Pipeline& PreviousPipeline = CurrentGraphicsPipelines.back().get();
      GraphicsPipelineInfo_.stageCount = static_cast<vdl::uint>(PipelineShaderStageInfos.size());
      GraphicsPipelineInfo_.pStages = PipelineShaderStageInfos.data();
      GraphicsPipelineInfo_.flags = (PreviousPipeline == VK_NULL_HANDLE ? vk::PipelineCreateFlagBits::eAllowDerivatives : vk::PipelineCreateFlagBits::eAllowDerivatives | vk::PipelineCreateFlagBits::eDerivative);
      GraphicsPipelineInfo_.basePipelineHandle = PreviousPipeline;
      GraphicsPipelineInfo_.basePipelineIndex = (PreviousPipeline == VK_NULL_HANDLE ? 0 : -1);

      vk::UniquePipeline Pipeline = VkDevice_.createGraphicsPipelineUnique(GraphicsPipelineCache_.get(), GraphicsPipelineInfo_);
      assert(Pipeline);

      CurrentGraphicsCommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, Pipeline.get());

      CurrentGraphicsPipelines.emplace_back(std::move(Pipeline));

      isChangeGraphicsPipelineState_ = false;
    }

    if (pInstanceBuffer)
    {
      const auto& InstanceDatas = _GraphicsCommandList.GetInstanceDatas(_Index);
      pDevice_->WriteMemory(pInstanceBuffer, InstanceDatas.data(), static_cast<vdl::uint>(InstanceDatas.size()));

      CurrentGraphicsCommandBuffer.bindVertexBuffers(1, pInstanceBuffer->BufferData.Buffer.get(), pInstanceBuffer->PreviousOffset);
    }
  };

  //  �R�}���h�̏�������
  for (auto& GraphicsCommand : _GraphicsCommandList.GetGraphicsCommands())
  {
    switch (GraphicsCommand.first)
    {
    case GraphicsCommandFlag::eDraw:
    {
      PreDrawCommand(GraphicsCommand.second);

      const DrawData& DrawData = _GraphicsCommandList.GetDrawData(GraphicsCommand.second);
      CurrentGraphicsCommandBuffer.draw(DrawData.VertexCount, DrawData.InstanceCount, DrawData.FirstVertex, DrawData.FirstInstance);
    }
    break;
    case GraphicsCommandFlag::eDrawIndexed:
    {
      PreDrawCommand(GraphicsCommand.second);

      const DrawIndexedData& DrawIndexedData = _GraphicsCommandList.GetDrawIndexedData(GraphicsCommand.second);
      CurrentGraphicsCommandBuffer.drawIndexed(DrawIndexedData.IndexCount, DrawIndexedData.InstanceCount, DrawIndexedData.FirstIndex, DrawIndexedData.VertexOffset, DrawIndexedData.FirstInstance);
    }
    break;
    case GraphicsCommandFlag::eSetVertexBuffer:
    {
      const CVertexBuffer* pVertexBuffer = Cast<CVertexBuffer>(pBufferManager_->GetBuffer(_GraphicsCommandList.GetVertexBuffer(GraphicsCommand.second).GetID()));

      constexpr vk::DeviceSize kOffset = 0;
      CurrentGraphicsCommandBuffer.bindVertexBuffers(0, pVertexBuffer->BufferData.Buffer.get(), kOffset);
    }
    break;
    case GraphicsCommandFlag::eSetInstanceBuffer:
    {
      pInstanceBuffer = Cast<CInstanceBuffer>(pBufferManager_->GetBuffer(_GraphicsCommandList.GetInstanceBuffer().GetID()));
    }
    break;
    case GraphicsCommandFlag::eSetIndexBuffer:
    {
      const CIndexBuffer* pIndexBuffer = Cast<CIndexBuffer>(pBufferManager_->GetBuffer(_GraphicsCommandList.GetIndexBuffer(GraphicsCommand.second).GetID()));

      constexpr vk::DeviceSize kOffset = 0;
      CurrentGraphicsCommandBuffer.bindIndexBuffer(pIndexBuffer->BufferData.Buffer.get(), kOffset, pIndexBuffer->IndexType);
    }
    break;
    case GraphicsCommandFlag::eSetInputLayout:
    {
      const vdl::InputLayoutType& InputLayoutType = _GraphicsCommandList.GetInputLayout(GraphicsCommand.second);
      if (PipelineVertexInputStateInfos_.find(InputLayoutType) == PipelineVertexInputStateInfos_.cend())
      {
        vk::PipelineVertexInputStateCreateInfo PipelineVertexInputStateInfo;
        {
          const InputLayout& InputLayout = InputLayouts_[InputLayoutType];

          PipelineVertexInputStateInfo.vertexBindingDescriptionCount = static_cast<vdl::uint>(InputLayout.VertexInputBindingDescriptions.size());
          PipelineVertexInputStateInfo.pVertexBindingDescriptions = InputLayout.VertexInputBindingDescriptions.data();
          PipelineVertexInputStateInfo.vertexAttributeDescriptionCount = static_cast<vdl::uint>(InputLayout.VertexInputAttributeDescriptions.size());
          PipelineVertexInputStateInfo.pVertexAttributeDescriptions = InputLayout.VertexInputAttributeDescriptions.data();
        }

        PipelineVertexInputStateInfos_.insert(std::make_pair(InputLayoutType, std::move(PipelineVertexInputStateInfo)));
      }

      GraphicsPipelineInfo_.pVertexInputState = &PipelineVertexInputStateInfos_[InputLayoutType];
      isChangeGraphicsPipelineState_ = true;
    }
    break;
    case GraphicsCommandFlag::eSetTopology:
    {
      const vdl::TopologyType& TopologyType = _GraphicsCommandList.GetTopology(GraphicsCommand.second);
      if (Topologys_.find(TopologyType) == Topologys_.cend())
      {
        Topology Toplogy;
        {
          Toplogy.PipelineInputAssemblyStateInfo.topology = Cast(TopologyType);
          Toplogy.PipelineInputAssemblyStateInfo.primitiveRestartEnable = false;
          Toplogy.PipelineTessellationStateInfo.patchControlPoints = GetPatchControlPoints(TopologyType);
        }

        Topologys_.insert(std::make_pair(TopologyType, std::move(Toplogy)));
      }

      GraphicsPipelineInfo_.pInputAssemblyState = &Topologys_[TopologyType].PipelineInputAssemblyStateInfo;
      GraphicsPipelineInfo_.pTessellationState = &Topologys_[TopologyType].PipelineTessellationStateInfo;
      isChangeGraphicsPipelineState_ = true;
    }
    break;
    case GraphicsCommandFlag::eSetScissor:
    {
      CurrentGraphicsCommandBuffer.setScissor(0, Cast(ScissorEnable ? _GraphicsCommandList.GetScissor(GraphicsCommand.second)
        : vdl::Scissor(Viewport.LeftTop, Viewport.Size)));
    }
    break;
    case GraphicsCommandFlag::eSetViewport:
    {
      Viewport = _GraphicsCommandList.GetViewport(GraphicsCommand.second);

      CurrentGraphicsCommandBuffer.setViewport(0, Cast(Viewport));
    }
    break;
    case GraphicsCommandFlag::eSetBlendState:
    {
      const vdl::BlendState& BlendState = _GraphicsCommandList.GetBlendState(GraphicsCommand.second);
      if (PipelineColorBlendStateInfo_.attachmentCount)
      {
        if (BlendState.IndependentBlendEnable)
        {
          for (vdl::uint i = 0; i < PipelineColorBlendStateInfo_.attachmentCount; ++i)
          {
            PipelineColorBlendAttachmentStates_[i] = GetPipelineColorBlendAttachmentState(BlendState.RenderTexture[i]);
          }
        }
        else
        {
          PipelineColorBlendAttachmentStates_[0] = GetPipelineColorBlendAttachmentState(BlendState.RenderTexture[0]);
          for (vdl::uint i = 1; i < PipelineColorBlendStateInfo_.attachmentCount; ++i)
          {
            PipelineColorBlendAttachmentStates_[i] = GetPipelineColorBlendAttachmentState(BlendState.RenderTexture[i]);
          }
        }
      }
      GraphicsPipelineInfo_.pMultisampleState = &GetMultisampleStateInfo(BlendState.AlphaToCoverageEnable);
      isChangeGraphicsPipelineState_ = true;
    }
    break;
    case GraphicsCommandFlag::eSetDepthStencilState:
    {
      GraphicsPipelineInfo_.pDepthStencilState = &GetPipelineDepthStencilStateInfo(_GraphicsCommandList.GetDepthStencilState(GraphicsCommand.second));
      isChangeGraphicsPipelineState_ = true;
    }
    break;
    case GraphicsCommandFlag::eSetRasterizerState:
    {
      const vdl::RasterizerState& RasterizerState = _GraphicsCommandList.GetRasterizerState(GraphicsCommand.second);
      ScissorEnable = RasterizerState.ScissorEnable;

      GraphicsPipelineInfo_.pRasterizationState = &GetPipelineRasterizationStateInfo(RasterizerState);
      isChangeGraphicsPipelineState_ = true;
    }
    break;
    case GraphicsCommandFlag::eSetVertexShader:
    {
      constexpr vk::ShaderStageFlagBits kShaderStage = vk::ShaderStageFlagBits::eVertex;

      if (const vdl::VertexShader& VertexShader = _GraphicsCommandList.GetVertexShader(GraphicsCommand.second);
        !VertexShader.isEmpty())
      {
        const CVertexShader* pVertexShader = Cast<CVertexShader>(pShaderManager_->GetShader(VertexShader.GetID()));

        //  ���ɑ��݂��Ă���̂ŏ㏑��
        if (const vdl::uint PipelineShaderStageCount = FindShader(kShaderStage);
          PipelineShaderStageCount < PipelineShaderStageInfos.size())
        {
          PipelineShaderStageInfos[PipelineShaderStageCount].module = pVertexShader->ShaderData.Module.get();
          PipelineShaderStageInfos[PipelineShaderStageCount].pName = pVertexShader->ShaderData.EntryPoint;
        }
        //  �V�K�ǉ�
        else
        {
          vk::PipelineShaderStageCreateInfo& PipelineShaderStageInfo = PipelineShaderStageInfos.emplace_back();
          {
            PipelineShaderStageInfo.stage = kShaderStage;
            PipelineShaderStageInfo.module = pVertexShader->ShaderData.Module.get();
            PipelineShaderStageInfo.pName = pVertexShader->ShaderData.EntryPoint;
          }
        }

        isChangeGraphicsPipelineState_ = true;
      }
      else
      {
        //  �f�[�^�����݂��Ă�΍폜
        for (auto Itr = PipelineShaderStageInfos.begin(), End = PipelineShaderStageInfos.end(); Itr != End; ++Itr)
        {
          if (Itr->stage == kShaderStage)
          {
            Itr = PipelineShaderStageInfos.erase(Itr);
            isChangeGraphicsPipelineState_ = true;
            break;
          }
        }
      }
    }
    break;
    case GraphicsCommandFlag::eSetHullShader:
    {
      constexpr vk::ShaderStageFlagBits kShaderStage = vk::ShaderStageFlagBits::eTessellationControl;

      if (const vdl::HullShader& HullShader = _GraphicsCommandList.GetHullShader(GraphicsCommand.second);
        !HullShader.isEmpty())
      {
        const CHullShader* pHullShader = Cast<CHullShader>(pShaderManager_->GetShader(HullShader.GetID()));

        //  ���ɑ��݂��Ă���̂ŏ㏑��
        if (const vdl::uint PipelineShaderStageCount = FindShader(kShaderStage);
          PipelineShaderStageCount < PipelineShaderStageInfos.size())
        {
          PipelineShaderStageInfos[PipelineShaderStageCount].module = pHullShader->ShaderData.Module.get();
          PipelineShaderStageInfos[PipelineShaderStageCount].pName = pHullShader->ShaderData.EntryPoint;
        }
        //  �V�K�ǉ�
        else
        {
          vk::PipelineShaderStageCreateInfo& PipelineShaderStageInfo = PipelineShaderStageInfos.emplace_back();
          {
            PipelineShaderStageInfo.stage = kShaderStage;
            PipelineShaderStageInfo.module = pHullShader->ShaderData.Module.get();
            PipelineShaderStageInfo.pName = pHullShader->ShaderData.EntryPoint;
          }
        }

        isChangeGraphicsPipelineState_ = true;
      }
      //  �f�[�^�����݂��Ă�΍폜
      else
      {
        for (auto Itr = PipelineShaderStageInfos.begin(), End = PipelineShaderStageInfos.end(); Itr != End; ++Itr)
        {
          if (Itr->stage == kShaderStage)
          {
            Itr = PipelineShaderStageInfos.erase(Itr);
            isChangeGraphicsPipelineState_ = true;
            break;
          }
        }
      }
    }
    break;
    case GraphicsCommandFlag::eSetDomainShader:
    {
      constexpr vk::ShaderStageFlagBits kShaderStage = vk::ShaderStageFlagBits::eTessellationEvaluation;

      if (const vdl::DomainShader& DomainShader = _GraphicsCommandList.GetDomainShader(GraphicsCommand.second);
        !DomainShader.isEmpty())
      {
        const CDomainShader* pDomainShader = Cast<CDomainShader>(pShaderManager_->GetShader(DomainShader.GetID()));

        //  ���ɑ��݂��Ă���̂ŏ㏑��
        if (const vdl::uint PipelineShaderStageCount = FindShader(kShaderStage);
          PipelineShaderStageCount < PipelineShaderStageInfos.size())
        {
          PipelineShaderStageInfos[PipelineShaderStageCount].module = pDomainShader->ShaderData.Module.get();
          PipelineShaderStageInfos[PipelineShaderStageCount].pName = pDomainShader->ShaderData.EntryPoint;
        }
        //  �V�K�ǉ�
        else
        {
          vk::PipelineShaderStageCreateInfo& PipelineShaderStageInfo = PipelineShaderStageInfos.emplace_back();
          {
            PipelineShaderStageInfo.stage = kShaderStage;
            PipelineShaderStageInfo.module = pDomainShader->ShaderData.Module.get();
            PipelineShaderStageInfo.pName = pDomainShader->ShaderData.EntryPoint;
          }
        }

        isChangeGraphicsPipelineState_ = true;
      }
      //  �f�[�^�����݂��Ă�΍폜
      else
      {
        for (auto Itr = PipelineShaderStageInfos.begin(), End = PipelineShaderStageInfos.end(); Itr != End; ++Itr)
        {
          if (Itr->stage == kShaderStage)
          {
            Itr = PipelineShaderStageInfos.erase(Itr);
            isChangeGraphicsPipelineState_ = true;
            break;
          }
        }
      }
    }
    break;
    case GraphicsCommandFlag::eSetGeometryShader:
    {
      constexpr vk::ShaderStageFlagBits kShaderStage = vk::ShaderStageFlagBits::eGeometry;

      if (const vdl::GeometryShader& GeometryShader = _GraphicsCommandList.GetGeometryShader(GraphicsCommand.second);
        !GeometryShader.isEmpty())
      {
        const CGeometryShader* pGeometryShader = Cast<CGeometryShader>(pShaderManager_->GetShader(GeometryShader.GetID()));

        //  ���ɑ��݂��Ă���̂ŏ㏑��
        if (const vdl::uint PipelineShaderStageCount = FindShader(kShaderStage);
          PipelineShaderStageCount < PipelineShaderStageInfos.size())
        {
          PipelineShaderStageInfos[PipelineShaderStageCount].module = pGeometryShader->ShaderData.Module.get();
          PipelineShaderStageInfos[PipelineShaderStageCount].pName = pGeometryShader->ShaderData.EntryPoint;
        }
        //  �V�K�ǉ�
        else
        {
          vk::PipelineShaderStageCreateInfo& PipelineShaderStageInfo = PipelineShaderStageInfos.emplace_back();
          {
            PipelineShaderStageInfo.stage = kShaderStage;
            PipelineShaderStageInfo.module = pGeometryShader->ShaderData.Module.get();
            PipelineShaderStageInfo.pName = pGeometryShader->ShaderData.EntryPoint;
          }
        }

        isChangeGraphicsPipelineState_ = true;
      }
      //  �f�[�^�����݂��Ă�΍폜
      else
      {
        for (auto Itr = PipelineShaderStageInfos.begin(), End = PipelineShaderStageInfos.end(); Itr != End; ++Itr)
        {
          if (Itr->stage == kShaderStage)
          {
            Itr = PipelineShaderStageInfos.erase(Itr);
            isChangeGraphicsPipelineState_ = true;
            break;
          }
        }
      }
    }
    break;
    case GraphicsCommandFlag::eSetPixelShader:
    {
      constexpr vk::ShaderStageFlagBits kShaderStage = vk::ShaderStageFlagBits::eFragment;

      if (const vdl::PixelShader& PixelShader = _GraphicsCommandList.GetPixelShader(GraphicsCommand.second);
        !PixelShader.isEmpty())
      {
        const CPixelShader* pPixelShader = Cast<CPixelShader>(pShaderManager_->GetShader(PixelShader.GetID()));

        //  ���ɑ��݂��Ă���̂ŏ㏑��
        if (const vdl::uint PipelineShaderStageCount = FindShader(kShaderStage);
          PipelineShaderStageCount < PipelineShaderStageInfos.size())
        {
          PipelineShaderStageInfos[PipelineShaderStageCount].module = pPixelShader->ShaderData.Module.get();
          PipelineShaderStageInfos[PipelineShaderStageCount].pName = pPixelShader->ShaderData.EntryPoint;
        }
        //  �V�K�ǉ�
        else
        {
          vk::PipelineShaderStageCreateInfo& PipelineShaderStageInfo = PipelineShaderStageInfos.emplace_back();
          {
            PipelineShaderStageInfo.stage = kShaderStage;
            PipelineShaderStageInfo.module = pPixelShader->ShaderData.Module.get();
            PipelineShaderStageInfo.pName = pPixelShader->ShaderData.EntryPoint;
          }
        }

        isChangeGraphicsPipelineState_ = true;
      }
      //  �f�[�^�����݂��Ă�΍폜
      else
      {
        for (auto Itr = PipelineShaderStageInfos.begin(), End = PipelineShaderStageInfos.end(); Itr != End; ++Itr)
        {
          if (Itr->stage == kShaderStage)
          {
            Itr = PipelineShaderStageInfos.erase(Itr);
            isChangeGraphicsPipelineState_ = true;
            break;
          }
        }
      }
    }
    break;
    case GraphicsCommandFlag::eSetVertexStageShaderResource:
    {
      BindShaderResources(_GraphicsCommandList.GetShaderResources<ShaderType::eVertexShader>(GraphicsCommand.second), ShaderType::eVertexShader);
    }
    break;
    case GraphicsCommandFlag::eSetHullStageShaderResource:
    {
      BindShaderResources(_GraphicsCommandList.GetShaderResources<ShaderType::eHullShader>(GraphicsCommand.second), ShaderType::eHullShader);
    }
    break;
    case GraphicsCommandFlag::eSetDomainStageShaderResource:
    {
      BindShaderResources(_GraphicsCommandList.GetShaderResources<ShaderType::eDomainShader>(GraphicsCommand.second), ShaderType::eDomainShader);
    }
    break;
    case GraphicsCommandFlag::eSetGeometryStageShaderResource:
    {
      BindShaderResources(_GraphicsCommandList.GetShaderResources<ShaderType::eGeometryShader>(GraphicsCommand.second), ShaderType::eGeometryShader);
    }
    break;
    case GraphicsCommandFlag::eSetPixelStageShaderResource:
    {
      BindShaderResources(_GraphicsCommandList.GetShaderResources<ShaderType::ePixelShader>(GraphicsCommand.second), ShaderType::ePixelShader);
    }
    break;
    case GraphicsCommandFlag::eSetVertexStageSampler:
    {
      BindSamplers(_GraphicsCommandList.GetSamplers<ShaderType::eVertexShader>(GraphicsCommand.second), ShaderType::eVertexShader);
    }
    break;
    case GraphicsCommandFlag::eSetHullStageSampler:
    {
      BindSamplers(_GraphicsCommandList.GetSamplers<ShaderType::eHullShader>(GraphicsCommand.second), ShaderType::eHullShader);
    }
    break;
    case GraphicsCommandFlag::eSetDomainStageSampler:
    {
      BindSamplers(_GraphicsCommandList.GetSamplers<ShaderType::eDomainShader>(GraphicsCommand.second), ShaderType::eDomainShader);
    }
    break;
    case GraphicsCommandFlag::eSetGeometryStageSampler:
    {
      BindSamplers(_GraphicsCommandList.GetSamplers<ShaderType::eGeometryShader>(GraphicsCommand.second), ShaderType::eGeometryShader);
    }
    break;
    case GraphicsCommandFlag::eSetPixelStageSampler:
    {
      BindSamplers(_GraphicsCommandList.GetSamplers<ShaderType::ePixelShader>(GraphicsCommand.second), ShaderType::ePixelShader);
    }
    break;
    case GraphicsCommandFlag::eSetVertexStageConstantBuffer:
    {
      BindConstantBuffers(_GraphicsCommandList.GetConstantBuffers<ShaderType::eVertexShader>(GraphicsCommand.second), ShaderType::eVertexShader);
    }
    break;
    case GraphicsCommandFlag::eSetHullStageConstantBuffer:
    {
      BindConstantBuffers(_GraphicsCommandList.GetConstantBuffers<ShaderType::eHullShader>(GraphicsCommand.second), ShaderType::eHullShader);
    }
    break;
    case GraphicsCommandFlag::eSetDomainStageConstantBuffer:
    {
      BindConstantBuffers(_GraphicsCommandList.GetConstantBuffers<ShaderType::eDomainShader>(GraphicsCommand.second), ShaderType::eDomainShader);
    }
    break;
    case GraphicsCommandFlag::eSetGeometryStageConstantBuffer:
    {
      BindConstantBuffers(_GraphicsCommandList.GetConstantBuffers<ShaderType::eGeometryShader>(GraphicsCommand.second), ShaderType::eGeometryShader);
    }
    break;
    case GraphicsCommandFlag::eSetPixelStageConstantBuffer:
    {
      BindConstantBuffers(_GraphicsCommandList.GetConstantBuffers<ShaderType::ePixelShader>(GraphicsCommand.second), ShaderType::ePixelShader);
    }
    break;
    default: assert(false);
    }
  }
}

void CDeviceContext::Execute(const ComputeCommandList& _ComputeCommandList)
{
  //  �f�[�^��ۗL���鎖�Ń��t�@�����X�J�E���g�̈ێ�
  ComputeCommandListDatas_[ComputeCommandBufferIndex_] = _ComputeCommandList;

  ComputeReserveData& CurrentComputeReserveData = GetCurrentComputeReserveData();

  //  �R�}���h���X�g�̏����ݒ�
  const vk::CommandBuffer& CurrentComputeCommandBuffer = GetCurrentComputeCommandBuffer();
  {
    CurrentComputeCommandBuffer.reset(vk::CommandBufferResetFlags());
    CurrentComputeCommandBuffer.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
  }

  BeginGraphicsCommandBuffer();
  const vk::CommandBuffer& CurrentGraphicsCommandBuffer = GetCurrentGraphicsCommandBuffer();

  //  �R�}���h�̔��s
  for (auto& ComputeCommand : _ComputeCommandList.GetComputeCommands())
  {
    switch (ComputeCommand)
    {
    case ComputeCommandFlag::eSetShader:
    {
      assert(!_ComputeCommandList.GetComputeShader().isEmpty());
      const CComputeShader* pComputeShader = Cast<CComputeShader>(pShaderManager_->GetShader(_ComputeCommandList.GetComputeShader().GetID()));

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

      vk::UniquePipeline Pipeline = VkDevice_.createComputePipelineUnique(ComputePipelineCache_.get(), ComputePipelineInfo);
      assert(Pipeline);

      LastComputePipeline_ = Pipeline.get();
      CurrentComputeReserveData.Pipelines.emplace_back(std::move(Pipeline));
    }
    break;
    case ComputeCommandFlag::eSetSampler:
    {
      const Samplers& Samplers = _ComputeCommandList.GetSamplers();
      const vdl::uint SamplerNum = static_cast<vdl::uint>(Samplers.size());

      if (SamplerNum == 0)
      {
        continue;
      }

      //  �f�[�^�̓ǂݍ���
      std::vector<DescriptorImageData> SamplerDatas;
      {
        for (vdl::uint SamplerCount = 0; SamplerCount < SamplerNum; ++SamplerCount)
        {
          DescriptorImageData& ImageData = SamplerDatas.emplace_back();
          {
            ImageData.Descriptor = GetSamplerDescriptor(Samplers[SamplerCount]);
            ImageData.Bind = SamplerCount;
          }
        }
      }

      if (SamplerDatas.empty())
      {
        continue;
      }

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
            WriteDescriptorSet.pImageInfo = &SamplerData.Descriptor;
          }
        }
      }

      VkDevice_.updateDescriptorSets(WriteDescriptorSets, nullptr);

      CurrentComputeCommandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eCompute, PipelineLayout_.get(), GetDescriptorLayoutOffset(ShaderType::eComputeShader, kDescriptorType), DescriptorSet.get(), nullptr);

      CurrentComputeReserveData.SamplerDescriptorSets.emplace_back(std::move(DescriptorSet));
    }
    break;
    case ComputeCommandFlag::eSetConstantBuffer:
    {
      const ConstantBuffers& ConstantBuffers = _ComputeCommandList.GetConstantBuffers();
      const vdl::uint ConstantBufferNum = static_cast<vdl::uint>(ConstantBuffers.size());

      if (ConstantBufferNum == 0)
      {
        continue;
      }

      //  �f�[�^�̓ǂݍ���
      std::vector<DescriptorBufferData> ConstantBufferDatas;
      {
        for (vdl::uint ConstantBufferCount = 0; ConstantBufferCount < ConstantBufferNum; ++ConstantBufferCount)
        {
          const vdl::Detail::ConstantBufferData& ConstantBuffer = ConstantBuffers[ConstantBufferCount];
          if (ConstantBuffer.isEmpty())
          {
            continue;
          }

          DescriptorBufferData& BufferData = ConstantBufferDatas.emplace_back();
          {
            BufferData.Descriptor = GetConstantBufferDescriptor(ConstantBuffer);
            BufferData.Bind = ConstantBufferCount;
          }
        }
      }

      if (ConstantBufferDatas.empty())
      {
        continue;
      }

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
            WriteDescriptorSet.pBufferInfo = &ConstantBufferData.Descriptor;
          }
        }
      }

      VkDevice_.updateDescriptorSets(WriteDescriptorSets, nullptr);

      CurrentComputeCommandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eCompute, PipelineLayout_.get(), GetDescriptorLayoutOffset(ShaderType::eComputeShader, kDescriptorType), DescriptorSet.get(), nullptr);

      CurrentComputeReserveData.ConstantBufferDescriptorSets.emplace_back(std::move(DescriptorSet));
    }
    break;
    default:
      break;
    }
  }

  //  ShaderResouce,UnorderedAccessObject��ImageLayout�̕ύX��Descriptor�̐ݒ�
  //  TODO:�R�}���h�݂̂œ����悤�ɂ���
  {
    //  SetShaderResource
    {
      const ShaderResources& ShaderResources = _ComputeCommandList.GetShaderResources();
      const vdl::uint ShaderResourceNum = static_cast<vdl::uint>(ShaderResources.size());

      if (ShaderResourceNum)
      {
        std::vector<DescriptorImageData> TextureDatas;
        std::vector<DescriptorBufferData> UnorderedAccessBufferDatas;

        //  �f�[�^�̓ǂݍ���
        {
          for (vdl::uint ShaderResourceCount = 0; ShaderResourceCount < ShaderResourceNum; ++ShaderResourceCount)
          {
            const vdl::ShaderResource& ShaderResource = ShaderResources[ShaderResourceCount];

            ITexture* pTexture = nullptr;
            {
              if (const vdl::Texture* pShaderResource = ShaderResource.GetIf<vdl::Texture>())
              {
                if (!pShaderResource->isEmpty())
                {
                  pTexture = pTextureManager_->GetTexture(pShaderResource->GetID());
                }
              }
              else if (const vdl::CubeTexture* pShaderResource = ShaderResource.GetIf<vdl::CubeTexture>())
              {
                if (!pShaderResource->isEmpty())
                {
                  pTexture = pTextureManager_->GetTexture(pShaderResource->GetID());
                }
              }
            }

            //  Texture
            if (pTexture)
            {
              constexpr vk::ImageLayout kImageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

              DescriptorImageData& ImageData = TextureDatas.emplace_back();
              {
                ImageData.Descriptor = GetTextureDescriptor(pTexture, CurrentGraphicsCommandBuffer);
                ImageData.Bind = ShaderResourceCount;
              }
            }
            //  UnorderedAccessBuffer
            else if (const vdl::Detail::UnorderedAccessBufferData* pShaderResource = ShaderResource.GetIf<vdl::Detail::UnorderedAccessBufferData>())
            {
              if (pShaderResource->isEmpty())
              {
                continue;
              }

              DescriptorBufferData& BufferData = UnorderedAccessBufferDatas.emplace_back();
              {
                BufferData.Descriptor = GetUnorderedAccessBufferDescriptor(*pShaderResource);
                BufferData.Bind = ShaderResourceCount;
              }
            }
          }
        }

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
                WriteDescriptorSet.pImageInfo = &TextureData.Descriptor;
              }
            }
          }

          VkDevice_.updateDescriptorSets(WriteDescriptorSets, nullptr);

          CurrentComputeCommandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eCompute, PipelineLayout_.get(), GetDescriptorLayoutOffset(ShaderType::eComputeShader, kDescriptorType), DescriptorSet.get(), nullptr);

          CurrentComputeReserveData.TextureDescriptorSets.emplace_back(std::move(DescriptorSet));
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
                WriteDescriptorSet.pBufferInfo = &UnorderedAccessBufferData.Descriptor;
              }
            }
          }

          VkDevice_.updateDescriptorSets(WriteDescriptorSets, nullptr);

          CurrentComputeReserveData.BufferDescriptorSets.emplace_back(std::move(DescriptorSet));
        }
      }
    }

    //  SetUnorderedAccessObject
    {
      const UnorderedAccessObjects& UnorderedAccessObjects = _ComputeCommandList.GetUnorderedAccessObjects();
      const vdl::uint UnorderedAccessObjectNum = static_cast<vdl::uint>(UnorderedAccessObjects.size());

      if (UnorderedAccessObjectNum)
      {
        std::vector<DescriptorImageData> UnorderedAccessTextureDatas;
        std::vector<DescriptorBufferData> UnorderedAccessBufferDatas;

        //  �f�[�^�̓ǂݍ���
        {
          for (vdl::uint UnorderedAccessObjectCount = 0; UnorderedAccessObjectCount < UnorderedAccessObjectNum; ++UnorderedAccessObjectCount)
          {
            const vdl::UnorderedAccessObject& UnorderedAccessObject = UnorderedAccessObjects[UnorderedAccessObjectCount];

            //  UnorderedAccessTexture
            if (const vdl::UnorderedAccessTexture* pUnorderedAccessObject = UnorderedAccessObject.GetIf<vdl::UnorderedAccessTexture>())
            {
              if (pUnorderedAccessObject->isEmpty())
              {
                continue;
              }

              DescriptorImageData& ImageData = UnorderedAccessTextureDatas.emplace_back();
              {
                ImageData.Descriptor = GetUnorderedAccessTextureDescriptor(*pUnorderedAccessObject, CurrentGraphicsCommandBuffer);
                ImageData.Bind = UnorderedAccessObjectCount;
              }
            }
            //  UnorderedAccessBuffer
            else if (const vdl::Detail::UnorderedAccessBufferData* pUnorderedAccessBuffer = UnorderedAccessObject.GetIf<vdl::Detail::UnorderedAccessBufferData>())
            {
              if (pUnorderedAccessBuffer->isEmpty())
              {
                continue;
              }

              DescriptorBufferData& BufferData = UnorderedAccessBufferDatas.emplace_back();
              {
                BufferData.Descriptor = GetUnorderedAccessBufferDescriptor(*pUnorderedAccessBuffer);
                BufferData.Bind = UnorderedAccessObjectCount;
              }
            }
          }
        }

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
                WriteDescriptorSet.pImageInfo = &UnorderedAccessTextureData.Descriptor;
              }
            }
          }

          VkDevice_.updateDescriptorSets(WriteDescriptorSets, nullptr);

          CurrentComputeCommandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eCompute, PipelineLayout_.get(), GetDescriptorLayoutOffset(ShaderType::eComputeShader, kDescriptorType), DescriptorSet.get(), nullptr);

          CurrentComputeReserveData.UnorderedAccessTextureDescriptorSets.emplace_back(std::move(DescriptorSet));
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
                WriteDescriptorSet.pBufferInfo = &UnorderedAccessBufferData.Descriptor;
              }
            }
          }

          VkDevice_.updateDescriptorSets(WriteDescriptorSets, nullptr);

          CurrentComputeCommandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eCompute, PipelineLayout_.get(), GetDescriptorLayoutOffset(ShaderType::eComputeShader, kDescriptorType), DescriptorSet.get(), nullptr);

          CurrentComputeReserveData.UnorderedAccessBufferDescriptorSets.emplace_back(std::move(DescriptorSet));
        }
      }
    }
  }

  Flush();

  //  SetPipeline
  CurrentComputeCommandBuffer.bindPipeline(vk::PipelineBindPoint::eCompute, LastComputePipeline_);

  //  SetDescriptor
  {
    if (const vk::UniqueDescriptorSet& DescriptorSet = CurrentComputeReserveData.TextureDescriptorSets.back())
    {
      CurrentComputeCommandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eCompute, PipelineLayout_.get(), GetDescriptorLayoutOffset(ShaderType::eComputeShader, DescriptorType::eTexture), DescriptorSet.get(), nullptr);
    }

    if (const vk::UniqueDescriptorSet& DescriptorSet = CurrentComputeReserveData.BufferDescriptorSets.back())
    {
      CurrentComputeCommandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eCompute, PipelineLayout_.get(), GetDescriptorLayoutOffset(ShaderType::eComputeShader, DescriptorType::eBuffer), DescriptorSet.get(), nullptr);
    }

    if (const vk::UniqueDescriptorSet& DescriptorSet = CurrentComputeReserveData.SamplerDescriptorSets.back())
    {
      CurrentComputeCommandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eCompute, PipelineLayout_.get(), GetDescriptorLayoutOffset(ShaderType::eComputeShader, DescriptorType::eSampler), DescriptorSet.get(), nullptr);
    }

    if (const vk::UniqueDescriptorSet& DescriptorSet = CurrentComputeReserveData.ConstantBufferDescriptorSets.back())
    {
      CurrentComputeCommandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eCompute, PipelineLayout_.get(), GetDescriptorLayoutOffset(ShaderType::eComputeShader, DescriptorType::eConstantBuffer), DescriptorSet.get(), nullptr);
    }

    if (const vk::UniqueDescriptorSet& DescriptorSet = CurrentComputeReserveData.UnorderedAccessBufferDescriptorSets.back())
    {
      CurrentComputeCommandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eCompute, PipelineLayout_.get(), GetDescriptorLayoutOffset(ShaderType::eComputeShader, DescriptorType::eUnorderedAccessBuffer), DescriptorSet.get(), nullptr);
    }

    if (const vk::UniqueDescriptorSet& DescriptorSet = CurrentComputeReserveData.UnorderedAccessTextureDescriptorSets.back())
    {
      CurrentComputeCommandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eCompute, PipelineLayout_.get(), GetDescriptorLayoutOffset(ShaderType::eComputeShader, DescriptorType::eUnorderedAccessTexture), DescriptorSet.get(), nullptr);
    }
  }

  const vdl::uint3& ThreadGroup = _ComputeCommandList.GetThreadGroup();
  CurrentComputeCommandBuffer.dispatch(ThreadGroup.x, ThreadGroup.y, ThreadGroup.z);
  CurrentComputeCommandBuffer.end();

  std::vector<vk::Semaphore> WaitSemaphores;
  {
    if (LastSemaphore_)
    {
      WaitSemaphores.push_back(LastSemaphore_);
    }
    if (const vk::Semaphore PresentSemaphore = SwapChain_.GetSemaphore())
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
    SubmitInfo.pCommandBuffers = &CurrentComputeCommandBuffer;
    SubmitInfo.signalSemaphoreCount = 1;
    SubmitInfo.pSignalSemaphores = &CurrentSemaphore;
  }

  ComputeQueue_.submit(SubmitInfo, GetCurrentComputeFence());

  LastSemaphore_ = CurrentSemaphore;

  ++ComputeCommandBufferIndex_ %= Constants::kComputeCommandBufferNum;
  WaitFence(GetCurrentComputeFence());

  //  �ŏI�f�[�^�̈����p��
  {
    ComputeReserveData& NextComputeReserveData = GetCurrentComputeReserveData();
    NextComputeReserveData.Clear();
    NextComputeReserveData.Pipelines.resize(1);
    NextComputeReserveData.Pipelines[0] = std::move(CurrentComputeReserveData.Pipelines.back());
    NextComputeReserveData.TextureDescriptorSets.resize(1);
    NextComputeReserveData.TextureDescriptorSets[0] = std::move(CurrentComputeReserveData.TextureDescriptorSets.back());
    NextComputeReserveData.BufferDescriptorSets.resize(1);
    NextComputeReserveData.BufferDescriptorSets[0] = std::move(CurrentComputeReserveData.BufferDescriptorSets.back());
    NextComputeReserveData.SamplerDescriptorSets.resize(1);
    NextComputeReserveData.SamplerDescriptorSets[0] = std::move(CurrentComputeReserveData.SamplerDescriptorSets.back());
    NextComputeReserveData.ConstantBufferDescriptorSets.resize(1);
    NextComputeReserveData.ConstantBufferDescriptorSets[0] = std::move(CurrentComputeReserveData.ConstantBufferDescriptorSets.back());
    NextComputeReserveData.UnorderedAccessTextureDescriptorSets.resize(1);
    NextComputeReserveData.UnorderedAccessTextureDescriptorSets[0] = std::move(CurrentComputeReserveData.UnorderedAccessTextureDescriptorSets.back());
    NextComputeReserveData.UnorderedAccessBufferDescriptorSets.resize(1);
    NextComputeReserveData.UnorderedAccessBufferDescriptorSets[0] = std::move(CurrentComputeReserveData.UnorderedAccessBufferDescriptorSets.back());
  }
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
    if (const vk::Semaphore PresentSemaphore = SwapChain_.GetSemaphore())
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

  ++GraphicsCommandBufferIndex_ %= Constants::kGraphicsCommandBufferNum;
  WaitFence(GetCurrentGraphicsFence());
  GetCurrentGraphicsCommandBuffer().reset(vk::CommandBufferResetFlags());

  GraphicsReserveDatas_[GraphicsCommandBufferIndex_].Clear();
  GraphicsReserveDatas_[GraphicsCommandBufferIndex_].Pipelines.resize(1);
  GraphicsReserveDatas_[GraphicsCommandBufferIndex_].Pipelines[0] = std::move(GraphicsReserveDatas_[CurrentGraphicsCommandBufferIndex].Pipelines.back());

  GraphicsCommandBufferState_ = CommandBufferState::eIdle;
}

void CDeviceContext::Present()
{
  const vk::CommandBuffer& CurrentCommandBuffer = GetCurrentGraphicsCommandBuffer();

  //  �R�}���h�o�b�t�@�̏�Ԃ��J�n��Ԃɂ���
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

  //  �o�b�N�o�b�t�@��Present�\��ԂɕύX
  SwapChain_.GetVkRenderTexture()->TextureData.SetImageLayout(CurrentCommandBuffer, vk::ImageLayout::ePresentSrcKHR, { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 });

  Flush();

  vk::PresentInfoKHR PresentInfo;
  {
    PresentInfo.waitSemaphoreCount = 1;
    PresentInfo.pWaitSemaphores = &LastSemaphore_;
    PresentInfo.swapchainCount = 1;
    PresentInfo.pSwapchains = &SwapChain_.GetSwapChain();
    PresentInfo.pImageIndices = &SwapChain_.GetCurrentBufferIndex();
  }

  vk::Result Result = vk::Result::eSuccess;
  Result = GraphicsQueue_.presentKHR(PresentInfo);
  assert(Result == vk::Result::eSuccess);

  LastSemaphore_ = vk::Semaphore();

  SwapChain_.AcquireNextImage(VkDevice_);

  ClearRenderTexture(SwapChain_.GetRenderTexture(), pWindow_->GetScreenClearColor());
  ClearDepthStencilTexture(SwapChain_.GetDepthStencilTexture(), Constants::kDefaultClearDepth, Constants::kDefaultClearStencil);
}

void CDeviceContext::ChangeWindowMode()
{
  //  TODO
}

void CDeviceContext::ScreenShot()
{
  //  TODO
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

    vdl::uint AttachmentCount = 0;
    std::array<vk::ImageView, Constants::kMaxRenderTextureNum + 1> Attachments;
    //  �����_�[�p�X�̍쐬
    {
      std::array<vk::AttachmentDescription, Constants::kMaxRenderTextureNum + 1> AttachmentDescriptions;
      std::array<vk::AttachmentReference, Constants::kMaxRenderTextureNum + 1> AttachmentReferences;
      {
        //  �����_�[�^�[�Q�b�g�̐ݒ�
        {
          auto SetAttachment = [&](CRenderTexture* _pRenderTexture)->void
          {
            constexpr vk::ImageLayout kImageLayout = vk::ImageLayout::eColorAttachmentOptimal;

            vk::AttachmentDescription& AttachmentDescription = AttachmentDescriptions[AttachmentCount];
            {
              AttachmentDescription.format = _pRenderTexture->VkFormat;
              AttachmentDescription.samples = vk::SampleCountFlagBits::e1;
              AttachmentDescription.loadOp = vk::AttachmentLoadOp::eLoad;
              AttachmentDescription.storeOp = vk::AttachmentStoreOp::eStore;
              AttachmentDescription.initialLayout = _pRenderTexture->TextureData.CurrentLayout;
              _pRenderTexture->TextureData.CurrentLayout = AttachmentDescription.finalLayout = kImageLayout;
            }

            vk::AttachmentReference& AttachmentReference = AttachmentReferences[AttachmentCount];
            {
              AttachmentReference.attachment = AttachmentCount;
              AttachmentReference.layout = kImageLayout;
            }

            Attachments[AttachmentCount] = _pRenderTexture->TextureData.View.get();
          };

          for (; AttachmentCount < Constants::kMaxRenderTextureNum; ++AttachmentCount)
          {
            const vdl::RenderTexture RenderTexture = CurrentOutputManager.RenderTextures[AttachmentCount];
            if (RenderTexture.isEmpty())
            {
              break;
            }

            CRenderTexture* pRenderTexture = GetVkRenderTexture(RenderTexture);
            SetAttachment(pRenderTexture);
          }
        }

        PipelineColorBlendStateInfo_.attachmentCount = AttachmentCount;

        //  �f�v�X�X�e���V���o�b�t�@�̐ݒ�
        if (!CurrentOutputManager.DepthStencilTexture.isEmpty())
        {
          CDepthStencilTexture* pDepthStencilTexture = Cast<CDepthStencilTexture>(pTextureManager_->GetTexture(CurrentOutputManager.DepthStencilTexture.GetID()));

          constexpr vk::ImageLayout kImageLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

          vk::AttachmentDescription& AttachmentDescription = AttachmentDescriptions[AttachmentCount];
          {
            AttachmentDescription.format = pDepthStencilTexture->VkFormat;
            AttachmentDescription.samples = vk::SampleCountFlagBits::e1;
            AttachmentDescription.stencilLoadOp = vk::AttachmentLoadOp::eLoad;
            AttachmentDescription.stencilStoreOp = vk::AttachmentStoreOp::eStore;
            AttachmentDescription.initialLayout = pDepthStencilTexture->TextureData.CurrentLayout;
            pDepthStencilTexture->TextureData.CurrentLayout = AttachmentDescription.finalLayout = kImageLayout;
          }

          vk::AttachmentReference& AttachmentReference = AttachmentReferences[AttachmentCount];
          {
            AttachmentReference.attachment = AttachmentCount;
            AttachmentReference.layout = kImageLayout;
          }

          Attachments[AttachmentCount] = pDepthStencilTexture->TextureData.View.get();
          ++AttachmentCount;
        }
      }

      vk::SubpassDescription SubpassDescription;
      {
        SubpassDescription.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
        SubpassDescription.colorAttachmentCount = PipelineColorBlendStateInfo_.attachmentCount;
        SubpassDescription.pColorAttachments = AttachmentReferences.data();
        SubpassDescription.pDepthStencilAttachment = (PipelineColorBlendStateInfo_.attachmentCount == AttachmentCount ? nullptr : &AttachmentReferences[PipelineColorBlendStateInfo_.attachmentCount]);
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

    const vdl::uint2 FrameBufferSize = (CurrentOutputManager.RenderTextures[0].isEmpty() ? CurrentOutputManager.DepthStencilTexture.GetDepthTexture().GetSize() : CurrentOutputManager.RenderTextures[0].GetSize());

    //  �t���[���o�b�t�@�̍쐬
    {
      vk::FramebufferCreateInfo FramebufferInfo;
      {
        FramebufferInfo.renderPass = RenderPassData.RenderPass.get();
        FramebufferInfo.attachmentCount = AttachmentCount;
        FramebufferInfo.pAttachments = Attachments.data();
        FramebufferInfo.width = FrameBufferSize.x;
        FramebufferInfo.height = FrameBufferSize.y;
        FramebufferInfo.layers = 1;
      }

      RenderPassData.Framebuffer = VkDevice_.createFramebufferUnique(FramebufferInfo);
      assert(RenderPassData.Framebuffer);
    }
    GraphicsPipelineInfo_.renderPass = RenderPassData.RenderPass.get();

    CurrentGraphicsCommandBuffer.beginRenderPass({ RenderPassData.RenderPass.get(), RenderPassData.Framebuffer.get(),
      { { 0, 0 }, { FrameBufferSize.x, FrameBufferSize.y } } }, vk::SubpassContents::eInline);
  }

  GraphicsCommandBufferState_ = CommandBufferState::eBeginRenderPass;
}

void CDeviceContext::WaitFence(const vk::Fence& _Fence)
{
  //  Fence���L���ȊԂ͊����܂ő҂�
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

CRenderTexture* CDeviceContext::GetVkRenderTexture(const vdl::RenderTexture& _RenderTexture)
{
  assert(!_RenderTexture.isEmpty());

  ITexture* pTexture = pTextureManager_->GetTexture(_RenderTexture.GetID());
  return (pTexture->GetType() == TextureType::eSwapChainRenderTexture ? SwapChain_.GetVkRenderTexture() : Cast<CRenderTexture>(pTexture));
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
      PipelineRasterizationStateInfo.depthBiasConstantFactor = static_cast<float>(_RasterizerState.DepthBias);
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
      PipelineDepthStencilStateInfo.front.reference = PipelineDepthStencilStateInfo.back.reference = _DepthStencilState.StencilReference;
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

const vk::DescriptorImageInfo& CDeviceContext::GetTextureDescriptor(ITexture* _pTexture, const vk::CommandBuffer& _CommandBuffer)
{
  assert(_pTexture);

  constexpr vk::ImageLayout kImageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

  switch (_pTexture->GetType())
  {
  case TextureType::eDepthStencilTexture:
    assert(false);
  case TextureType::eDepthTexture:
  {
    CDepthTexture* pDepthTexture = Cast<CDepthTexture>(_pTexture);

    if (pDepthTexture->pParent->TextureData.CurrentLayout != kImageLayout)
    {
      pDepthTexture->pParent->TextureData.SetImageLayout(_CommandBuffer, kImageLayout, { vk::ImageAspectFlagBits::eDepth, 0, 1, 0, 1 });
    }
    pDepthTexture->Descriptor.imageLayout = kImageLayout;
    return pDepthTexture->Descriptor;
  }
  case TextureType::eStencilTexture:
  {
    CStencilTexture* pStencilTexture = Cast<CStencilTexture>(_pTexture);

    if (pStencilTexture->pParent->TextureData.CurrentLayout != kImageLayout)
    {
      pStencilTexture->pParent->TextureData.SetImageLayout(_CommandBuffer, kImageLayout, { vk::ImageAspectFlagBits::eStencil, 0, 1, 0, 1 });
    }
    pStencilTexture->Descriptor.imageLayout = kImageLayout;
    return pStencilTexture->Descriptor;
  }
  default:
  {
    CTexture* pColorTexture = Cast<CTexture>(_pTexture);

    if (pColorTexture->TextureData.CurrentLayout != kImageLayout)
    {
      pColorTexture->TextureData.SetImageLayout(_CommandBuffer, kImageLayout, { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 });
    }
    pColorTexture->Descriptor.imageLayout = kImageLayout;
    return pColorTexture->Descriptor;
  }
  }
}

const vk::DescriptorImageInfo& CDeviceContext::GetSamplerDescriptor(const vdl::Sampler& _Sampler)
{
  if (Samplers_.find(_Sampler) == Samplers_.end())
  {
    Sampler Sampler;

    vk::SamplerCreateInfo SamplerInfo;
    {
      SamplerInfo.magFilter = GetMag(_Sampler.Filter);
      SamplerInfo.minFilter = GetMin(_Sampler.Filter);
      SamplerInfo.mipmapMode = GetMipmap(_Sampler.Filter);
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

    Sampler.VkSampler = VkDevice_.createSamplerUnique(SamplerInfo);
    assert(Sampler.VkSampler);

    Sampler.Descriptor.sampler = Sampler.VkSampler.get();
    Sampler.Descriptor.imageLayout = vk::ImageLayout::eUndefined;

    Samplers_.insert(std::make_pair(_Sampler, std::move(Sampler)));
  }

  return Samplers_.at(_Sampler).Descriptor;
}

const vk::DescriptorBufferInfo& CDeviceContext::GetConstantBufferDescriptor(const vdl::Detail::ConstantBufferData& _ConstantBuffer)
{
  assert(!_ConstantBuffer.isEmpty());

  CCopyConstantBuffer* pConstantBuffer = Cast<CCopyConstantBuffer>(pBufferManager_->GetBuffer(_ConstantBuffer.GetID()));

  return pConstantBuffer->Descriptor;
}

const vk::DescriptorImageInfo& CDeviceContext::GetUnorderedAccessTextureDescriptor(const vdl::UnorderedAccessTexture& _UnorderedAccessTexture, const vk::CommandBuffer& _CommandBuffer)
{
  assert(!_UnorderedAccessTexture.isEmpty());

  constexpr vk::ImageLayout kImageLayout = vk::ImageLayout::eGeneral;

  CTexture* pTexture = Cast<CTexture>(pTextureManager_->GetTexture(_UnorderedAccessTexture.GetID()));
  if (pTexture->TextureData.CurrentLayout != kImageLayout)
  {
    pTexture->TextureData.SetImageLayout(_CommandBuffer, kImageLayout, { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 });
    pTexture->Descriptor.imageLayout = kImageLayout;
  }

  return pTexture->Descriptor;
}

const vk::DescriptorBufferInfo& CDeviceContext::GetUnorderedAccessBufferDescriptor(const vdl::Detail::UnorderedAccessBufferData& _UnorderedAccessBuffer)
{
  assert(!_UnorderedAccessBuffer.isEmpty());

  CUnordererdAccessBuffer* pUnorderedAccessBuffer = Cast<CUnordererdAccessBuffer>(pBufferManager_->GetBuffer(_UnorderedAccessBuffer.GetID()));

  return pUnorderedAccessBuffer->Descriptor;
}
