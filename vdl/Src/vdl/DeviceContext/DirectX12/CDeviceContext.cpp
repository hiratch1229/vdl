#include "CDeviceContext.hpp"

#include <vdl/Engine.hpp>
#include <vdl/Device/DirectX12/CDevice.hpp>
#include <vdl/Window/IWindow.hpp>
#include <vdl/TextureManager/ITextureManager.hpp>
#include <vdl/BufferManager/IBufferManager.hpp>
#include <vdl/ShaderManager/IShaderManager.hpp>
#include <vdl/Renderer/IRenderer.hpp>

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
  pWindow_ = Engine::Get<IWindow>();
  pTextureManager_ = Engine::Get<ITextureManager>();
  pBufferManager_ = Engine::Get<IBufferManager>();
  pShaderManager_ = Engine::Get<IShaderManager>();
  pRenderer_ = Engine::Get<IRenderer>();

  pD3D12Device_ = pDevice_->GetDevice();
  SwapChain_.Initialize(pDevice_);
  CurrentBackBufferIndex_ = SwapChain_->GetCurrentBackBufferIndex();
  pGraphicsCommandQueue_ = pDevice_->GetCommandQueueManager()->GetGraphicsQueue();
  pComputeCommandQueue_ = pDevice_->GetCommandQueueManager()->GetComputeQueue();
  pSamplerDescriptorAllocator_ = pDevice_->GetDescriptorAllocator(DescriptorHeapType::eSampler);

  //  入力レイアウトの作成
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

  //  ディスクリプタヒープの作成
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

  //  フェンスイベントの作成
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

  //  グラフィックス用のオブジェクトの作成
  {
    //  ルートシグネチャの作成
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

    //  コマンドリストの作成
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

    //  フェンスの初期化
    for (auto& GraphicsFence : GraphicsFences_)
    {
      GraphicsFence.Initialize(pD3D12Device_);
    }

    //  保存データの初期化
    {
      GraphicsReserveData& GraphicsReserveData = GetCurrentGraphicsReserveData();
      GraphicsReserveData.ShaderResourceHeaps.resize(1);
      GraphicsReserveData.SamplerHeaps.resize(1);
      GraphicsReserveData.ConstantBufferHeaps.resize(1);
      GraphicsReserveData.PipelineStates.resize(1);
    }

    //  パイプラインステートの初期化
    {
      GraphicsPipelineStateDesc_.pRootSignature = pGraphicsRootSignature_.Get();
      GraphicsPipelineStateDesc_.SampleMask = vdl::Math::kMax<vdl::uint>;
      GraphicsPipelineStateDesc_.SampleDesc.Count = 1;
      GraphicsPipelineStateDesc_.SampleDesc.Quality = 0;
      GraphicsPipelineStateDesc_.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
    }
  }

  //  コンピュート用のオブジェクトの作成
  {
    //  ルートシグネチャの作成
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

    //  コマンドリストの作成
    {
      for (vdl::uint i = 0; i < Constants::kComputeCommandBufferNum; ++i)
      {
        ComputeCommandLists_[i].Initialize(pD3D12Device_, D3D12_COMMAND_LIST_TYPE_COMPUTE);
      }

      GetCurrentComputeReserveData().PipelineStates.resize(1);
    }

    //  フェンスの初期化
    for (auto& ComputeFence : ComputeFences_)
    {
      ComputeFence.Initialize(pD3D12Device_);
    }

    //  保存データの初期化
    {
      ComputeReserveData& ComputeReserveData = GetCurrentComputeReserveData();
      ComputeReserveData.ShaderResourceHeaps.resize(1);
      ComputeReserveData.SamplerHeaps.resize(1);
      ComputeReserveData.ConstantBufferHeaps.resize(1);
      ComputeReserveData.UnorderedAccessHeaps.resize(1);
      ComputeReserveData.PipelineStates.resize(1);
    }

    //  パイプラインステートの初期化
    {
      ComputePipelineStateDesc_.pRootSignature = pComputeRootSignature_.Get();
      ComputePipelineStateDesc_.NodeMask = 0;
      ComputePipelineStateDesc_.CachedPSO = {};
      ComputePipelineStateDesc_.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
    }
  }
}

