#include "CDeviceContext.hpp"

#include <vdl/Engine.hpp>
#include <vdl/Device/DirectX12/CDevice.hpp>
#include <vdl/SwapChain/DirectX12/CSwapChain.hpp>
#include <vdl/TextureManager/ITextureManager.hpp>
#include <vdl/BufferManager/IBufferManager.hpp>
#include <vdl/ShaderManager/IShaderManager.hpp>

#include <vdl/Topology/DirectX/Topology.hpp>
#include <vdl/Scissor/DirectX/Scissor.hpp>
#include <vdl/Viewport/DirectX12/Viewport.hpp>
#include <vdl/Buffer/DirectX12/CBuffer.hpp>
#include <vdl/Texture/DirectX12/CTexture.hpp>
#include <vdl/Shader/DirectX12/CShader.hpp>
#include <vdl/Misc/Windows/Misc.hpp>

#include <ThirdParty/ImGui/imgui.h>

#include <vdl/Vertex.hpp>

namespace
{
  inline D3D12_BLEND Cast(vdl::BlendType _Type)
  {
    switch (_Type)
    {
    case vdl::BlendType::eZero:
      return D3D12_BLEND_ZERO;
    case vdl::BlendType::eOne:
      return D3D12_BLEND_ONE;
    case vdl::BlendType::eSrcColor:
      return D3D12_BLEND_SRC_COLOR;
    case vdl::BlendType::eInvSrcColor:
      return D3D12_BLEND_INV_SRC_COLOR;
    case vdl::BlendType::eSrcAlpha:
      return D3D12_BLEND_SRC_ALPHA;
    case vdl::BlendType::eInvSrcAlpha:
      return D3D12_BLEND_INV_SRC_ALPHA;
    case vdl::BlendType::eDestAlpha:
      return D3D12_BLEND_DEST_ALPHA;
    case vdl::BlendType::eInvDestAlpha:
      return D3D12_BLEND_INV_DEST_ALPHA;
    case vdl::BlendType::eDestColor:
      return D3D12_BLEND_DEST_COLOR;
    case vdl::BlendType::eInvDestColor:
      return D3D12_BLEND_INV_DEST_COLOR;
    case vdl::BlendType::eSrcAlphaSat:
      return D3D12_BLEND_SRC_ALPHA_SAT;
    case vdl::BlendType::eSrc1Color:
      return D3D12_BLEND_SRC1_COLOR;
    case vdl::BlendType::eInvSrc1Color:
      return D3D12_BLEND_INV_SRC1_COLOR;
    case vdl::BlendType::eSrc1Alpha:
      return D3D12_BLEND_SRC1_ALPHA;
    case vdl::BlendType::eInvSrc1Alpha:
      return D3D12_BLEND_INV_SRC1_ALPHA;
    default: assert(false);
    }

    return D3D12_BLEND_ZERO;
  }

  inline D3D12_BLEND_OP Cast(vdl::BlendOpType _Type)
  {
    switch (_Type)
    {
    case vdl::BlendOpType::eAdd:
      return D3D12_BLEND_OP_ADD;
    case vdl::BlendOpType::eSubtract:
      return D3D12_BLEND_OP_SUBTRACT;
    case vdl::BlendOpType::eRevSubtract:
      return D3D12_BLEND_OP_REV_SUBTRACT;
    case vdl::BlendOpType::eMin:
      return D3D12_BLEND_OP_MIN;
    case vdl::BlendOpType::eMax:
      return D3D12_BLEND_OP_MAX;
    default: assert(false);
    }

    return D3D12_BLEND_OP_ADD;
  }

  inline D3D12_COLOR_WRITE_ENABLE Cast(vdl::ColorWriteEnableType _Type)
  {
    switch (_Type)
    {
    case vdl::ColorWriteEnableType::eRed:
      return D3D12_COLOR_WRITE_ENABLE_RED;
    case vdl::ColorWriteEnableType::eGreen:
      return D3D12_COLOR_WRITE_ENABLE_GREEN;
    case vdl::ColorWriteEnableType::eBlue:
      return D3D12_COLOR_WRITE_ENABLE_BLUE;
    case vdl::ColorWriteEnableType::eAlpha:
      return D3D12_COLOR_WRITE_ENABLE_ALPHA;
    case vdl::ColorWriteEnableType::eAll:
      return D3D12_COLOR_WRITE_ENABLE_ALL;
    default: assert(false);
    }

    return D3D12_COLOR_WRITE_ENABLE_RED;
  }

  inline D3D12_DEPTH_WRITE_MASK Cast(vdl::DepthWriteMaskType _Type)
  {
    switch (_Type)
    {
    case vdl::DepthWriteMaskType::eZero:
      return D3D12_DEPTH_WRITE_MASK_ZERO;
    case vdl::DepthWriteMaskType::eAll:
      return D3D12_DEPTH_WRITE_MASK_ALL;
    default: assert(false);
    }

    return D3D12_DEPTH_WRITE_MASK_ZERO;
  }

  inline D3D12_COMPARISON_FUNC Cast(vdl::ComparisonFuncType _Type)
  {
    switch (_Type)
    {
    case vdl::ComparisonFuncType::eNever:
      return D3D12_COMPARISON_FUNC_NEVER;
    case vdl::ComparisonFuncType::eLess:
      return D3D12_COMPARISON_FUNC_LESS;
    case vdl::ComparisonFuncType::eEqual:
      return D3D12_COMPARISON_FUNC_EQUAL;
    case vdl::ComparisonFuncType::eLessEqual:
      return D3D12_COMPARISON_FUNC_LESS_EQUAL;
    case vdl::ComparisonFuncType::eGreater:
      return D3D12_COMPARISON_FUNC_GREATER;
    case vdl::ComparisonFuncType::eNotEqual:
      return D3D12_COMPARISON_FUNC_NOT_EQUAL;
    case vdl::ComparisonFuncType::eGreaterEqual:
      return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
    case vdl::ComparisonFuncType::eAlways:
      return D3D12_COMPARISON_FUNC_ALWAYS;
    default: assert(false);
    }

    return D3D12_COMPARISON_FUNC_NEVER;
  }

  inline D3D12_STENCIL_OP Cast(vdl::StencilOpType _Type)
  {
    switch (_Type)
    {
    case vdl::StencilOpType::eKeep:
      return D3D12_STENCIL_OP_KEEP;
    case vdl::StencilOpType::eZero:
      return D3D12_STENCIL_OP_ZERO;
    case vdl::StencilOpType::eRreplace:
      return D3D12_STENCIL_OP_REPLACE;
    case vdl::StencilOpType::eIncrSat:
      return D3D12_STENCIL_OP_INCR_SAT;
    case vdl::StencilOpType::eDecrSat:
      return D3D12_STENCIL_OP_DECR_SAT;
    case vdl::StencilOpType::eInvert:
      return D3D12_STENCIL_OP_INVERT;
    case vdl::StencilOpType::eIncr:
      return D3D12_STENCIL_OP_INCR;
    case vdl::StencilOpType::eDecr:
      return D3D12_STENCIL_OP_DECR;
    default: assert(false);
    }

    return D3D12_STENCIL_OP_KEEP;
  }

  inline D3D12_CULL_MODE Cast(vdl::CullModeType _Type)
  {
    switch (_Type)
    {
    case vdl::CullModeType::eNone:
      return D3D12_CULL_MODE_NONE;
    case vdl::CullModeType::eFront:
      return D3D12_CULL_MODE_FRONT;
    case vdl::CullModeType::eBack:
      return D3D12_CULL_MODE_BACK;
    default: assert(false);
    }

    return D3D12_CULL_MODE_NONE;
  }

  inline D3D12_FILL_MODE Cast(vdl::FillModeType _Type)
  {
    switch (_Type)
    {
    case vdl::FillModeType::eWireframe:
      return D3D12_FILL_MODE_WIREFRAME;
    case vdl::FillModeType::eSolid:
      return D3D12_FILL_MODE_SOLID;
    default: assert(false);
    }

    return D3D12_FILL_MODE_WIREFRAME;
  }

  inline D3D12_FILTER Cast(vdl::FilterType _Type)
  {
    switch (_Type)
    {
    case vdl::FilterType::eMinMagMipPoint:
      return D3D12_FILTER_MIN_MAG_MIP_POINT;
    case vdl::FilterType::eMinMagPointMipLinear:
      return D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR;
    case vdl::FilterType::eMinPointMagLinearMipPoint:
      return D3D12_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
    case vdl::FilterType::eMinPointMagMipLinear:
      return D3D12_FILTER_MIN_POINT_MAG_MIP_LINEAR;
    case vdl::FilterType::eMinLinearMagMipPoint:
      return D3D12_FILTER_MIN_LINEAR_MAG_MIP_POINT;
    case vdl::FilterType::eMinLinearMagPointMipLinear:
      return D3D12_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
    case vdl::FilterType::eMinMagLinearMipPoint:
      return D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
    case vdl::FilterType::eMinMagMipLinear:
      return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    case vdl::FilterType::eAnisotropic:
      return D3D12_FILTER_ANISOTROPIC;
    case vdl::FilterType::eComparisonMinMagMipPoint:
      return D3D12_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
    case vdl::FilterType::eComparisonMinMagPointMipLinear:
      return D3D12_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR;
    case vdl::FilterType::eComparisonMinPointMagLinearMipPoint:
      return D3D12_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT;
    case vdl::FilterType::eComparisonMinPointMagMipLinear:
      return D3D12_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR;
    case vdl::FilterType::eComparisonMinLinearMagMipPoint:
      return D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT;
    case vdl::FilterType::eComparisonMinLinearMagPointMipLinear:
      return D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
    case vdl::FilterType::eComparisonMinMagLinearMipPoint:
      return D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
    case vdl::FilterType::eComparisonMinMagMipLinear:
      return D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
    case vdl::FilterType::eComparisonAnisotropic:
      return D3D12_FILTER_COMPARISON_ANISOTROPIC;
    default: assert(false);
    }

    return D3D12_FILTER_MIN_MAG_MIP_POINT;
  }