void CDeviceContext::SetRenderTextures(const vdl::RenderTextures& _RenderTextures, const vdl::DepthStencilTexture& _DepthStenilTexture)
{
  vdl::OutputManager OutputManager = { _RenderTextures, _DepthStenilTexture };
  {
    //  ステートの設定
    {
      vdl::uint RenderTargetCount = 0;
      for (; RenderTargetCount < Constants::kMaxRenderTextureNum; ++RenderTargetCount)
      {
        if (OutputManager.RenderTextures[RenderTargetCount].isEmpty())
        {
          break;
        }
        GraphicsPipelineStateDesc_.RTVFormats[RenderTargetCount] = GetD3D12RenderTexture(OutputManager.RenderTextures[RenderTargetCount])->DXFormat;
      }
      GraphicsPipelineStateDesc_.NumRenderTargets = RenderTargetCount;
      for (; RenderTargetCount < Constants::kMaxRenderTextureNum; ++RenderTargetCount)
      {
        GraphicsPipelineStateDesc_.RTVFormats[RenderTargetCount] = DXGI_FORMAT_UNKNOWN;
      }

      GraphicsPipelineStateDesc_.DSVFormat = (OutputManager.DepthStencilTexture.isEmpty() ? DXGI_FORMAT_UNKNOWN : Cast<CDepthStencilTexture>(pTextureManager_->GetTexture(OutputManager.DepthStencilTexture.GetID()))->DXFormat);
    }

    //  コマンドの書き込み
    {
      CommandList& CurrentGraphicsCommandList = GetCurrentGraphicsCommandList();

      std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> RenderTargetDescriptors(GraphicsPipelineStateDesc_.NumRenderTargets);
      {
        constexpr D3D12_RESOURCE_STATES kResourceState = D3D12_RESOURCE_STATE_RENDER_TARGET;

        for (vdl::uint RenderTextureCount = 0; RenderTextureCount < GraphicsPipelineStateDesc_.NumRenderTargets; ++RenderTextureCount)
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

      CurrentGraphicsCommandList->OMSetRenderTargets(GraphicsPipelineStateDesc_.NumRenderTargets, RenderTargetDescriptors.data(), false, DepthStencilCPUHandle);
    }
  }

  GetCurrentGraphicsReserveData().OutputManagers.emplace_back(std::move(OutputManager));
  isChangeGraphicsPipelineState_ = true;
}

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

  //  Clearするまで生存を保証
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

  //  Clearするまで生存を保証
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

  //  Clearするまで生存を保証
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
  //  データを保有する事でリファレンスカウントの維持
  GraphicsCommandListDatas_[GraphicsCommandBufferIndex_] = _GraphicsCommandList;

  CommandList& CurrentGraphicsCommandList = GetCurrentGraphicsCommandList();
  GraphicsReserveData& CurrentGraphicsReserveData = GetCurrentGraphicsReserveData();

  //  TODO:C++20で書き換え
  auto BindShaderResources = [&](const ShaderResources& _ShaderResouces, ShaderType _Type)->void
  {
    const vdl::uint ShaderResourceNum = static_cast<vdl::uint>(_ShaderResouces.size());

    if (ShaderResourceNum == 0)
    {
      return;
    }

    //  データの読み込み
    std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> ShaderResourceDatas(ShaderResourceNum);
    {
      for (vdl::uint ShaderResourceCount = 0; ShaderResourceCount < ShaderResourceNum; ++ShaderResourceCount)
      {
        ShaderResourceDatas[ShaderResourceCount] = GetShaderResourceCPUHandle(_ShaderResouces[ShaderResourceCount], &CurrentGraphicsCommandList);
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
  };

  //  TODO:C++20で書き換え
  auto BindSamplers = [&](const Samplers& _Samplers, ShaderType _Type)->void
  {
    const vdl::uint SamplerNum = static_cast<vdl::uint>(_Samplers.size());
    if (SamplerNum == 0)
    {
      return;
    }

    //  データの読み込み
    std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> SamplerDatas(SamplerNum);
    {
      for (vdl::uint SamplerCount = 0; SamplerCount < SamplerNum; ++SamplerCount)
      {
        SamplerDatas[SamplerCount] = GetSamplerCPUHandle(_Samplers[SamplerCount]);
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

  //  TODO:C++20で書き換え
  auto BindConstantBuffers = [&](const ConstantBuffers& _ConstantBuffers, ShaderType _Type)
  {
    const vdl::uint ConstantBufferNum = static_cast<vdl::uint>(_ConstantBuffers.size());
    if (ConstantBufferNum == 0)
    {
      return;
    }

    //  データの読み込み
    std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> ConstantBufferDatas(ConstantBufferNum);
    {
      for (vdl::uint ConstantBufferCount = 0; ConstantBufferCount < ConstantBufferNum; ++ConstantBufferCount)
      {
        ConstantBufferDatas[ConstantBufferCount] = GetConstantBufferCPUHandle(_ConstantBuffers[ConstantBufferCount]);
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
  };

  HRESULT hr = S_OK;

  CInstanceBuffer* pInstanceBuffer = nullptr;
  bool ScissorEnable = true;
  vdl::Viewport Viewport;

  //  DrawCommandの前処理
  auto PreDrawCommand = [&](vdl::uint _Index)
  {
    if (isChangeGraphicsPipelineState_)
    {
      Microsoft::WRL::ComPtr<ID3D12PipelineState>& pPipelineState = CurrentGraphicsReserveData.PipelineStates.emplace_back();
      hr = pD3D12Device_->CreateGraphicsPipelineState(&GraphicsPipelineStateDesc_, IID_PPV_ARGS(pPipelineState.GetAddressOf()));
      ERROR_CHECK(hr);

      CurrentGraphicsCommandList->SetPipelineState(pPipelineState.Get());

      isChangeGraphicsPipelineState_ = false;
    }

    if (pInstanceBuffer)
    {
      const auto& InstanceDatas = _GraphicsCommandList.GetInstanceDatas(_Index);
      pDevice_->WriteMemory(pInstanceBuffer, InstanceDatas.data(), static_cast<vdl::uint>(InstanceDatas.size()));

      D3D12_VERTEX_BUFFER_VIEW BufferView;
      {
        BufferView.BufferLocation = pInstanceBuffer->BufferData.pResource->GetGPUVirtualAddress() + pInstanceBuffer->PreviousOffset;
        BufferView.SizeInBytes = pInstanceBuffer->Offset - pInstanceBuffer->PreviousOffset;
        BufferView.StrideInBytes = GetInstanceBufferStride();
      }
      CurrentGraphicsCommandList->IASetVertexBuffers(1, 1, &BufferView);
    }
  };

  //  コマンドの書き込み
  for (auto& GraphicsCommand : _GraphicsCommandList.GetGraphicsCommands())
  {
    switch (GraphicsCommand.first)
    {
    case GraphicsCommandFlag::eDraw:
    {
      PreDrawCommand(GraphicsCommand.second);

      const DrawData& DrawData = _GraphicsCommandList.GetDrawData(GraphicsCommand.second);
      CurrentGraphicsCommandList->DrawInstanced(DrawData.VertexCount, DrawData.InstanceCount, DrawData.FirstVertex, DrawData.FirstInstance);
    }
    break;
    case GraphicsCommandFlag::eDrawIndexed:
    {
      PreDrawCommand(GraphicsCommand.second);

      const DrawIndexedData& DrawIndexedData = _GraphicsCommandList.GetDrawIndexedData(GraphicsCommand.second);
      CurrentGraphicsCommandList->DrawIndexedInstanced(DrawIndexedData.IndexCount, DrawIndexedData.InstanceCount, DrawIndexedData.FirstIndex, DrawIndexedData.VertexOffset, DrawIndexedData.FirstInstance);
    }
    break;
    case GraphicsCommandFlag::eSetVertexBuffer:
    {
      const CVertexBuffer* pVertexBuffer = Cast<CVertexBuffer>(pBufferManager_->GetBuffer(_GraphicsCommandList.GetVertexBuffer(GraphicsCommand.second).GetID()));
      D3D12_VERTEX_BUFFER_VIEW BufferView = pVertexBuffer->View;
      BufferView.StrideInBytes = GetVertexBufferStride();

      CurrentGraphicsCommandList->IASetVertexBuffers(0, 1, &BufferView);
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

      CurrentGraphicsCommandList->IASetIndexBuffer(&pIndexBuffer->View);
    }
    break;
    case GraphicsCommandFlag::eSetInputLayout:
    {
      CurrentInputLayoutType_ = _GraphicsCommandList.GetInputLayout(GraphicsCommand.second);
      const std::vector<D3D12_INPUT_ELEMENT_DESC>& InputLayout = InputLayouts_[CurrentInputLayoutType_];
      GraphicsPipelineStateDesc_.InputLayout = { InputLayout.data(),static_cast<vdl::uint>(InputLayout.size()) };
      isChangeGraphicsPipelineState_ = true;
    }
    break;
    case GraphicsCommandFlag::eSetTopology:
    {
      const vdl::TopologyType& Toplogy = _GraphicsCommandList.GetTopology(GraphicsCommand.second);
      GraphicsPipelineStateDesc_.PrimitiveTopologyType = GetTopologyType(Toplogy);
      CurrentGraphicsCommandList->IASetPrimitiveTopology(Cast(Toplogy));

      isChangeGraphicsPipelineState_ = true;
    }
    break;
    case GraphicsCommandFlag::eSetScissor:
    {
      RECT Scissor = Cast(ScissorEnable ? _GraphicsCommandList.GetScissor(GraphicsCommand.second)
        : vdl::Scissor(Viewport.LeftTop, Viewport.Size));
      CurrentGraphicsCommandList->RSSetScissorRects(1, &Scissor);
    }
    break;
    case GraphicsCommandFlag::eSetViewport:
    {
      D3D12_VIEWPORT ViewPort = Cast(Viewport = _GraphicsCommandList.GetViewport(GraphicsCommand.second));
      CurrentGraphicsCommandList->RSSetViewports(1, &ViewPort);
    }
    break;
    case GraphicsCommandFlag::eSetBlendState:
    {
      GraphicsPipelineStateDesc_.BlendState = GetBlendDesc(_GraphicsCommandList.GetBlendState(GraphicsCommand.second));
      isChangeGraphicsPipelineState_ = true;
    }
    break;
    case GraphicsCommandFlag::eSetDepthStencilState:
    {
      GraphicsPipelineStateDesc_.DepthStencilState = GetDepthStecilDesc(_GraphicsCommandList.GetDepthStencilState(GraphicsCommand.second));
      isChangeGraphicsPipelineState_ = true;
    }
    break;
    case GraphicsCommandFlag::eSetRasterizerState:
    {
      const vdl::RasterizerState& RasterizerState = _GraphicsCommandList.GetRasterizerState(GraphicsCommand.second);
      ScissorEnable = RasterizerState.ScissorEnable;

      GraphicsPipelineStateDesc_.RasterizerState = GetRasterizerDesc(RasterizerState);
      isChangeGraphicsPipelineState_ = true;
    }
    break;
    case GraphicsCommandFlag::eSetVertexShader:
    {
      D3D12_SHADER_BYTECODE ShaderBytecode = {};
      {
        if (const vdl::VertexShader& VertexShader = _GraphicsCommandList.GetVertexShader(GraphicsCommand.second);
          !VertexShader.isEmpty())
        {
          ShaderBytecode = Cast<CVertexShader>(pShaderManager_->GetShader(VertexShader.GetID()))->ShaderData.ByteCode;
        }
      }

      GraphicsPipelineStateDesc_.VS = std::move(ShaderBytecode);
      isChangeGraphicsPipelineState_ = true;
    }
    break;
    case GraphicsCommandFlag::eSetHullShader:
    {
      D3D12_SHADER_BYTECODE ShaderBytecode = {};
      {
        if (const vdl::HullShader& HullShader = _GraphicsCommandList.GetHullShader(GraphicsCommand.second);
          !HullShader.isEmpty())
        {
          ShaderBytecode = Cast<CHullShader>(pShaderManager_->GetShader(HullShader.GetID()))->ShaderData.ByteCode;
        }
      }

      GraphicsPipelineStateDesc_.HS = std::move(ShaderBytecode);
      isChangeGraphicsPipelineState_ = true;
    }
    break;
    case GraphicsCommandFlag::eSetDomainShader:
    {
      D3D12_SHADER_BYTECODE ShaderBytecode = {};
      {
        if (const vdl::DomainShader& DomainShader = _GraphicsCommandList.GetDomainShader(GraphicsCommand.second);
          !DomainShader.isEmpty())
        {
          ShaderBytecode = Cast<CDomainShader>(pShaderManager_->GetShader(DomainShader.GetID()))->ShaderData.ByteCode;
        }
      }

      GraphicsPipelineStateDesc_.DS = std::move(ShaderBytecode);
      isChangeGraphicsPipelineState_ = true;
    }
    break;
    case GraphicsCommandFlag::eSetGeometryShader:
    {
      D3D12_SHADER_BYTECODE ShaderBytecode = {};
      {
        if (const vdl::GeometryShader& GeometryShader = _GraphicsCommandList.GetGeometryShader(GraphicsCommand.second);
          !GeometryShader.isEmpty())
        {
          ShaderBytecode = Cast<CGeometryShader>(pShaderManager_->GetShader(GeometryShader.GetID()))->ShaderData.ByteCode;
        }
      }

      GraphicsPipelineStateDesc_.GS = std::move(ShaderBytecode);
      isChangeGraphicsPipelineState_ = true;
    }
    break;
    case GraphicsCommandFlag::eSetPixelShader:
    {
      D3D12_SHADER_BYTECODE ShaderBytecode = {};
      {
        if (const vdl::PixelShader& PixelShader = _GraphicsCommandList.GetPixelShader(GraphicsCommand.second);
          !PixelShader.isEmpty())
        {
          ShaderBytecode = Cast<CPixelShader>(pShaderManager_->GetShader(PixelShader.GetID()))->ShaderData.ByteCode;
        }
      }

      GraphicsPipelineStateDesc_.PS = std::move(ShaderBytecode);
      isChangeGraphicsPipelineState_ = true;
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
  //  データを保有する事でリファレンスカウントの維持
  ComputeCommandListDatas_[ComputeCommandBufferIndex_] = _ComputeCommandList;

  ComputeReserveData& CurrentComputeReserveData = GetCurrentComputeReserveData();

  //  コマンドリストの初期設定
  CommandList& CurrentComputeCommandList = GetCurrentComputeCommandList();
  {
    CurrentComputeCommandList.Reset(CurrentComputeReserveData.PipelineStates[0].Get());
    CurrentComputeCommandList->SetComputeRootSignature(pComputeRootSignature_.Get());
    CurrentComputeCommandList->SetDescriptorHeaps(kDescriptorHeapNum, ppDescriptorHeaps_.data());
  }
  CommandList& CurrentGraphicsCommandList = GetCurrentGraphicsCommandList();

  HRESULT hr = S_OK;

  //  コマンドの発行
  for (auto& ComputeCommand : _ComputeCommandList.GetComputeCommands())
  {
    switch (ComputeCommand)
    {
    case ComputeCommandFlag::eSetShader:
    {
      assert(!_ComputeCommandList.GetComputeShader().isEmpty());
      const CComputeShader* pComputeShader = Cast<CComputeShader>(pShaderManager_->GetShader(_ComputeCommandList.GetComputeShader().GetID()));

      ComputePipelineStateDesc_.CS = pComputeShader->ShaderData.ByteCode;

      Microsoft::WRL::ComPtr<ID3D12PipelineState>& pPipelineState = CurrentComputeReserveData.PipelineStates.emplace_back();
      hr = pD3D12Device_->CreateComputePipelineState(&ComputePipelineStateDesc_, IID_PPV_ARGS(pPipelineState.GetAddressOf()));
      ERROR_CHECK(hr);

      CurrentComputeCommandList->SetPipelineState(pPipelineState.Get());
    }
    break;
    case ComputeCommandFlag::eSetShaderResource:
    {
      const ShaderResources& ShaderResources = _ComputeCommandList.GetShaderResources();
      const vdl::uint ShaderResourceNum = static_cast<vdl::uint>(ShaderResources.size());

      if (ShaderResourceNum == 0)
      {
        continue;
      }

      //  データの読み込み
      std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> ShaderResourceDatas(ShaderResourceNum);
      {
        for (vdl::uint ShaderResourceCount = 0; ShaderResourceCount < ShaderResourceNum; ++ShaderResourceCount)
        {
          ShaderResourceDatas[ShaderResourceCount] = GetShaderResourceCPUHandle(ShaderResources[ShaderResourceCount], &CurrentGraphicsCommandList);
        }
      }

      constexpr DescriptorHeapType kDescriptorHeapType = DescriptorHeapType::eCBV_SRV_UAV;
      DescriptorAllocator& DescriptorAllocator = DescriptorAllocators_[static_cast<vdl::uint>(kDescriptorHeapType)];

      DescriptorHeap& DescriptorHeap = CurrentComputeReserveData.ShaderResourceHeaps.emplace_back();
      DescriptorAllocator.Allocate(&DescriptorHeap, ShaderResourceNum);

      pD3D12Device_->CopyDescriptors(1, &DescriptorHeap.CPUHandle, &ShaderResourceNum,
        ShaderResourceNum, ShaderResourceDatas.data(), nullptr, Cast(kDescriptorHeapType));
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

      //  データの読み込み
      std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> SamplerDatas(SamplerNum);
      {
        for (vdl::uint SamplerCount = 0; SamplerCount < SamplerNum; ++SamplerCount)
        {
          SamplerDatas[SamplerCount] = GetSamplerCPUHandle(Samplers[SamplerCount]);
        }
      }

      constexpr DescriptorHeapType kDescriptorHeapType = DescriptorHeapType::eSampler;
      DescriptorAllocator& DescriptorAllocator = DescriptorAllocators_[static_cast<vdl::uint>(kDescriptorHeapType)];

      DescriptorHeap& DescriptorHeap = CurrentComputeReserveData.SamplerHeaps.emplace_back();
      DescriptorAllocator.Allocate(&DescriptorHeap, SamplerNum);

      pD3D12Device_->CopyDescriptors(1, &DescriptorHeap.CPUHandle, &SamplerNum,
        SamplerNum, SamplerDatas.data(), nullptr, Cast(kDescriptorHeapType));
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

      //  データの読み込み
      std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> ConstantBufferDatas(ConstantBufferNum);
      {
        for (vdl::uint ConstantBufferCount = 0; ConstantBufferCount < ConstantBufferNum; ++ConstantBufferCount)
        {
          ConstantBufferDatas[ConstantBufferCount] = GetConstantBufferCPUHandle(ConstantBuffers[ConstantBufferCount]);
        }
      }

      constexpr DescriptorHeapType kDescriptorHeapType = DescriptorHeapType::eCBV_SRV_UAV;
      DescriptorAllocator& DescriptorAllocator = DescriptorAllocators_[static_cast<vdl::uint>(kDescriptorHeapType)];

      DescriptorHeap& DescriptorHeap = CurrentComputeReserveData.ConstantBufferHeaps.emplace_back();
      DescriptorAllocator.Allocate(&DescriptorHeap, ConstantBufferNum);

      pD3D12Device_->CopyDescriptors(1, &DescriptorHeap.CPUHandle, &ConstantBufferNum,
        ConstantBufferNum, ConstantBufferDatas.data(), nullptr, Cast(kDescriptorHeapType));
    }
    break;
    case ComputeCommandFlag::eSetUnorderedAccessObject:
    {
      const UnorderedAccessObjects& UnorderedAccessObjects = _ComputeCommandList.GetUnorderedAccessObjects();
      const vdl::uint UnorderedAccessObjectNum = static_cast<vdl::uint>(UnorderedAccessObjects.size());

      if (UnorderedAccessObjectNum == 0)
      {
        continue;
      }

      //  データの読み込み
      std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> UnorderedAccessObjectDatas(UnorderedAccessObjectNum);
      {
        for (vdl::uint UnorderedAccessObjectCount = 0; UnorderedAccessObjectCount < UnorderedAccessObjectNum; ++UnorderedAccessObjectCount)
        {
          UnorderedAccessObjectDatas[UnorderedAccessObjectCount] = GetUnorderedAccessObjectCPUHandle(UnorderedAccessObjects[UnorderedAccessObjectCount], &CurrentGraphicsCommandList);
        }
      }

      constexpr DescriptorHeapType kDescriptorHeapType = DescriptorHeapType::eCBV_SRV_UAV;
      DescriptorAllocator& DescriptorAllocator = DescriptorAllocators_[static_cast<vdl::uint>(kDescriptorHeapType)];

      DescriptorHeap& DescriptorHeap = CurrentComputeReserveData.UnorderedAccessHeaps.emplace_back();
      DescriptorAllocator.Allocate(&DescriptorHeap, UnorderedAccessObjectNum);

      pD3D12Device_->CopyDescriptors(1, &DescriptorHeap.CPUHandle, &UnorderedAccessObjectNum,
        UnorderedAccessObjectNum, UnorderedAccessObjectDatas.data(), nullptr, Cast(kDescriptorHeapType));
    }
    break;
    default: assert(false);
    }
  }

  Flush();

  //  SetDescriptor
  {
    if (const DescriptorHeap& DescriptorHeap = CurrentComputeReserveData.ShaderResourceHeaps.back();
      DescriptorHeap.Allocator)
    {
      constexpr DescriptorType kDescriptorType = DescriptorType::eShaderResource;
      CurrentComputeCommandList->SetComputeRootDescriptorTable(GetDescriptorOffset(ShaderType::eComputeShader, kDescriptorType), DescriptorHeap.GPUHandle);
    }

    if (const DescriptorHeap& DescriptorHeap = CurrentComputeReserveData.SamplerHeaps.back();
      DescriptorHeap.Allocator)
    {
      constexpr DescriptorType kDescriptorType = DescriptorType::eSampler;
      CurrentComputeCommandList->SetComputeRootDescriptorTable(GetDescriptorOffset(ShaderType::eComputeShader, kDescriptorType), DescriptorHeap.GPUHandle);
    }

    if (const DescriptorHeap& DescriptorHeap = CurrentComputeReserveData.ConstantBufferHeaps.back();
      DescriptorHeap.Allocator)
    {
      constexpr DescriptorType kDescriptorType = DescriptorType::eConstantBuffer;
      CurrentComputeCommandList->SetComputeRootDescriptorTable(GetDescriptorOffset(ShaderType::eComputeShader, kDescriptorType), DescriptorHeap.GPUHandle);
    }

    if (const DescriptorHeap& DescriptorHeap = CurrentComputeReserveData.UnorderedAccessHeaps.back();
      DescriptorHeap.Allocator)
    {
      constexpr DescriptorType kDescriptorType = DescriptorType::eUnorderedAccessObject;
      CurrentComputeCommandList->SetComputeRootDescriptorTable(GetDescriptorOffset(ShaderType::eComputeShader, kDescriptorType), DescriptorHeap.GPUHandle);
    }
  }

  const vdl::uint3& ThreadGroup = _ComputeCommandList.GetThreadGroup();
  CurrentComputeCommandList->Dispatch(ThreadGroup.x, ThreadGroup.y, ThreadGroup.z);
  CurrentComputeCommandList.Close();

  //  最後のコマンドをGPUで同期
  if (pLastFence_)
  {
    pLastFence_->Wait(pComputeCommandQueue_);
  }

  //  コマンドを発行
  CurrentComputeCommandList.Execute(pComputeCommandQueue_);

  //  最後のフェンスを更新してGPUで更新設定
  {
    pLastFence_ = &GetCurrentComputeFence();
    pLastFence_->Signal(pComputeCommandQueue_);
  }

  //  次のコマンドリストの設定
  {
    ++ComputeCommandBufferIndex_ %= Constants::kComputeCommandBufferNum;

    //  CPUで同期
    GetCurrentComputeFence().Wait(FenceEvent_);

    //  最終データの引き継ぎ
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
  }
}

void CDeviceContext::Flush()
{
  CommandList& CurrentGraphicsCommandList = GetCurrentGraphicsCommandList();
  CurrentGraphicsCommandList.Close();

  //  最後のコマンドをGPUで同期
  if (pLastFence_)
  {
    pLastFence_->Wait(pGraphicsCommandQueue_);
  }

  //  コマンドを発行
  CurrentGraphicsCommandList.Execute(pGraphicsCommandQueue_);

  //  最後のフェンスを更新してGPUで更新設定
  {
    pLastFence_ = &GetCurrentGraphicsFence();
    pLastFence_->Signal(pGraphicsCommandQueue_);
  }

  //  次のコマンドリストの設定
  {
    GraphicsReserveData& CurrentGraphicsReserveData = GetCurrentGraphicsReserveData();

    //  次のコマンドリストへ
    ++GraphicsCommandBufferIndex_ %= Constants::kGraphicsCommandBufferNum;

    //  CPUで同期
    GetCurrentGraphicsFence().Wait(FenceEvent_);

    //  最終データの引き継ぎ
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

    //  コマンドリストの初期設定  
    CommandList& NextGraphicsCommandList = GetCurrentGraphicsCommandList();
    {
      NextGraphicsCommandList.Reset(NextGraphicsReserveData.PipelineStates[0].Get());
      NextGraphicsCommandList->SetGraphicsRootSignature(pGraphicsRootSignature_.Get());
      NextGraphicsCommandList->SetDescriptorHeaps(kDescriptorHeapNum, ppDescriptorHeaps_.data());
    }
  }
}

void CDeviceContext::Present()
{
  SwapChain_.GetD3D12RenderTexture(CurrentBackBufferIndex_)->TextureData.TransitionResourceBarrier(&GetCurrentGraphicsCommandList(), D3D12_RESOURCE_STATE_PRESENT);

  Flush();

  HRESULT hr = S_OK;

  hr = SwapChain_->Present(0, 0);
  ERROR_CHECK(hr);

  ++CurrentBackBufferIndex_ %= Constants::kBackBufferNum;

  //  次のフレームの準備
  const vdl::RenderTextures& RenderTextures = SwapChain_.GetRenderTextures();
  const vdl::DepthStencilTexture& DepthStencilTexture = SwapChain_.GetDepthStencilTexture();
  pRenderer_->SetRenderTextures(RenderTextures, DepthStencilTexture);
  ClearRenderTexture(RenderTextures[0], pWindow_->GetScreenClearColor());
  ClearDepthStencilTexture(DepthStencilTexture, Constants::kDefaultClearDepth, Constants::kDefaultClearStencil);
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

CRenderTexture* CDeviceContext::GetD3D12RenderTexture(const vdl::RenderTexture& _RenderTexture)
{
  assert(!_RenderTexture.isEmpty());

  ITexture* pTexture = pTextureManager_->GetTexture(_RenderTexture.GetID());
  return (pTexture->GetType() == TextureType::eSwapChainRenderTexture ? SwapChain_.GetD3D12RenderTexture(CurrentBackBufferIndex_) : Cast<CRenderTexture>(pTexture));
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
      vdl::Color4F BorderColor = Cast(_Sampler.BorderColor);
      ::memcpy(SamplerDesc.BorderColor, &BorderColor, sizeof(float) * 4);
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
  switch (CurrentInputLayoutType_)
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
  switch (CurrentInputLayoutType_)
  {
  case vdl::InputLayoutType::eTexture: return sizeof(vdl::Instance2D);
  case vdl::InputLayoutType::eStaticMesh: return sizeof(vdl::InstanceStaticMesh);
  case vdl::InputLayoutType::eSkinnedMesh: return sizeof(vdl::InstanceSkinnedMesh);
  default: assert(false);
  }

  return 0;
}