  inline D3D12_TEXTURE_ADDRESS_MODE Cast(vdl::AddressModeType _Type)
  {
    switch (_Type)
    {
    case vdl::AddressModeType::eWrap:
      return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    case vdl::AddressModeType::eMirror:
      return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
    case vdl::AddressModeType::eClamp:
      return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    case vdl::AddressModeType::eBorder:
      return D3D12_TEXTURE_ADDRESS_MODE_BORDER;
    case vdl::AddressModeType::eMirrorOnce:
      return D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE;
    default: assert(false);
    }

    return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
  }

  inline vdl::Color4F Cast(vdl::BorderColorType _Type)
  {
    switch (_Type)
    {
    case vdl::BorderColorType::eTransparent:
      return vdl::Color4F(0.0f, 0.0f, 0.0f, 0.0f);
    case vdl::BorderColorType::eBlack:
      return vdl::Color4F(0.0f, 0.0f, 0.0f, 1.0f);
    case vdl::BorderColorType::eWhite:
      return vdl::Color4F(1.0f, 1.0f, 1.0f, 1.0f);
    default: assert(false);
    }

    return vdl::Color4F();
  }

  inline D3D12_SHADER_VISIBILITY GetShaderVisibility(ShaderType _Type)
  {
    switch (_Type)
    {
    case ShaderType::eVertexShader:
      return D3D12_SHADER_VISIBILITY_VERTEX;
    case ShaderType::eHullShader:
      return D3D12_SHADER_VISIBILITY_HULL;
    case ShaderType::eDomainShader:
      return D3D12_SHADER_VISIBILITY_DOMAIN;
    case ShaderType::eGeometryShader:
      return D3D12_SHADER_VISIBILITY_GEOMETRY;
    case ShaderType::ePixelShader:
      return D3D12_SHADER_VISIBILITY_PIXEL;
    case ShaderType::eComputeShader:
      return D3D12_SHADER_VISIBILITY_ALL;
    default: assert(false);
    }

    return D3D12_SHADER_VISIBILITY_ALL;
  }

  inline D3D12_PRIMITIVE_TOPOLOGY_TYPE GetTopologyType(vdl::TopologyType _Type)
  {
    switch (_Type)
    {
    case vdl::TopologyType::ePointList:
      return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
    case vdl::TopologyType::eLineList:
    case vdl::TopologyType::eLineStrip:
      return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
    case vdl::TopologyType::eTriangleList:
    case vdl::TopologyType::eTriangleStrip:
      return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    case vdl::TopologyType::ePatchList1ControlPoint:
    case vdl::TopologyType::ePatchList2ControlPoint:
    case vdl::TopologyType::ePatchList3ControlPoint:
    case vdl::TopologyType::ePatchList4ControlPoint:
    case vdl::TopologyType::ePatchList5ControlPoint:
    case vdl::TopologyType::ePatchList6ControlPoint:
    case vdl::TopologyType::ePatchList7ControlPoint:
    case vdl::TopologyType::ePatchList8ControlPoint:
    case vdl::TopologyType::ePatchList9ControlPoint:
    case vdl::TopologyType::ePatchList10ControlPoint:
    case vdl::TopologyType::ePatchList11ControlPoint:
    case vdl::TopologyType::ePatchList12ControlPoint:
    case vdl::TopologyType::ePatchList13ControlPoint:
    case vdl::TopologyType::ePatchList14ControlPoint:
    case vdl::TopologyType::ePatchList15ControlPoint:
    case vdl::TopologyType::ePatchList16ControlPoint:
    case vdl::TopologyType::ePatchList17ControlPoint:
    case vdl::TopologyType::ePatchList18ControlPoint:
    case vdl::TopologyType::ePatchList19ControlPoint:
    case vdl::TopologyType::ePatchList20ControlPoint:
    case vdl::TopologyType::ePatchList21ControlPoint:
    case vdl::TopologyType::ePatchList22ControlPoint:
    case vdl::TopologyType::ePatchList23ControlPoint:
    case vdl::TopologyType::ePatchList24ControlPoint:
    case vdl::TopologyType::ePatchList25ControlPoint:
    case vdl::TopologyType::ePatchList26ControlPoint:
    case vdl::TopologyType::ePatchList27ControlPoint:
    case vdl::TopologyType::ePatchList28ControlPoint:
    case vdl::TopologyType::ePatchList29ControlPoint:
    case vdl::TopologyType::ePatchList30ControlPoint:
    case vdl::TopologyType::ePatchList31ControlPoint:
    case vdl::TopologyType::ePatchList32ControlPoint:
      return D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
    default: assert(false);
    }

    return D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
  }
}

void CDeviceContext::Initialize()
{
  pDevice_ = Cast<CDevice>(Engine::Get<IDevice>());
  pSwapChain_ = Cast<CSwapChain>(Engine::Get<ISwapChain>());

  pTextureManager_ = Engine::Get<ITextureManager>();
  pBufferManager_ = Engine::Get<IBufferManager>();
  pShaderManager_ = Engine::Get<IShaderManager>();

  pD3D12Device_ = pDevice_->GetDevice();
  pDXGISwapChain_ = pDevice_->GetSwapChain();
  pGraphicsCommandQueue_ = pDevice_->GetCommandQueueManager()->GetGraphicsQueue();
  pComputeCommandQueue_ = pDevice_->GetCommandQueueManager()->GetComputeQueue();
  pSamplerDescriptorAllocator_ = pDevice_->GetDescriptorAllocator(DescriptorHeapType::eSampler);

  //  ���̓��C�A�E�g�̍쐬
  {
    //  None
    {
      std::vector<D3D12_INPUT_ELEMENT_DESC> InputElementDesc;

      InputLayouts_.insert(std::make_pair(vdl::InputLayoutType::eNone, std::move(InputElementDesc)));
    }

    //  Texture
    {
      std::vector<D3D12_INPUT_ELEMENT_DESC> InputElementDesc;
      {
        InputElementDesc.push_back({ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
        InputElementDesc.push_back({ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });

        InputElementDesc.push_back({ "NDC_TRANSFORM", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 });
        InputElementDesc.push_back({ "NDC_TRANSFORM", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 });
        InputElementDesc.push_back({ "NDC_TRANSFORM", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 });
        InputElementDesc.push_back({ "NDC_TRANSFORM", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 });
        InputElementDesc.push_back({ "TEXCOORD_SCALE", 0, DXGI_FORMAT_R32G32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 });
        InputElementDesc.push_back({ "TEXCOORD_TRANSLATE", 0, DXGI_FORMAT_R32G32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 });
        InputElementDesc.push_back({ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 });
      }

      InputLayouts_.insert(std::make_pair(vdl::InputLayoutType::eTexture, std::move(InputElementDesc)));
    }

    //  StaticMesh
    {
      std::vector<D3D12_INPUT_ELEMENT_DESC> InputElementDesc;
      {
        InputElementDesc.push_back({ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
        InputElementDesc.push_back({ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
        InputElementDesc.push_back({ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
        InputElementDesc.push_back({ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });

        InputElementDesc.push_back({ "WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 });
        InputElementDesc.push_back({ "WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 });
        InputElementDesc.push_back({ "WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 });
        InputElementDesc.push_back({ "WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 });
        InputElementDesc.push_back({ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 });
      }

      InputLayouts_.insert(std::make_pair(vdl::InputLayoutType::eStaticMesh, std::move(InputElementDesc)));
    }

    //  SkinnedMesh
    {
      std::vector<D3D12_INPUT_ELEMENT_DESC> InputElementDesc;
      {
        InputElementDesc.push_back({ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
        InputElementDesc.push_back({ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
        InputElementDesc.push_back({ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
        InputElementDesc.push_back({ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
        InputElementDesc.push_back({ "WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
        InputElementDesc.push_back({ "BONES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });

        InputElementDesc.push_back({ "WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 });
        InputElementDesc.push_back({ "WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 });
        InputElementDesc.push_back({ "WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 });
        InputElementDesc.push_back({ "WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 });
        InputElementDesc.push_back({ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 });
      }

      InputLayouts_.insert(std::make_pair(vdl::InputLayoutType::eSkinnedMesh, std::move(InputElementDesc)));
    }

    //  GUI
    {
      std::vector<D3D12_INPUT_ELEMENT_DESC> InputElementDesc;
      {
        InputElementDesc.push_back({ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,   0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
        InputElementDesc.push_back({ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,   0, D3D12_APPEND_ALIGNED_ELEMENT,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
        InputElementDesc.push_back({ "COLOR",    0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
      }

      InputLayouts_.insert(std::make_pair(vdl::InputLayoutType::eGUI, std::move(InputElementDesc)));
    }
  }

  HRESULT hr = S_OK;

  //  �f�B�X�N���v�^�q�[�v�̍쐬
  {
    for (vdl::uint i = 0; i < kDescriptorHeapNum; ++i)
    {
      DescriptorAllocators_[i].Initialize(pD3D12Device_, Cast(kDescriptorHeapTypes[i]), D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
      ppDescriptorHeaps_[i] = DescriptorAllocators_[i].GetDescriptorHeap();
    }

    D3D12_DESCRIPTOR_HEAP_DESC DescriptorHeapDesc;
    {
      DescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
      DescriptorHeapDesc.NumDescriptors = 1;
      DescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
      DescriptorHeapDesc.NodeMask = 0;
    }
    hr = pD3D12Device_->CreateDescriptorHeap(&DescriptorHeapDesc, IID_PPV_ARGS(pDefaultViewDescriptorHeap_.GetAddressOf()));
    ERROR_CHECK(hr);

    DefaultViewCPUHandle_ = pDefaultViewDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();
  }

  //  �t�F���X�C�x���g�̍쐬
  {
    FenceEvent_ = ::CreateEvent(nullptr, false, false, nullptr);
    assert(FenceEvent_ != nullptr);
  }

  constexpr D3D12_DESCRIPTOR_RANGE kDescriptorRanges[] = {
    { D3D12_DESCRIPTOR_RANGE_TYPE_SRV, Constants::kMaxShaderResourceNum, 0, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND },
    { D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, Constants::kMaxSamplerNum, 0, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND },
    { D3D12_DESCRIPTOR_RANGE_TYPE_CBV, Constants::kMaxConstantBufferNum, 0, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND },
    { D3D12_DESCRIPTOR_RANGE_TYPE_UAV, Constants::kMaxUnorderedAccessObjectNum, 0, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND },
  };

  auto GetDescriptorRange = [&kDescriptorRanges](DescriptorType _Type)->const D3D12_DESCRIPTOR_RANGE*
  {
    return &kDescriptorRanges[static_cast<vdl::uint>(_Type)];
  };

  //  �O���t�B�b�N�X�p�̃I�u�W�F�N�g�̍쐬
  {
    //  ���[�g�V�O�l�`���̍쐬
    {
      D3D12_ROOT_PARAMETER RootParameters[kGraphicsDescriptorNum];
      for (vdl::uint i = 0; i < kGraphicsDescriptorTypeNum; ++i)
      {
        for (vdl::uint j = 0; j < kGraphicsShaderStageNum; ++j)
        {
          D3D12_ROOT_PARAMETER& RootParameter = RootParameters[GetDescriptorOffset(static_cast<ShaderType>(j), static_cast<DescriptorType>(i))];
          RootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
          RootParameter.DescriptorTable.NumDescriptorRanges = 1;
          RootParameter.DescriptorTable.pDescriptorRanges = GetDescriptorRange(static_cast<DescriptorType>(i));
          RootParameter.ShaderVisibility = GetShaderVisibility(static_cast<ShaderType>(j));
        }
      }

      D3D12_ROOT_SIGNATURE_DESC RootSignatureDesc;
      {
        RootSignatureDesc.NumParameters = kGraphicsDescriptorNum;
        RootSignatureDesc.pParameters = RootParameters;
        RootSignatureDesc.NumStaticSamplers = 0;
        RootSignatureDesc.pStaticSamplers = nullptr;
        RootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
      }

      Microsoft::WRL::ComPtr<ID3DBlob> pSignature;
      Microsoft::WRL::ComPtr<ID3DBlob> pError;
      hr = ::D3D12SerializeRootSignature(&RootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, pSignature.GetAddressOf(), pError.GetAddressOf());
      _ASSERT_EXPR_A(SUCCEEDED(hr), pError->GetBufferPointer());

      hr = pD3D12Device_->CreateRootSignature(0, pSignature->GetBufferPointer(), pSignature->GetBufferSize(), IID_PPV_ARGS(pGraphicsRootSignature_.GetAddressOf()));
      ERROR_CHECK(hr);
    }

    //  �R�}���h���X�g�̍쐬
    {
      for (vdl::uint i = 0; i < Constants::kGraphicsCommandBufferNum; ++i)
      {
        GraphicsCommandLists_[i].Initialize(pD3D12Device_, D3D12_COMMAND_LIST_TYPE_DIRECT);
      }

      CommandList& CurrentGraphicsCommandList = GetCurrentGraphicsCommandList();
      CurrentGraphicsCommandList.Reset();
      CurrentGraphicsCommandList->SetGraphicsRootSignature(pGraphicsRootSignature_.Get());
      CurrentGraphicsCommandList->SetDescriptorHeaps(kDescriptorHeapNum, ppDescriptorHeaps_.data());
    }

    //  �t�F���X�̏�����
    for (auto& GraphicsFence : GraphicsFences_)
    {
      GraphicsFence.Initialize(pD3D12Device_);
    }

    //  �ۑ��f�[�^�̏�����
    {
      GraphicsReserveData& GraphicsReserveData = GetCurrentGraphicsReserveData();
      GraphicsReserveData.ShaderResourceHeaps.resize(1);
      GraphicsReserveData.SamplerHeaps.resize(1);
      GraphicsReserveData.ConstantBufferHeaps.resize(1);
      GraphicsReserveData.PipelineStates.resize(1);
    }
  }

  //  �R���s���[�g�p�̃I�u�W�F�N�g�̍쐬
  {
    //  ���[�g�V�O�l�`���̍쐬
    {
      D3D12_ROOT_PARAMETER RootParameters[kComputeDescriptorTypeNum];
      for (vdl::uint i = 0; i < kComputeDescriptorTypeNum; ++i)
      {
        D3D12_ROOT_PARAMETER& RootParameter = RootParameters[i];
        RootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        RootParameter.DescriptorTable.NumDescriptorRanges = 1;
        RootParameter.DescriptorTable.pDescriptorRanges = GetDescriptorRange(static_cast<DescriptorType>(i));
        RootParameter.ShaderVisibility = GetShaderVisibility(ShaderType::eComputeShader);
      }

      D3D12_ROOT_SIGNATURE_DESC RootSignatureDesc;
      {
        RootSignatureDesc.NumParameters = kComputeDescriptorTypeNum;
        RootSignatureDesc.pParameters = RootParameters;
        RootSignatureDesc.NumStaticSamplers = 0;
        RootSignatureDesc.pStaticSamplers = nullptr;
        RootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;
      }

      Microsoft::WRL::ComPtr<ID3DBlob> pSignature;
      Microsoft::WRL::ComPtr<ID3DBlob> pError;
      hr = ::D3D12SerializeRootSignature(&RootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, pSignature.GetAddressOf(), pError.GetAddressOf());
      _ASSERT_EXPR_A(SUCCEEDED(hr), pError->GetBufferPointer());

      hr = pD3D12Device_->CreateRootSignature(0, pSignature->GetBufferPointer(), pSignature->GetBufferSize(), IID_PPV_ARGS(pComputeRootSignature_.GetAddressOf()));
      ERROR_CHECK(hr);
    }

    //  �R�}���h���X�g�̍쐬
    {
      for (vdl::uint i = 0; i < Constants::kComputeCommandBufferNum; ++i)
      {
        ComputeCommandLists_[i].Initialize(pD3D12Device_, D3D12_COMMAND_LIST_TYPE_COMPUTE);
      }

      GetCurrentComputeReserveData().PipelineStates.resize(1);
    }

    //  �t�F���X�̏�����
    for (auto& ComputeFence : ComputeFences_)
    {
      ComputeFence.Initialize(pD3D12Device_);
    }

    //  �ۑ��f�[�^�̏�����
    {
      ComputeReserveData& ComputeReserveData = GetCurrentComputeReserveData();
      ComputeReserveData.ShaderResourceHeaps.resize(1);
      ComputeReserveData.SamplerHeaps.resize(1);
      ComputeReserveData.ConstantBufferHeaps.resize(1);
      ComputeReserveData.UnorderedAccessHeaps.resize(1);
      ComputeReserveData.PipelineStates.resize(1);
    }
  }
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
  vdl::OutputManager OutputManager = { _RenderTextures, _DepthStenilTexture };

  std::vector<vdl::OutputManager>& OutputManagers = GraphicsReserveDatas_[GraphicsCommandBufferIndex_].OutputManagers;

  if (OutputManagers.empty() || OutputManagers.back() != OutputManager)
  {
    OutputManagers.emplace_back(std::move(OutputManager));
    GraphicsStateChangeFlags_.Set(GraphicsCommandType::eSetRenderTextures);
  }
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

void CDeviceContext::ClearRenderTexture(const vdl::RenderTexture& _RenderTexture, const vdl::Color4F& _ClearColor)
{
  assert(!_RenderTexture.isEmpty());

  constexpr D3D12_RESOURCE_STATES kResourceState = D3D12_RESOURCE_STATE_RENDER_TARGET;

  CommandList& CurrentGraphicsCommandList = GetCurrentGraphicsCommandList();

  CRenderTexture* pRenderTexture = GetD3D12RenderTexture(_RenderTexture);
  if (pRenderTexture->TextureData.ResourceState != kResourceState)
  {
    pRenderTexture->TextureData.TransitionResourceBarrier(&CurrentGraphicsCommandList, kResourceState);
  }
  CurrentGraphicsCommandList->ClearRenderTargetView(pRenderTexture->RenderTargetViewHeap.CPUHandle, &_ClearColor.Red, 0, nullptr);

  //  Clear����܂Ő�����ۏ�
  {
    std::unordered_map<vdl::ID, Texture>& ClearTextures = GraphicsReserveDatas_[GraphicsCommandBufferIndex_].ClearTextures;

    const vdl::ID TextureID = _RenderTexture.GetID();
    if (ClearTextures.find(TextureID) == ClearTextures.end())
    {
      ClearTextures.insert(std::make_pair(TextureID, _RenderTexture));
    }
  }
}

void CDeviceContext::ClearDepthStencilTexture(const vdl::DepthStencilTexture& _DepthStencilTexture, float _ClearDepth, vdl::uint _ClearStencil)
{
  assert(!_DepthStencilTexture.isEmpty());

  constexpr D3D12_RESOURCE_STATES kResourceState = D3D12_RESOURCE_STATE_DEPTH_WRITE;

  CommandList& CurrentGraphicsCommandList = GetCurrentGraphicsCommandList();

  const vdl::ID TextureID = _DepthStencilTexture.GetID();
  CDepthStencilTexture* pDepthStencilTexture = Cast<CDepthStencilTexture>(pTextureManager_->GetTexture(TextureID));
  if (pDepthStencilTexture->TextureData.ResourceState != kResourceState)
  {
    pDepthStencilTexture->TextureData.TransitionResourceBarrier(&CurrentGraphicsCommandList, kResourceState);
  }
  CurrentGraphicsCommandList->ClearDepthStencilView(pDepthStencilTexture->DepthStencilViewHeap.CPUHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, _ClearDepth, _ClearStencil, 0, nullptr);

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

  constexpr D3D12_RESOURCE_STATES kResourceState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;

  CommandList& CurrentGraphicsCommandList = GetCurrentGraphicsCommandList();

  const vdl::ID TextureID = _UnorderedAccessTexture.GetID();
  CUnorderedAccessTexture* pUnorderedAccessTexture = Cast<CUnorderedAccessTexture>(pTextureManager_->GetTexture(TextureID));
  if (pUnorderedAccessTexture->TextureData.ResourceState != kResourceState)
  {
    pUnorderedAccessTexture->TextureData.TransitionResourceBarrier(&CurrentGraphicsCommandList, kResourceState);
  }
  CurrentGraphicsCommandList->ClearUnorderedAccessViewFloat(pUnorderedAccessTexture->UnorderedAccessViewHeap.GPUHandle,
    pUnorderedAccessTexture->UnorderedAccessViewHeap.CPUHandle, pUnorderedAccessTexture->TextureData.pResource.Get(), &_ClearColor.Red, 0, nullptr);

  //  Clear����܂Ő�����ۏ�
  {
    std::unordered_map<vdl::ID, Texture>& ClearTextures = GraphicsReserveDatas_[GraphicsCommandBufferIndex_].ClearTextures;

    if (ClearTextures.find(TextureID) == ClearTextures.end())
    {
      ClearTextures.insert(std::make_pair(TextureID, _UnorderedAccessTexture));
    }
  }
}

void CDeviceContext::Draw(vdl::uint _VertexCount, vdl::uint _InstanceCount, vdl::uint _FirstVertex, vdl::uint _FirstInstance)
{
  PreprocessingDraw();

  GetCurrentGraphicsCommandList()->DrawInstanced(_VertexCount, _InstanceCount, _FirstVertex, _FirstInstance);
}

void CDeviceContext::DrawIndexed(vdl::uint _IndexCount, vdl::uint _InstanceCount, vdl::uint _FirstIndex, vdl::uint _VertexOffset, vdl::uint _FirstInstance)
{
  PreprocessingDraw();

  GetCurrentGraphicsCommandList()->DrawIndexedInstanced(_IndexCount, _InstanceCount, _FirstIndex, _VertexOffset, _FirstInstance);
}

void CDeviceContext::Dispatch(vdl::uint _ThreadGroupX, vdl::uint _ThreadGroupY, vdl::uint _ThreadGroupZ)
{
  HRESULT hr = S_OK;

  ComputeReserveData& CurrentComputeReserveData = GetCurrentComputeReserveData();

  //  �R�}���h���X�g�̏����ݒ�
  CommandList& CurrentComputeCommandList = GetCurrentComputeCommandList();
  {
    CurrentComputeCommandList.Reset(CurrentComputeReserveData.PipelineStates[0].Get());
    CurrentComputeCommandList->SetComputeRootSignature(pComputeRootSignature_.Get());
    CurrentComputeCommandList->SetDescriptorHeaps(kDescriptorHeapNum, ppDescriptorHeaps_.data());
  }

  //  SetDescriptor
  {
    CommandList& CurrentGraphicsCommandList = GetCurrentGraphicsCommandList();

    //  SetShaderResource
    {
      const vdl::uint ShaderResourceNum = static_cast<vdl::uint>(CurrentComputeState_.ShaderResources.size());

      if (ShaderResourceNum)
      {
        if (ComputeStateChangeFlags_.Has(ComputeCommandType::eSetShaderResource))
        {
          //  �f�[�^�̓ǂݍ���
          std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> ShaderResourceDatas(ShaderResourceNum);
          {
            for (vdl::uint ShaderResourceCount = 0; ShaderResourceCount < ShaderResourceNum; ++ShaderResourceCount)
            {
              ShaderResourceDatas[ShaderResourceCount] = GetShaderResourceCPUHandle(CurrentComputeState_.ShaderResources[ShaderResourceCount], &CurrentGraphicsCommandList);
            }
          }

          constexpr DescriptorHeapType kDescriptorHeapType = DescriptorHeapType::eCBV_SRV_UAV;
          DescriptorAllocator& DescriptorAllocator = DescriptorAllocators_[static_cast<vdl::uint>(kDescriptorHeapType)];

          DescriptorHeap& DescriptorHeap = CurrentComputeReserveData.ShaderResourceHeaps.emplace_back();
          DescriptorAllocator.Allocate(&DescriptorHeap, ShaderResourceNum);

          pD3D12Device_->CopyDescriptors(1, &DescriptorHeap.CPUHandle, &ShaderResourceNum,
            ShaderResourceNum, ShaderResourceDatas.data(), nullptr, Cast(kDescriptorHeapType));

          CurrentComputeReserveData.ShaderResources = CurrentComputeState_.ShaderResources;
        }

        constexpr DescriptorType kDescriptorType = DescriptorType::eShaderResource;
        CurrentComputeCommandList->SetComputeRootDescriptorTable(GetDescriptorOffset(ShaderType::eComputeShader, kDescriptorType), CurrentComputeReserveData.ShaderResourceHeaps.back().GPUHandle);
      }
    }

    //  SetSampler
    {
      const vdl::uint SamplerNum = static_cast<vdl::uint>(CurrentComputeState_.Samplers.size());

      if (SamplerNum)
      {
        if (ComputeStateChangeFlags_.Has(ComputeCommandType::eSetSampler))
        {
          //  �f�[�^�̓ǂݍ���
          std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> SamplerDatas(SamplerNum);
          {
            for (vdl::uint SamplerCount = 0; SamplerCount < SamplerNum; ++SamplerCount)
            {
              SamplerDatas[SamplerCount] = GetSamplerCPUHandle(CurrentComputeState_.Samplers[SamplerCount]);
            }
          }

          constexpr DescriptorHeapType kDescriptorHeapType = DescriptorHeapType::eSampler;
          DescriptorAllocator& DescriptorAllocator = DescriptorAllocators_[static_cast<vdl::uint>(kDescriptorHeapType)];

          DescriptorHeap& DescriptorHeap = CurrentComputeReserveData.SamplerHeaps.emplace_back();
          DescriptorAllocator.Allocate(&DescriptorHeap, SamplerNum);

          pD3D12Device_->CopyDescriptors(1, &DescriptorHeap.CPUHandle, &SamplerNum,
            SamplerNum, SamplerDatas.data(), nullptr, Cast(kDescriptorHeapType));
        }

        constexpr DescriptorType kDescriptorType = DescriptorType::eSampler;
        CurrentComputeCommandList->SetComputeRootDescriptorTable(GetDescriptorOffset(ShaderType::eComputeShader, kDescriptorType), CurrentComputeReserveData.SamplerHeaps.back().GPUHandle);
      }
    }

    //  SetConstantBuffer
    {
      const vdl::uint ConstantBufferNum = static_cast<vdl::uint>(CurrentComputeState_.ConstantBuffers.size());

      if (ConstantBufferNum)
      {
        if (ComputeStateChangeFlags_.Has(ComputeCommandType::eSetConstantBuffer))
        {
          //  �f�[�^�̓ǂݍ���
          std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> ConstantBufferDatas(ConstantBufferNum);
          {
            for (vdl::uint ConstantBufferCount = 0; ConstantBufferCount < ConstantBufferNum; ++ConstantBufferCount)
            {
              ConstantBufferDatas[ConstantBufferCount] = GetConstantBufferCPUHandle(CurrentComputeState_.ConstantBuffers[ConstantBufferCount]);
            }
          }

          constexpr DescriptorHeapType kDescriptorHeapType = DescriptorHeapType::eCBV_SRV_UAV;
          DescriptorAllocator& DescriptorAllocator = DescriptorAllocators_[static_cast<vdl::uint>(kDescriptorHeapType)];

          DescriptorHeap& DescriptorHeap = CurrentComputeReserveData.ConstantBufferHeaps.emplace_back();
          DescriptorAllocator.Allocate(&DescriptorHeap, ConstantBufferNum);

          pD3D12Device_->CopyDescriptors(1, &DescriptorHeap.CPUHandle, &ConstantBufferNum,
            ConstantBufferNum, ConstantBufferDatas.data(), nullptr, Cast(kDescriptorHeapType));

          CurrentComputeReserveData.ConstantBuffers = CurrentComputeState_.ConstantBuffers;
        }

        constexpr DescriptorType kDescriptorType = DescriptorType::eConstantBuffer;
        CurrentComputeCommandList->SetComputeRootDescriptorTable(GetDescriptorOffset(ShaderType::eComputeShader, kDescriptorType), CurrentComputeReserveData.ConstantBufferHeaps.back().GPUHandle);
      }
    }

    //  SetUnorderedAccessObject
    {
      const vdl::uint UnorderedAccessObjectNum = static_cast<vdl::uint>(CurrentComputeState_.UnorderedAccessObjects.size());

      if (UnorderedAccessObjectNum)
      {
        if (ComputeStateChangeFlags_.Has(ComputeCommandType::eSetUnorderedAccessObject))
        {
          //  �f�[�^�̓ǂݍ���
          std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> UnorderedAccessObjectDatas(UnorderedAccessObjectNum);
          {
            for (vdl::uint UnorderedAccessObjectCount = 0; UnorderedAccessObjectCount < UnorderedAccessObjectNum; ++UnorderedAccessObjectCount)
            {
              UnorderedAccessObjectDatas[UnorderedAccessObjectCount] = GetUnorderedAccessObjectCPUHandle(CurrentComputeState_.UnorderedAccessObjects[UnorderedAccessObjectCount], &CurrentGraphicsCommandList);
            }
          }

          constexpr DescriptorHeapType kDescriptorHeapType = DescriptorHeapType::eCBV_SRV_UAV;
          DescriptorAllocator& DescriptorAllocator = DescriptorAllocators_[static_cast<vdl::uint>(kDescriptorHeapType)];

          DescriptorHeap& DescriptorHeap = CurrentComputeReserveData.UnorderedAccessHeaps.emplace_back();
          DescriptorAllocator.Allocate(&DescriptorHeap, UnorderedAccessObjectNum);

          pD3D12Device_->CopyDescriptors(1, &DescriptorHeap.CPUHandle, &UnorderedAccessObjectNum,
            UnorderedAccessObjectNum, UnorderedAccessObjectDatas.data(), nullptr, Cast(kDescriptorHeapType));

          CurrentComputeReserveData.UnorderedAccessObjects = CurrentComputeState_.UnorderedAccessObjects;
        }

        constexpr DescriptorType kDescriptorType = DescriptorType::eUnorderedAccessObject;
        CurrentComputeCommandList->SetComputeRootDescriptorTable(GetDescriptorOffset(ShaderType::eComputeShader, kDescriptorType), CurrentComputeReserveData.UnorderedAccessHeaps.back().GPUHandle);
      }
    }

    Flush();
  }

  //  SetPipeline
  if (ComputeStateChangeFlags_.Has(ComputeCommandType::eSetComputeShader))
  {
    assert(!CurrentComputeState_.ComputeShader.isEmpty());
    const CComputeShader* pComputeShader = Cast<CComputeShader>(pShaderManager_->GetShader(CurrentComputeState_.ComputeShader.GetID()));

    D3D12_COMPUTE_PIPELINE_STATE_DESC PipelineStateDesc;
    {
      PipelineStateDesc.pRootSignature = pComputeRootSignature_.Get();
      PipelineStateDesc.CS = pComputeShader->ShaderData.ByteCode;
      PipelineStateDesc.NodeMask = 0;
      PipelineStateDesc.CachedPSO = {};
      PipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
    }

    Microsoft::WRL::ComPtr<ID3D12PipelineState> pPipelineState;
    hr = pD3D12Device_->CreateComputePipelineState(&PipelineStateDesc, IID_PPV_ARGS(pPipelineState.GetAddressOf()));
    ERROR_CHECK(hr);

    CurrentComputeCommandList->SetPipelineState(pPipelineState.Get());
    CurrentComputeReserveData.PipelineStates.emplace_back(std::move(pPipelineState));
  }

  CurrentComputeCommandList->Dispatch(_ThreadGroupX, _ThreadGroupY, _ThreadGroupZ);
  CurrentComputeCommandList.Close();

  //  �Ō�̃R�}���h��GPU�œ���
  if (pLastFence_)
  {
    pLastFence_->Wait(pComputeCommandQueue_);
  }

  //  �R�}���h�𔭍s
  CurrentComputeCommandList.Execute(pComputeCommandQueue_);

  //  �Ō�̃t�F���X���X�V����GPU�ōX�V�ݒ�
  {
    pLastFence_ = &GetCurrentComputeFence();
    pLastFence_->Signal(pComputeCommandQueue_);
  }

  //  ���̃R�}���h���X�g�̐ݒ�
  {
    ++ComputeCommandBufferIndex_ %= Constants::kComputeCommandBufferNum;

    //  �ŏI�f�[�^�̈����p��
    {
      ComputeReserveData& NextComputeReserveData = GetCurrentComputeReserveData();
      NextComputeReserveData.Clear();
      NextComputeReserveData.ShaderResourceHeaps.resize(1);
      NextComputeReserveData.ShaderResourceHeaps[0] = std::move(CurrentComputeReserveData.ShaderResourceHeaps.back());
      NextComputeReserveData.SamplerHeaps.resize(1);
      NextComputeReserveData.SamplerHeaps[0] = std::move(CurrentComputeReserveData.SamplerHeaps.back());
      NextComputeReserveData.ConstantBufferHeaps.resize(1);
      NextComputeReserveData.ConstantBufferHeaps[0] = std::move(CurrentComputeReserveData.ConstantBufferHeaps.back());
      NextComputeReserveData.UnorderedAccessHeaps.resize(1);
      NextComputeReserveData.UnorderedAccessHeaps[0] = std::move(CurrentComputeReserveData.UnorderedAccessHeaps.back());
      NextComputeReserveData.PipelineStates = { std::move(CurrentComputeReserveData.PipelineStates.back()) };
    }
    ComputeStateChangeFlags_.Clear();
  }
}

void CDeviceContext::Flush()
{
  CommandList& CurrentGraphicsCommandList = GetCurrentGraphicsCommandList();
  CurrentGraphicsCommandList.Close();

  //  �Ō�̃R�}���h��GPU�œ���
  if (pLastFence_)
  {
    pLastFence_->Wait(pGraphicsCommandQueue_);
  }

  //  �R�}���h�𔭍s
  CurrentGraphicsCommandList.Execute(pGraphicsCommandQueue_);

  //  �Ō�̃t�F���X���X�V����GPU�ōX�V�ݒ�
  {
    pLastFence_ = &GetCurrentGraphicsFence();
    pLastFence_->Signal(pGraphicsCommandQueue_);
  }

  //  ���̃R�}���h���X�g�̐ݒ�
  {
    GraphicsReserveData& CurrentGraphicsReserveData = GetCurrentGraphicsReserveData();

    //  ���̃R�}���h���X�g��
    ++GraphicsCommandBufferIndex_ %= Constants::kGraphicsCommandBufferNum;

    //  CPU�œ���
    GetCurrentGraphicsFence().Wait(FenceEvent_);

    //  �ŏI�f�[�^�̈����p��
    GraphicsReserveData& NextGraphicsReserveData = GetCurrentGraphicsReserveData();
    {
      NextGraphicsReserveData.Clear();
      NextGraphicsReserveData.ShaderResourceHeaps.resize(1);
      NextGraphicsReserveData.ShaderResourceHeaps[0] = std::move(CurrentGraphicsReserveData.ShaderResourceHeaps.back());
      NextGraphicsReserveData.SamplerHeaps.resize(1);
      NextGraphicsReserveData.SamplerHeaps[0] = std::move(CurrentGraphicsReserveData.SamplerHeaps.back());
      NextGraphicsReserveData.ConstantBufferHeaps.resize(1);
      NextGraphicsReserveData.ConstantBufferHeaps[0] = std::move(CurrentGraphicsReserveData.ConstantBufferHeaps.back());
      NextGraphicsReserveData.PipelineStates.resize(1);
      NextGraphicsReserveData.PipelineStates[0] = std::move(CurrentGraphicsReserveData.PipelineStates.back());
    }

    //  �R�}���h���X�g�̏����ݒ�  
    CommandList& NextGraphicsCommandList = GetCurrentGraphicsCommandList();
    {
      NextGraphicsCommandList.Reset(NextGraphicsReserveData.PipelineStates[0].Get());
      NextGraphicsCommandList->SetGraphicsRootSignature(pGraphicsRootSignature_.Get());
      NextGraphicsCommandList->SetDescriptorHeaps(kDescriptorHeapNum, ppDescriptorHeaps_.data());
    }
  }
}

//--------------------------------------------------

void CDeviceContext::Present()
{
  pSwapChain_->GetD3D12RenderTexture()->TextureData.TransitionResourceBarrier(&GetCurrentGraphicsCommandList(), D3D12_RESOURCE_STATE_PRESENT);

  Flush();

  HRESULT hr = S_OK;

  hr = pDXGISwapChain_->Present(0, 0);
  ERROR_CHECK(hr);
}

//--------------------------------------------------

void CDeviceContext::PreprocessingDraw()
{
  HRESULT hr = S_OK;

  CommandList& CurrentGraphicsCommandList = GetCurrentGraphicsCommandList();

  //  �V�U�[�������̎��ɋ����I�ɐݒ肷��
  if (!CurrentGraphicsState_.RasterizerState.ScissorEnable)
  {
    GraphicsStateChangeFlags_.Set(GraphicsCommandType::eSetScissor);
  }

  if (GraphicsStateChangeFlags_.isEmpty())
  {
    return;
  }

  GraphicsReserveData& CurrentGraphicsReserveData = GetCurrentGraphicsReserveData();

  //  SetPipelineState
  for (auto& GraphicsPipelineStateCommand : kGraphicsPipelineStateCommands)
  {
    if (!GraphicsStateChangeFlags_.Has(GraphicsPipelineStateCommand))
    {
      continue;
    }

    D3D12_GRAPHICS_PIPELINE_STATE_DESC PipelineStateDesc = {};
    {
      PipelineStateDesc.pRootSignature = pGraphicsRootSignature_.Get();
      if (!CurrentGraphicsState_.VertexShader.isEmpty())
      {
        PipelineStateDesc.VS = Cast<CVertexShader>(pShaderManager_->GetShader(CurrentGraphicsState_.VertexShader.GetID()))->ShaderData.ByteCode;
      }
      if (!CurrentGraphicsState_.PixelShader.isEmpty())
      {
        PipelineStateDesc.PS = Cast<CPixelShader>(pShaderManager_->GetShader(CurrentGraphicsState_.PixelShader.GetID()))->ShaderData.ByteCode;
      }
      if (!CurrentGraphicsState_.DomainShader.isEmpty())
      {
        PipelineStateDesc.DS = Cast<CDomainShader>(pShaderManager_->GetShader(CurrentGraphicsState_.DomainShader.GetID()))->ShaderData.ByteCode;
      }
      if (!CurrentGraphicsState_.HullShader.isEmpty())
      {
        PipelineStateDesc.HS = Cast<CHullShader>(pShaderManager_->GetShader(CurrentGraphicsState_.HullShader.GetID()))->ShaderData.ByteCode;
      }
      if (!CurrentGraphicsState_.GeometryShader.isEmpty())
      {
        PipelineStateDesc.GS = Cast<CGeometryShader>(pShaderManager_->GetShader(CurrentGraphicsState_.GeometryShader.GetID()))->ShaderData.ByteCode;
      }
      PipelineStateDesc.BlendState = GetBlendDesc(CurrentGraphicsState_.BlendState);
      PipelineStateDesc.SampleMask = UINT_MAX;
      PipelineStateDesc.RasterizerState = GetRasterizerDesc(CurrentGraphicsState_.RasterizerState);
      PipelineStateDesc.DepthStencilState = GetDepthStecilDesc(CurrentGraphicsState_.DepthStencilState);
      const std::vector<D3D12_INPUT_ELEMENT_DESC>& InputLayout = InputLayouts_[CurrentGraphicsState_.InputLayout];
      PipelineStateDesc.InputLayout = { InputLayout.data(),static_cast<vdl::uint>(InputLayout.size()) };
      PipelineStateDesc.PrimitiveTopologyType = GetTopologyType(CurrentGraphicsState_.Topology);
      GraphicsRenderTargetCount_ = 0;
      const vdl::OutputManager& OutputManager = CurrentGraphicsReserveData.OutputManagers.back();
      for (; GraphicsRenderTargetCount_ < Constants::kMaxRenderTextureNum; ++GraphicsRenderTargetCount_)
      {
        if (OutputManager.RenderTextures[GraphicsRenderTargetCount_].isEmpty())
        {
          break;
        }
        PipelineStateDesc.RTVFormats[GraphicsRenderTargetCount_] = GetD3D12RenderTexture(OutputManager.RenderTextures[GraphicsRenderTargetCount_])->DXFormat;
      }
      PipelineStateDesc.NumRenderTargets = GraphicsRenderTargetCount_;
      if (!OutputManager.DepthStencilTexture.isEmpty())
      {
        PipelineStateDesc.DSVFormat = Cast<CDepthStencilTexture>(pTextureManager_->GetTexture(OutputManager.DepthStencilTexture.GetID()))->DXFormat;
      }
      PipelineStateDesc.SampleDesc.Count = 1;
      PipelineStateDesc.SampleDesc.Quality = 0;
      PipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
    }

    Microsoft::WRL::ComPtr<ID3D12PipelineState> pPipelineState;
    hr = pD3D12Device_->CreateGraphicsPipelineState(&PipelineStateDesc, IID_PPV_ARGS(pPipelineState.GetAddressOf()));
    ERROR_CHECK(hr);

    CurrentGraphicsCommandList->SetPipelineState(pPipelineState.Get());
    CurrentGraphicsReserveData.PipelineStates.emplace_back(std::move(pPipelineState));

    break;
  }

  //  SetDescriptor
  {
    //  SetShaderResource
    {
      auto BindShaderResources = [&](ShaderType _Type)->void
      {
        const ShaderResources& ShaderResources = CurrentGraphicsState_.ShaderResources[static_cast<vdl::uint>(_Type)];
        const vdl::uint ShaderResourceNum = static_cast<vdl::uint>(ShaderResources.size());

        //  �f�[�^�̓ǂݍ���
        std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> ShaderResourceDatas(ShaderResourceNum);
        {
          for (vdl::uint ShaderResourceCount = 0; ShaderResourceCount < ShaderResourceNum; ++ShaderResourceCount)
          {
            ShaderResourceDatas[ShaderResourceCount] = GetShaderResourceCPUHandle(ShaderResources[ShaderResourceCount], &CurrentGraphicsCommandList);
          }
        }

        constexpr DescriptorHeapType kDescriptorHeapType = DescriptorHeapType::eCBV_SRV_UAV;
        DescriptorAllocator& DescriptorAllocator = DescriptorAllocators_[static_cast<vdl::uint>(kDescriptorHeapType)];

        DescriptorHeap& DescriptorHeap = CurrentGraphicsReserveData.ShaderResourceHeaps.emplace_back();
        DescriptorAllocator.Allocate(&DescriptorHeap, ShaderResourceNum);
        pD3D12Device_->CopyDescriptors(1, &DescriptorHeap.CPUHandle, &ShaderResourceNum,
          ShaderResourceNum, ShaderResourceDatas.data(), nullptr, Cast(kDescriptorHeapType));

        constexpr DescriptorType kDescriptorType = DescriptorType::eShaderResource;
        CurrentGraphicsCommandList->SetGraphicsRootDescriptorTable(GetDescriptorOffset(_Type, kDescriptorType), DescriptorHeap.GPUHandle);

        CurrentGraphicsReserveData.ShaderResources.push_back(ShaderResources);
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
        const vdl::uint SamplerNum = static_cast<vdl::uint>(Samplers.size());

        //  �f�[�^�̓ǂݍ���
        std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> SamplerDatas(SamplerNum);
        {
          for (vdl::uint SamplerCount = 0; SamplerCount < SamplerNum; ++SamplerCount)
          {
            SamplerDatas[SamplerCount] = GetSamplerCPUHandle(Samplers[SamplerCount]);
          }
        }

        constexpr DescriptorHeapType kDescriptorHeapType = DescriptorHeapType::eSampler;
        DescriptorAllocator& DescriptorAllocator = DescriptorAllocators_[static_cast<vdl::uint>(kDescriptorHeapType)];

        DescriptorHeap& DescriptorHeap = CurrentGraphicsReserveData.SamplerHeaps.emplace_back();
        DescriptorAllocator.Allocate(&DescriptorHeap, SamplerNum);

        pD3D12Device_->CopyDescriptors(1, &DescriptorHeap.CPUHandle, &SamplerNum,
          SamplerNum, SamplerDatas.data(), nullptr, Cast(kDescriptorHeapType));

        constexpr DescriptorType kDescriptorType = DescriptorType::eSampler;
        CurrentGraphicsCommandList->SetGraphicsRootDescriptorTable(GetDescriptorOffset(_Type, kDescriptorType), DescriptorHeap.GPUHandle);
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
        const vdl::uint ConstantBufferNum = static_cast<vdl::uint>(ConstantBuffers.size());

        //  �f�[�^�̓ǂݍ���
        std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> ConstantBufferDatas(ConstantBufferNum);
        {
          for (vdl::uint ConstantBufferCount = 0; ConstantBufferCount < ConstantBufferNum; ++ConstantBufferCount)
          {
            ConstantBufferDatas[ConstantBufferCount] = GetConstantBufferCPUHandle(ConstantBuffers[ConstantBufferCount]);
          }
        }

        constexpr DescriptorHeapType kDescriptorHeapType = DescriptorHeapType::eCBV_SRV_UAV;
        DescriptorAllocator& DescriptorAllocator = DescriptorAllocators_[static_cast<vdl::uint>(kDescriptorHeapType)];

        DescriptorHeap& DescriptorHeap = CurrentGraphicsReserveData.ConstantBufferHeaps.emplace_back();
        DescriptorAllocator.Allocate(&DescriptorHeap, ConstantBufferNum);

        pD3D12Device_->CopyDescriptors(1, &DescriptorHeap.CPUHandle, &ConstantBufferNum,
          ConstantBufferNum, ConstantBufferDatas.data(), nullptr, Cast(kDescriptorHeapType));

        constexpr DescriptorType kDescriptorType = DescriptorType::eConstantBuffer;
        CurrentGraphicsCommandList->SetGraphicsRootDescriptorTable(GetDescriptorOffset(_Type, kDescriptorType), DescriptorHeap.GPUHandle);

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
    const CVertexBuffer* pVertexBuffer = Cast<CVertexBuffer>(pBufferManager_->GetBuffer(CurrentGraphicsState_.VertexBuffer.GetID()));
    D3D12_VERTEX_BUFFER_VIEW BufferView = pVertexBuffer->View;
    BufferView.StrideInBytes = GetVertexBufferStride();

    CurrentGraphicsCommandList->IASetVertexBuffers(0, 1, &BufferView);

    CurrentGraphicsReserveData.VertexBuffers.push_back(CurrentGraphicsState_.VertexBuffer);
  }

  if (GraphicsStateChangeFlags_.Has(GraphicsCommandType::eSetInstanceBuffer) && !CurrentGraphicsState_.InstanceBuffer.isEmpty())
  {
    const CInstanceBuffer* pInstanceBuffer = Cast<CInstanceBuffer>(pBufferManager_->GetBuffer(CurrentGraphicsState_.InstanceBuffer.GetID()));
    D3D12_VERTEX_BUFFER_VIEW BufferView;
    {
      BufferView.BufferLocation = pInstanceBuffer->BufferData.pResource->GetGPUVirtualAddress() + pInstanceBuffer->PreviousOffset;
      BufferView.SizeInBytes = pInstanceBuffer->Offset - pInstanceBuffer->PreviousOffset;
      BufferView.StrideInBytes = GetInstanceBufferStride();
    }
    CurrentGraphicsCommandList->IASetVertexBuffers(1, 1, &BufferView);

    CurrentGraphicsReserveData.InstanceBuffers.push_back(CurrentGraphicsState_.InstanceBuffer);
  }

  if (GraphicsStateChangeFlags_.Has(GraphicsCommandType::eSetIndexBuffer) && !CurrentGraphicsState_.IndexBuffer.isEmpty())
  {
    const CIndexBuffer* pIndexBuffer = Cast<CIndexBuffer>(pBufferManager_->GetBuffer(CurrentGraphicsState_.IndexBuffer.GetID()));

    CurrentGraphicsCommandList->IASetIndexBuffer(&pIndexBuffer->View);

    CurrentGraphicsReserveData.IndexBuffers.push_back(CurrentGraphicsState_.IndexBuffer);
  }

  if (GraphicsStateChangeFlags_.Has(GraphicsCommandType::eSetTopology))
  {
    CurrentGraphicsCommandList->IASetPrimitiveTopology(Cast(CurrentGraphicsState_.Topology));
  }

  if (GraphicsStateChangeFlags_.Has(GraphicsCommandType::eSetScissor))
  {
    CurrentGraphicsCommandList->RSSetScissorRects(1, &Cast(CurrentGraphicsState_.RasterizerState.ScissorEnable ? CurrentGraphicsState_.Scissor
      : vdl::Scissor(CurrentGraphicsState_.Viewport.LeftTop, CurrentGraphicsState_.Viewport.Size)));
  }

  if (GraphicsStateChangeFlags_.Has(GraphicsCommandType::eSetViewport))
  {
    CurrentGraphicsCommandList->RSSetViewports(1, &Cast(CurrentGraphicsState_.Viewport));
  }

  if (GraphicsStateChangeFlags_.Has(GraphicsCommandType::eSetRenderTextures))
  {
    const vdl::OutputManager& OutputManager = CurrentGraphicsReserveData.OutputManagers.back();

    std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> RenderTargetDescriptors(GraphicsRenderTargetCount_);
    {
      constexpr D3D12_RESOURCE_STATES kResourceState = D3D12_RESOURCE_STATE_RENDER_TARGET;

      for (vdl::uint RenderTextureCount = 0; RenderTextureCount < GraphicsRenderTargetCount_; ++RenderTextureCount)
      {
        CRenderTexture* pRenderTexture = GetD3D12RenderTexture(OutputManager.RenderTextures[RenderTextureCount]);
        if (pRenderTexture->TextureData.ResourceState != kResourceState)
        {
          pRenderTexture->TextureData.TransitionResourceBarrier(&CurrentGraphicsCommandList, kResourceState);
        }
        RenderTargetDescriptors[RenderTextureCount] = pRenderTexture->RenderTargetViewHeap.CPUHandle;
      }
    }

    const D3D12_CPU_DESCRIPTOR_HANDLE* DepthStencilCPUHandle = nullptr;
    {
      if (!OutputManager.DepthStencilTexture.isEmpty())
      {
        constexpr D3D12_RESOURCE_STATES kResourceState = D3D12_RESOURCE_STATE_DEPTH_WRITE;

        CDepthStencilTexture* pDepthStencilTexture = Cast<CDepthStencilTexture>(pTextureManager_->GetTexture(OutputManager.DepthStencilTexture.GetID()));
        if (pDepthStencilTexture->TextureData.ResourceState != kResourceState)
        {
          pDepthStencilTexture->TextureData.TransitionResourceBarrier(&CurrentGraphicsCommandList, kResourceState);
        }

        DepthStencilCPUHandle = &pDepthStencilTexture->DepthStencilViewHeap.CPUHandle;
      }
    }

    CurrentGraphicsCommandList->OMSetRenderTargets(GraphicsRenderTargetCount_, RenderTargetDescriptors.data(), false, DepthStencilCPUHandle);
  }

  GraphicsStateChangeFlags_.Clear();
}

CRenderTexture* CDeviceContext::GetD3D12RenderTexture(const vdl::RenderTexture& _RenderTexture)
{
  assert(!_RenderTexture.isEmpty());

  ITexture* pTexture = pTextureManager_->GetTexture(_RenderTexture.GetID());
  return (pTexture->GetType() == TextureType::eSwapChainRenderTexture ? pSwapChain_->GetD3D12RenderTexture() : Cast<CRenderTexture>(pTexture));
}

const D3D12_BLEND_DESC& CDeviceContext::GetBlendDesc(const vdl::BlendState& _BlendState)
{
  if (BlendStates_.find(_BlendState) == BlendStates_.end())
  {
    D3D12_BLEND_DESC BlendDesc;
    {
      BlendDesc.AlphaToCoverageEnable = _BlendState.AlphaToCoverageEnable;
      BlendDesc.IndependentBlendEnable = _BlendState.IndependentBlendEnable;

      const vdl::uint EnableRenderTextureNum = (_BlendState.IndependentBlendEnable ? Constants::kMaxRenderTextureNum : 1);

      for (vdl::uint RenderTextureCount = 0; RenderTextureCount < EnableRenderTextureNum; ++RenderTextureCount)
      {
        D3D12_RENDER_TARGET_BLEND_DESC& RenderTargetBlendDesc = BlendDesc.RenderTarget[RenderTextureCount];
        const vdl::RenderTextureBlendState& RenderTextureBlendState = _BlendState.RenderTexture[RenderTextureCount];

        RenderTargetBlendDesc.BlendEnable = RenderTextureBlendState.BlendEnable;
        RenderTargetBlendDesc.LogicOpEnable = false;
        RenderTargetBlendDesc.SrcBlend = Cast(RenderTextureBlendState.SrcBlend);
        RenderTargetBlendDesc.DestBlend = Cast(RenderTextureBlendState.DestBlend);
        RenderTargetBlendDesc.BlendOp = Cast(RenderTextureBlendState.BlendOp);
        RenderTargetBlendDesc.SrcBlendAlpha = Cast(RenderTextureBlendState.SrcBlendAlpha);
        RenderTargetBlendDesc.DestBlendAlpha = Cast(RenderTextureBlendState.DestBlendAlpha);
        RenderTargetBlendDesc.BlendOpAlpha = Cast(RenderTextureBlendState.BlendOpAlpha);
        RenderTargetBlendDesc.RenderTargetWriteMask = Cast(RenderTextureBlendState.RenderTargetWriteMask);
      }
    }

    BlendStates_.insert(std::make_pair(_BlendState, std::move(BlendDesc)));
  }

  return BlendStates_.at(_BlendState);
}

const D3D12_DEPTH_STENCIL_DESC& CDeviceContext::GetDepthStecilDesc(const vdl::DepthStencilState& _DepthStencilState)
{
  if (DepthStencilStates_.find(_DepthStencilState) == DepthStencilStates_.end())
  {
    D3D12_DEPTH_STENCIL_DESC DepthStencilDesc;
    {
      DepthStencilDesc.DepthEnable = _DepthStencilState.DepthEnable;
      DepthStencilDesc.DepthWriteMask = Cast(_DepthStencilState.DepthWriteMask);
      DepthStencilDesc.DepthFunc = Cast(_DepthStencilState.DepthFunc);
      DepthStencilDesc.StencilEnable = _DepthStencilState.StencilEnable;
      DepthStencilDesc.StencilReadMask = 0xFF;
      DepthStencilDesc.StencilWriteMask = _DepthStencilState.StencilWriteMask;
      DepthStencilDesc.FrontFace.StencilFailOp = Cast(_DepthStencilState.FrontFace.StencilFailOp);
      DepthStencilDesc.FrontFace.StencilDepthFailOp = Cast(_DepthStencilState.FrontFace.StencilDepthFailOp);
      DepthStencilDesc.FrontFace.StencilPassOp = Cast(_DepthStencilState.FrontFace.StencilPassOp);
      DepthStencilDesc.FrontFace.StencilFunc = Cast(_DepthStencilState.FrontFace.StencilFunc);
      DepthStencilDesc.BackFace.StencilFailOp = Cast(_DepthStencilState.BackFace.StencilFailOp);
      DepthStencilDesc.BackFace.StencilDepthFailOp = Cast(_DepthStencilState.BackFace.StencilDepthFailOp);
      DepthStencilDesc.BackFace.StencilPassOp = Cast(_DepthStencilState.BackFace.StencilPassOp);
      DepthStencilDesc.BackFace.StencilFunc = Cast(_DepthStencilState.BackFace.StencilFunc);
    }

    DepthStencilStates_.insert(std::make_pair(_DepthStencilState, std::move(DepthStencilDesc)));
  }

  return DepthStencilStates_.at(_DepthStencilState);
}

const D3D12_RASTERIZER_DESC& CDeviceContext::GetRasterizerDesc(const vdl::RasterizerState& _RasterizerState)
{
  if (RasterizerStates_.find(_RasterizerState) == RasterizerStates_.end())
  {
    D3D12_RASTERIZER_DESC RasterizerDesc;
    {
      RasterizerDesc.FillMode = Cast(_RasterizerState.FillMode);
      RasterizerDesc.CullMode = Cast(_RasterizerState.CullMode);
      RasterizerDesc.FrontCounterClockwise = _RasterizerState.FrontCounterClockwise;
      RasterizerDesc.DepthBias = _RasterizerState.DepthBias;
      RasterizerDesc.DepthBiasClamp = 0.0f;
      RasterizerDesc.SlopeScaledDepthBias = 0.0f;
      RasterizerDesc.DepthClipEnable = _RasterizerState.DepthClipEnable;
      RasterizerDesc.MultisampleEnable = false;
      RasterizerDesc.AntialiasedLineEnable = _RasterizerState.AntialiasedLineEnable;
      RasterizerDesc.ForcedSampleCount = 0;
      RasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
    }

    RasterizerStates_.insert(std::make_pair(_RasterizerState, std::move(RasterizerDesc)));
  }

  return RasterizerStates_.at(_RasterizerState);
}

const D3D12_CPU_DESCRIPTOR_HANDLE& CDeviceContext::GetShaderResourceCPUHandle(const vdl::ShaderResource& _ShaderResource, CommandList* _pCommandList)
{
  constexpr D3D12_RESOURCE_STATES kResourceState = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

  //  Texture
  if (const vdl::Texture* pShaderResource = _ShaderResource.GetIf<vdl::Texture>())
  {
    if (!pShaderResource->isEmpty())
    {
      ITexture* pTexture = pTextureManager_->GetTexture(pShaderResource->GetID());

      switch (pTexture->GetType())
      {
      case TextureType::eDepthStencilTexture:
      case TextureType::eSwapChainRenderTexture:
        assert(false);
      case TextureType::eDepthTexture:
      {
        CDepthTexture* pDepthTexture = Cast<CDepthTexture>(pTexture);
        if (pDepthTexture->pParent->TextureData.ResourceState != kResourceState)
        {
          pDepthTexture->pParent->TextureData.TransitionResourceBarrier(_pCommandList, kResourceState);
        }
        return pDepthTexture->ShaderResourceViewHeap.CPUHandle;
      }
      case TextureType::eStencilTexture:
      {
        CStencilTexture* pStencilTexture = Cast<CStencilTexture>(pTexture);
        if (pStencilTexture->pParent->TextureData.ResourceState != kResourceState)
        {
          pStencilTexture->pParent->TextureData.TransitionResourceBarrier(_pCommandList, kResourceState);
        }
        return pStencilTexture->ShaderResourceViewHeap.CPUHandle;
      }
      default:
      {
        CTexture* pColorTexture = Cast<CTexture>(pTexture);
        if (pColorTexture->TextureData.ResourceState != kResourceState)
        {
          pColorTexture->TextureData.TransitionResourceBarrier(_pCommandList, kResourceState);
        }
        return pColorTexture->ShaderResourceViewHeap.CPUHandle;
      }
      }
    }
  }
  //  CubeTexture
  if (const vdl::CubeTexture* pShaderResource = _ShaderResource.GetIf<vdl::CubeTexture>())
  {
    if (!pShaderResource->isEmpty())
    {
      CCubeTexture* pCubeTexture = Cast<CCubeTexture>(pTextureManager_->GetTexture(pShaderResource->GetID()));
      if (pCubeTexture->TextureData.ResourceState != kResourceState)
      {
        pCubeTexture->TextureData.TransitionResourceBarrier(_pCommandList, kResourceState);
      }
      return pCubeTexture->ShaderResourceViewHeap.CPUHandle;
    }
  }
  //  UnorderedAccessBuffer
  else if (const vdl::Detail::UnorderedAccessBufferData* pShaderResource = _ShaderResource.GetIf<vdl::Detail::UnorderedAccessBufferData>())
  {
    if (!pShaderResource->isEmpty())
    {
      CUnordererdAccessBuffer* pUnorderedAccessBuffer = Cast<CUnordererdAccessBuffer>(pBufferManager_->GetBuffer(pShaderResource->GetID()));
      if (pUnorderedAccessBuffer->ResourceState != kResourceState)
      {
        CTexture::TransitionResourceBarrier(_pCommandList, pUnorderedAccessBuffer->BufferData.pResource.Get(), pUnorderedAccessBuffer->ResourceState, kResourceState);
        pUnorderedAccessBuffer->ResourceState = kResourceState;
      }
      return pUnorderedAccessBuffer->ShaderResourceViewHeap.CPUHandle;
    }
  }

  return DefaultViewCPUHandle_;
}

const D3D12_CPU_DESCRIPTOR_HANDLE& CDeviceContext::GetSamplerCPUHandle(const vdl::Sampler& _Sampler)
{
  if (Samplers_.find(_Sampler) == Samplers_.end())
  {
    D3D12_SAMPLER_DESC SamplerDesc;
    {
      SamplerDesc.Filter = Cast(_Sampler.Filter);
      SamplerDesc.AddressU = Cast(_Sampler.AddressModeU);
      SamplerDesc.AddressV = Cast(_Sampler.AddressModeV);
      SamplerDesc.AddressW = Cast(_Sampler.AddressModeW);
      SamplerDesc.MipLODBias = 0.0f;
      SamplerDesc.MaxAnisotropy = _Sampler.MaxAnisotropy;
      SamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
      ::memcpy(SamplerDesc.BorderColor, &Cast(_Sampler.BorderColor), sizeof(float) * 4);
      SamplerDesc.MinLOD = 0;
      SamplerDesc.MaxLOD = FLT_MAX;
    }

    DescriptorHeap DescriptorHeap;
    pSamplerDescriptorAllocator_->Allocate(&DescriptorHeap);
    pD3D12Device_->CreateSampler(&SamplerDesc, DescriptorHeap.CPUHandle);

    Samplers_[_Sampler] = std::move(DescriptorHeap);
  }

  return Samplers_.at(_Sampler).CPUHandle;
}

const D3D12_CPU_DESCRIPTOR_HANDLE& CDeviceContext::GetConstantBufferCPUHandle(const vdl::Detail::ConstantBufferData& _ConstantBuffer)
{
  if (!_ConstantBuffer.isEmpty())
  {
    return Cast<CCopyConstantBuffer>(pBufferManager_->GetBuffer(_ConstantBuffer.GetID()))->ConstantBufferViewHeap.CPUHandle;
  }

  return DefaultViewCPUHandle_;
}

const D3D12_CPU_DESCRIPTOR_HANDLE& CDeviceContext::GetUnorderedAccessObjectCPUHandle(const vdl::UnorderedAccessObject& _UnorderedAccessObject, CommandList* _pCommandList)
{
  constexpr D3D12_RESOURCE_STATES kResourceState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;

  //  UnorderedAccessTexture
  if (const vdl::UnorderedAccessTexture* pUnorderedAccessObject = _UnorderedAccessObject.GetIf<vdl::UnorderedAccessTexture>())
  {
    if (!pUnorderedAccessObject->isEmpty())
    {
      CUnorderedAccessTexture* pUnorderedAccessTexture = Cast<CUnorderedAccessTexture>(pTextureManager_->GetTexture(pUnorderedAccessObject->GetID()));
      if (pUnorderedAccessTexture->TextureData.ResourceState != kResourceState)
      {
        pUnorderedAccessTexture->TextureData.TransitionResourceBarrier(_pCommandList, kResourceState);
      }
      return pUnorderedAccessTexture->UnorderedAccessViewHeap.CPUHandle;
    }
  }
  //  UnorderedAccessBuffer
  else if (const vdl::Detail::UnorderedAccessBufferData* pUnorderedAccessObject = _UnorderedAccessObject.GetIf<vdl::Detail::UnorderedAccessBufferData>())
  {
    if (!pUnorderedAccessObject->isEmpty())
    {
      CUnordererdAccessBuffer* pUnorderedAccessBuffer = Cast<CUnordererdAccessBuffer>(pBufferManager_->GetBuffer(pUnorderedAccessObject->GetID()));
      if (pUnorderedAccessBuffer->ResourceState != kResourceState)
      {
        CTexture::TransitionResourceBarrier(_pCommandList, pUnorderedAccessBuffer->BufferData.pResource.Get(), pUnorderedAccessBuffer->ResourceState, kResourceState);
        pUnorderedAccessBuffer->ResourceState = kResourceState;
      }
      return pUnorderedAccessBuffer->UnorderedAccessViewHeap.CPUHandle;
    }
  }

  return DefaultViewCPUHandle_;
}

vdl::uint CDeviceContext::GetVertexBufferStride()const
{
  switch (CurrentGraphicsState_.InputLayout)
  {
  case vdl::InputLayoutType::eTexture: return sizeof(vdl::Vertex2D);
  case vdl::InputLayoutType::eStaticMesh: return sizeof(vdl::VertexSkinnedMesh);
  case vdl::InputLayoutType::eSkinnedMesh: return sizeof(vdl::VertexSkinnedMesh);
  case vdl::InputLayoutType::eGUI: return sizeof(ImDrawVert);
  default: assert(false);
  }

  return 0;
}

vdl::uint CDeviceContext::GetInstanceBufferStride()const
{
  switch (CurrentGraphicsState_.InputLayout)
  {
  case vdl::InputLayoutType::eTexture: return sizeof(vdl::Instance2D);
  case vdl::InputLayoutType::eStaticMesh: return sizeof(vdl::InstanceStaticMesh);
  case vdl::InputLayoutType::eSkinnedMesh: return sizeof(vdl::InstanceSkinnedMesh);
  default: assert(false);
  }

  return 0;
}
