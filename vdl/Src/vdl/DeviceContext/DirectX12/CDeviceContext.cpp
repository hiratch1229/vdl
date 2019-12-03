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

#include <vdl/Vertex.hpp>
#include <ThirdParty/ImGui/imgui.h>

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

  inline vdl::ColorF Cast(vdl::BorderColorType _Type)
  {
    switch (_Type)
    {
    case vdl::BorderColorType::eTransparent:
      return vdl::ColorF(0.0f, 0.0f, 0.0f, 0.0f);
    case vdl::BorderColorType::eBlack:
      return vdl::ColorF(0.0f, 0.0f, 0.0f, 1.0f);
    case vdl::BorderColorType::eWhite:
      return vdl::ColorF(1.0f, 1.0f, 1.0f, 1.0f);
    default: assert(false);
    }

    return vdl::ColorF();
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

  struct DescriptorData
  {
    D3D12_CPU_DESCRIPTOR_HANDLE CPUDescriptorHandle;
    vdl::uint Bind;
  };
}

void CDeviceContext::Initialize()
{
  pDevice_ = static_cast<CDevice*>(Engine::Get<IDevice>());
  pSwapChain_ = static_cast<CSwapChain*>(Engine::Get<ISwapChain>());

  pTextureManager_ = Engine::Get<ITextureManager>();
  pBufferManager_ = Engine::Get<IBufferManager>();
  pShaderManager_ = Engine::Get<IShaderManager>();

  pD3D12Device_ = pDevice_->GetDevice();

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
  for (vdl::uint i = 0; i < kDescriptorHeapTypeNum; ++i)
  {
    DescriptorHeaps_[i].Initialize(pD3D12Device_, static_cast<DescriptorHeapType>(i));
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

  auto GetDescriptorRange = [kDescriptorRanges](DescriptorType _Type)->const D3D12_DESCRIPTOR_RANGE *
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
      _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
    }

    //  コマンドキューの作成
    {
      D3D12_COMMAND_QUEUE_DESC CommandQueueDesc;
      {
        CommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        CommandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
        CommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        CommandQueueDesc.NodeMask = 0;
      }

      hr = pD3D12Device_->CreateCommandQueue(&CommandQueueDesc, IID_PPV_ARGS(pGraphicsCommandQueue_.GetAddressOf()));
      _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
    }

    //  コマンドリストの作成
    {
      for (vdl::uint i = 0; i < Constants::kGraphicsCommandBufferNum; ++i)
      {
        //  コマンドアロケータの作成
        hr = pD3D12Device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(pGraphicsCommandAllocators_[i].GetAddressOf()));
        _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));

        //  コマンドリストの作成
        hr = pD3D12Device_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, pGraphicsCommandAllocators_[i].Get(), nullptr, IID_PPV_ARGS(pGraphicsCommandLists_[i].GetAddressOf()));
        _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));

        hr = pGraphicsCommandLists_[i]->Close();
        _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
      }

      hr = GetCurrentGraphicsCommandList()->Reset(GetCurrentGraphicsCommandAllocator(), nullptr);
      _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
    }

    //  フェンスの作成
    {
      for (auto& GraphicsSyncState : GraphicsSyncStates_)
      {
        hr = pD3D12Device_->CreateFence(GraphicsSyncState.Value, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(GraphicsSyncState.pFence.GetAddressOf()));
        _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
      }
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
      _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
    }

    //  コマンドキューの作成
    {
      D3D12_COMMAND_QUEUE_DESC CommandQueueDesc;
      {
        CommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
        CommandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
        CommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        CommandQueueDesc.NodeMask = 0;
      }

      hr = pD3D12Device_->CreateCommandQueue(&CommandQueueDesc, IID_PPV_ARGS(pComputeCommandQueue_.GetAddressOf()));
      _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
    }

    //  コマンドリストの作成
    {
      for (vdl::uint i = 0; i < Constants::kComputeCommandBufferNum; ++i)
      {
        //  コマンドアロケータの作成
        hr = pD3D12Device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COMPUTE, IID_PPV_ARGS(pComputeCommandAllocators_[i].GetAddressOf()));
        _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));

        //  コマンドリストの作成
        hr = pD3D12Device_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COMPUTE, pComputeCommandAllocators_[i].Get(), nullptr, IID_PPV_ARGS(pComputeCommandLists_[i].GetAddressOf()));
        _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));

        hr = pComputeCommandLists_[i]->Close();
        _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
      }
    }

    //  フェンスの作成
    {
      for (auto& ComputeSyncState : ComputeSyncStates_)
      {
        hr = pD3D12Device_->CreateFence(ComputeSyncState.Value, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(ComputeSyncState.pFence.GetAddressOf()));
        _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
      }
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

void CDeviceContext::ClearRenderTexture(const vdl::RenderTexture& _RenderTexture, const vdl::ColorF& _ClearColor)
{
  assert(!_RenderTexture.isEmpty());

  constexpr D3D12_RESOURCE_STATES kResourceState = D3D12_RESOURCE_STATE_RENDER_TARGET;

  ID3D12GraphicsCommandList* pCurrentGraphicsCommandList = GetCurrentGraphicsCommandList();

  CRenderTexture* pRenderTexture = GetD3D12RenderTexture(_RenderTexture);
  if (pRenderTexture->TextureData.ResourceState != kResourceState)
  {
    pRenderTexture->TextureData.TransitionResourceBarrier(pCurrentGraphicsCommandList, kResourceState);
  }
  pCurrentGraphicsCommandList->ClearRenderTargetView(pRenderTexture->pRenderTargetViewHeap->GetCPUDescriptorHandleForHeapStart(), &_ClearColor.Red, 0, nullptr);

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

  ID3D12GraphicsCommandList* pCurrentGraphicsCommandList = GetCurrentGraphicsCommandList();

  const vdl::ID TextureID = _DepthStencilTexture.GetID();
  CDepthStencilTexture* pDepthStencilTexture = Cast<CDepthStencilTexture>(pTextureManager_->GetTexture(TextureID));
  if (pDepthStencilTexture->TextureData.ResourceState != kResourceState)
  {
    pDepthStencilTexture->TextureData.TransitionResourceBarrier(pCurrentGraphicsCommandList, kResourceState);
  }
  pCurrentGraphicsCommandList->ClearDepthStencilView(pDepthStencilTexture->pDepthStencilViewHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, _ClearDepth, _ClearStencil, 0, nullptr);

  //  Clearするまで生存を保証
  {
    std::unordered_map<vdl::ID, Texture>& ClearTextures = GraphicsReserveDatas_[GraphicsCommandBufferIndex_].ClearTextures;

    if (ClearTextures.find(TextureID) == ClearTextures.end())
    {
      ClearTextures.insert(std::make_pair(TextureID, _DepthStencilTexture));
    }
  }
}

void CDeviceContext::ClearUnorderedAccessTexture(const vdl::UnorderedAccessTexture& _UnorderedAccessTexture, const vdl::ColorF& _ClearColor)
{
  assert(!_UnorderedAccessTexture.isEmpty());

  constexpr D3D12_RESOURCE_STATES kResourceState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;

  ID3D12GraphicsCommandList* pCurrentGraphicsCommandList = GetCurrentGraphicsCommandList();

  const vdl::ID TextureID = _UnorderedAccessTexture.GetID();
  CUnorderedAccessTexture* pUnorderedAccessTexture = Cast<CUnorderedAccessTexture>(pTextureManager_->GetTexture(TextureID));
  if (pUnorderedAccessTexture->TextureData.ResourceState != kResourceState)
  {
    pUnorderedAccessTexture->TextureData.TransitionResourceBarrier(pCurrentGraphicsCommandList, kResourceState);
  }
  pCurrentGraphicsCommandList->ClearUnorderedAccessViewFloat(pUnorderedAccessTexture->pUnorderedAccessViewHeap->GetGPUDescriptorHandleForHeapStart(),
    pUnorderedAccessTexture->pUnorderedAccessViewHeap->GetCPUDescriptorHandleForHeapStart(), pUnorderedAccessTexture->TextureData.pResource.Get(), &_ClearColor.Red, 0, nullptr);

  //  Clearするまで生存を保証
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

  ComputeReserveData* pCurrentComputeReserveData = GetCurrentComputeReserveData();

  ID3D12GraphicsCommandList* pCurrentComputeCommandList = GetCurrentComputeCommandList();
  pCurrentComputeCommandList->Reset(GetCurrentComputeCommandAllocator(), nullptr);
  _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));

  pCurrentComputeCommandList->SetComputeRootSignature(pComputeRootSignature_.Get());
  ID3D12DescriptorHeap* ppDescriptorHeaps[] = { DescriptorHeaps_[static_cast<vdl::uint>(DescriptorHeapType::eCBV_SRV_UAV)].GetDescriptorHeap(), DescriptorHeaps_[static_cast<vdl::uint>(DescriptorHeapType::eSampler)].GetDescriptorHeap() };
  pCurrentComputeCommandList->SetDescriptorHeaps(static_cast<vdl::uint>(vdl::Macro::ArraySize(ppDescriptorHeaps)), ppDescriptorHeaps);

  ID3D12GraphicsCommandList* pCurrentGraphicsCommandList = GetCurrentGraphicsCommandList();

  //  SetDescriptor
  {
    //  SetShaderResource
    {
      const vdl::uint ShaderResourceNum = static_cast<vdl::uint>(CurrentComputeState_.ShaderResources.size());

      //  データの読み込み
      std::vector<DescriptorData> ShaderResouceDatas;
      {
        for (vdl::uint ShaderResourceCount = 0; ShaderResourceCount < ShaderResourceNum; ++ShaderResourceCount)
        {
          const vdl::ShaderResource& ShaderResource = CurrentComputeState_.ShaderResources[ShaderResourceCount];

          ITexture* pTexture = nullptr;
          {
            if (std::get_if<vdl::Texture>(&ShaderResource))
            {
              const vdl::Texture& Texture = std::get<vdl::Texture>(ShaderResource);
              if (!Texture.isEmpty())
              {
                pTexture = pTextureManager_->GetTexture(Texture.GetID());
              }
            }
            else if (std::get_if<vdl::CubeTexture>(&ShaderResource))
            {
              const vdl::CubeTexture& CubeTexture = std::get<vdl::CubeTexture>(ShaderResource);
              if (!CubeTexture.isEmpty())
              {
                pTexture = pTextureManager_->GetTexture(CubeTexture.GetID());
              }
            }
          }

          //  Texture
          if (pTexture)
          {
            DescriptorData& ShaderResouceData = ShaderResouceDatas.emplace_back();
            {
              ShaderResouceData.Bind = ShaderResourceCount;
            }

            constexpr D3D12_RESOURCE_STATES kResourceState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

            switch (pTexture->GetType())
            {
            case TextureType::eDepthStencilTexture:
              assert(false);
            case TextureType::eDepthTexture:
            {
              CDepthTexture* pDepthTexture = Cast<CDepthTexture>(pTexture);
              if (pDepthTexture->pParent->TextureData.ResourceState != kResourceState)
              {
                pDepthTexture->pParent->TextureData.TransitionResourceBarrier(pCurrentGraphicsCommandList, kResourceState);
              }
              ShaderResouceData.CPUDescriptorHandle = pDepthTexture->pShaderResourceViewHeap->GetCPUDescriptorHandleForHeapStart();
            }
            break;
            case TextureType::eStencilTexture:
            {
              CStencilTexture* pStencilTexture = Cast<CStencilTexture>(pTexture);
              if (pStencilTexture->pParent->TextureData.ResourceState != kResourceState)
              {
                pStencilTexture->pParent->TextureData.TransitionResourceBarrier(pCurrentGraphicsCommandList, kResourceState);
              }
              ShaderResouceData.CPUDescriptorHandle = pStencilTexture->pShaderResourceViewHeap->GetCPUDescriptorHandleForHeapStart();
            }
            break;
            default:
            {
              CTexture* pColorTexture = Cast<CTexture>(pTexture);
              if (pColorTexture->TextureData.ResourceState != kResourceState)
              {
                pColorTexture->TextureData.TransitionResourceBarrier(pCurrentGraphicsCommandList, kResourceState);
              }
              ShaderResouceData.CPUDescriptorHandle = pColorTexture->TextureData.pShaderResourceViewHeap->GetCPUDescriptorHandleForHeapStart();
            }
            break;
            }
          }
          //  UnorderedAccessBuffer
          else if (std::get_if<vdl::Detail::UnorderedAccessBufferData>(&ShaderResource))
          {
            const vdl::Detail::UnorderedAccessBufferData& UnorderedAccessBuffer = std::get<vdl::Detail::UnorderedAccessBufferData>(ShaderResource);

            if (!UnorderedAccessBuffer.isEmpty())
            {
              CUnordererdAccessBuffer* pUnorderedAccessBuffer = Cast<CUnordererdAccessBuffer>(pBufferManager_->GetBuffer(UnorderedAccessBuffer.GetID()));

              DescriptorData& ShaderResouceData = ShaderResouceDatas.emplace_back();
              {
                ShaderResouceData.Bind = ShaderResourceCount;
                ShaderResouceData.CPUDescriptorHandle = pUnorderedAccessBuffer->pShaderResourceViewHeap->GetCPUDescriptorHandleForHeapStart();
              }
            }
          }
        }
      }

      if (!ShaderResouceDatas.empty())
      {
        constexpr DescriptorHeapType kDescriptorHeapType = DescriptorHeapType::eCBV_SRV_UAV;
        DescriptorHeap& DescriptorHeap = DescriptorHeaps_[static_cast<vdl::uint>(kDescriptorHeapType)];

        const vdl::uint DescriptorHeapOffset = DescriptorHeap.Secure(ShaderResourceNum);

        const D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHandle = DescriptorHeap.GetCPUDescriptorHandle(DescriptorHeapOffset);
        const vdl::uint DescriptorIncrementSize = DescriptorHeap.GetDescriptorIncrementSize();

        for (auto& ShaderResouceData : ShaderResouceDatas)
        {
          D3D12_CPU_DESCRIPTOR_HANDLE DestDescriptorHandle = DescriptorHandle;
          DestDescriptorHandle.ptr += DescriptorIncrementSize * ShaderResouceData.Bind;

          pD3D12Device_->CopyDescriptorsSimple(1, DestDescriptorHandle, ShaderResouceData.CPUDescriptorHandle, Cast(kDescriptorHeapType));
        }

        constexpr DescriptorType kDescriptorType = DescriptorType::eShaderResource;
        pCurrentComputeCommandList->SetComputeRootDescriptorTable(GetDescriptorOffset(ShaderType::eComputeShader, kDescriptorType), DescriptorHeap.GetGPUDescriptorHandle(DescriptorHeapOffset));

        pCurrentComputeReserveData->ShaderResources = CurrentComputeState_.ShaderResources;
      }
    }

    //  SetSampler
    {
      const vdl::uint SamplerNum = static_cast<vdl::uint>(CurrentComputeState_.Samplers.size());

      //  データの読み込み
      std::vector<DescriptorData> SamplerDatas;
      {
        for (vdl::uint SamplerCount = 0; SamplerCount < SamplerNum; ++SamplerCount)
        {
          DescriptorData& SamplerData = SamplerDatas.emplace_back();
          {
            SamplerData.CPUDescriptorHandle = GetSamplerDescriptorHeap(CurrentComputeState_.Samplers[SamplerCount])->GetCPUDescriptorHandleForHeapStart();
            SamplerData.Bind = SamplerCount;
          }
        }
      }

      if (!SamplerDatas.empty())
      {
        constexpr DescriptorHeapType kDescriptorHeapType = DescriptorHeapType::eSampler;
        DescriptorHeap& DescriptorHeap = DescriptorHeaps_[static_cast<vdl::uint>(kDescriptorHeapType)];
        const vdl::uint DescriptorHeapOffset = DescriptorHeap.Secure(SamplerNum);

        const D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHandle = DescriptorHeap.GetCPUDescriptorHandle(DescriptorHeapOffset);
        const vdl::uint DescriptorIncrementSize = DescriptorHeap.GetDescriptorIncrementSize();

        for (auto& SamplerData : SamplerDatas)
        {
          D3D12_CPU_DESCRIPTOR_HANDLE DestDescriptorHandle = DescriptorHandle;
          DestDescriptorHandle.ptr += DescriptorIncrementSize * SamplerData.Bind;

          pD3D12Device_->CopyDescriptorsSimple(1, DestDescriptorHandle, SamplerData.CPUDescriptorHandle, Cast(kDescriptorHeapType));
        }

        constexpr DescriptorType kDescriptorType = DescriptorType::eSampler;
        pCurrentComputeCommandList->SetComputeRootDescriptorTable(GetDescriptorOffset(ShaderType::eComputeShader, kDescriptorType), DescriptorHeap.GetGPUDescriptorHandle(DescriptorHeapOffset));
      }
    }

    //  SetConstantBuffer
    {
      const vdl::uint ConstantBufferNum = static_cast<vdl::uint>(CurrentComputeState_.ConstantBuffers.size());

      //  データの読み込み
      std::vector<DescriptorData> ConstantBufferDatas;
      {
        for (vdl::uint ConstantBufferCount = 0; ConstantBufferCount < ConstantBufferNum; ++ConstantBufferCount)
        {
          const vdl::Detail::ConstantBufferData& ConstantBuffer = CurrentComputeState_.ConstantBuffers[ConstantBufferCount];
          if (ConstantBuffer.isEmpty())
          {
            continue;
          }

          CCopyConstantBuffer* pConstantBuffer = Cast<CCopyConstantBuffer>(pBufferManager_->GetBuffer(ConstantBuffer.GetID()));

          DescriptorData& BufferData = ConstantBufferDatas.emplace_back();
          {
            BufferData.CPUDescriptorHandle = pConstantBuffer->pConstantBufferViewHeap->GetCPUDescriptorHandleForHeapStart();
            BufferData.Bind = ConstantBufferCount;
          }
        }
      }

      if (!ConstantBufferDatas.empty())
      {
        constexpr DescriptorHeapType kDescriptorHeapType = DescriptorHeapType::eCBV_SRV_UAV;
        DescriptorHeap& DescriptorHeap = DescriptorHeaps_[static_cast<vdl::uint>(kDescriptorHeapType)];
        const vdl::uint DescriptorHeapOffset = DescriptorHeap.Secure(ConstantBufferNum);

        const D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHandle = DescriptorHeap.GetCPUDescriptorHandle(DescriptorHeapOffset);
        const vdl::uint DescriptorIncrementSize = DescriptorHeap.GetDescriptorIncrementSize();

        for (auto& ConstantBufferData : ConstantBufferDatas)
        {
          D3D12_CPU_DESCRIPTOR_HANDLE DestDescriptorHandle = DescriptorHandle;
          DestDescriptorHandle.ptr += DescriptorIncrementSize * ConstantBufferData.Bind;

          pD3D12Device_->CopyDescriptorsSimple(1, DestDescriptorHandle, ConstantBufferData.CPUDescriptorHandle, Cast(kDescriptorHeapType));
        }

        constexpr DescriptorType kDescriptorType = DescriptorType::eConstantBuffer;
        pCurrentComputeCommandList->SetComputeRootDescriptorTable(GetDescriptorOffset(ShaderType::eComputeShader, kDescriptorType), DescriptorHeap.GetGPUDescriptorHandle(DescriptorHeapOffset));

        pCurrentComputeReserveData->ConstantBuffers = CurrentComputeState_.ConstantBuffers;
      }
    }

    //  SetUnorderedAccessObject
    {
      const vdl::uint UnorderedAccessObjectNum = static_cast<vdl::uint>(CurrentComputeState_.UnorderedAccessObjects.size());

      //  データの読み込み
      std::vector<DescriptorData> UnorderedAccessObjectDatas;
      {
        for (vdl::uint UnorderedAccessObjectCount = 0; UnorderedAccessObjectCount < UnorderedAccessObjectNum; ++UnorderedAccessObjectCount)
        {
          const vdl::UnorderedAccessObject& UnorderedAccessObject = CurrentComputeState_.UnorderedAccessObjects[UnorderedAccessObjectCount];

          //  UnorderedAccessTexture
          if (std::get_if<vdl::UnorderedAccessTexture>(&UnorderedAccessObject))
          {
            const vdl::UnorderedAccessTexture& UnorderedAccessTexture = std::get<vdl::UnorderedAccessTexture>(UnorderedAccessObject);
            if (!UnorderedAccessTexture.isEmpty())
            {
              DescriptorData& UnorderedAccessObjectData = UnorderedAccessObjectDatas.emplace_back();
              {
                UnorderedAccessObjectData.Bind = UnorderedAccessObjectCount;
              }

              constexpr D3D12_RESOURCE_STATES kResourceState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;

              CTexture* pTexture = Cast<CTexture>(pTextureManager_->GetTexture(UnorderedAccessTexture.GetID()));
              if (pTexture->TextureData.ResourceState != kResourceState)
              {
                pTexture->TextureData.TransitionResourceBarrier(pCurrentGraphicsCommandList, kResourceState);
              }
              UnorderedAccessObjectData.CPUDescriptorHandle = pTexture->TextureData.pShaderResourceViewHeap->GetCPUDescriptorHandleForHeapStart();
            }
          }
          //  UnorderedAccessBuffer
          else if (std::get_if<vdl::Detail::UnorderedAccessBufferData>(&UnorderedAccessObject))
          {
            const vdl::Detail::UnorderedAccessBufferData& UnorderedAccessBuffer = std::get<vdl::Detail::UnorderedAccessBufferData>(UnorderedAccessObject);
            if (!UnorderedAccessBuffer.isEmpty())
            {
              CUnordererdAccessBuffer* pUnorderedAccessBuffer = Cast<CUnordererdAccessBuffer>(pBufferManager_->GetBuffer(UnorderedAccessBuffer.GetID()));

              DescriptorData& UnorderedAccessObjectData = UnorderedAccessObjectDatas.emplace_back();
              {
                UnorderedAccessObjectData.Bind = UnorderedAccessObjectCount;
                UnorderedAccessObjectData.CPUDescriptorHandle = pUnorderedAccessBuffer->pShaderResourceViewHeap->GetCPUDescriptorHandleForHeapStart();
              }
            }
          }
        }
      }

      if (!UnorderedAccessObjectDatas.empty())
      {
        constexpr DescriptorHeapType kDescriptorHeapType = DescriptorHeapType::eCBV_SRV_UAV;
        DescriptorHeap& DescriptorHeap = DescriptorHeaps_[static_cast<vdl::uint>(kDescriptorHeapType)];
        const vdl::uint DescriptorHeapOffset = DescriptorHeap.Secure(UnorderedAccessObjectNum);

        const D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHandle = DescriptorHeap.GetCPUDescriptorHandle(DescriptorHeapOffset);
        const vdl::uint DescriptorIncrementSize = DescriptorHeap.GetDescriptorIncrementSize();

        for (auto& UnorderedAccessObjectData : UnorderedAccessObjectDatas)
        {
          D3D12_CPU_DESCRIPTOR_HANDLE DestDescriptorHandle = DescriptorHandle;
          DestDescriptorHandle.ptr += DescriptorIncrementSize * UnorderedAccessObjectData.Bind;

          pD3D12Device_->CopyDescriptorsSimple(1, DestDescriptorHandle, UnorderedAccessObjectData.CPUDescriptorHandle, Cast(kDescriptorHeapType));
        }

        constexpr DescriptorType kDescriptorType = DescriptorType::eUnorderedAccessObject;
        pCurrentComputeCommandList->SetComputeRootDescriptorTable(GetDescriptorOffset(ShaderType::eComputeShader, kDescriptorType), DescriptorHeap.GetGPUDescriptorHandle(DescriptorHeapOffset));

        pCurrentComputeReserveData->UnorderedAccessObjects = CurrentComputeState_.UnorderedAccessObjects;
      }
    }
  }

  Flush();

  //  SetPipeline
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
    _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));

    pCurrentComputeCommandList->SetPipelineState(pPipelineState.Get());

    pCurrentComputeReserveData->PipelineStates.emplace_back(std::move(pPipelineState));
  }

  pCurrentComputeCommandList->Dispatch(_ThreadGroupX, _ThreadGroupY, _ThreadGroupZ);

  pCurrentComputeCommandList->Close();

  ID3D12CommandList* pCurrentCommandList = pCurrentComputeCommandList;
  pComputeCommandQueue_->ExecuteCommandLists(1, &pCurrentCommandList);

  SingnalFence(pComputeCommandQueue_.Get(), &ComputeSyncStates_[ComputeCommandBufferIndex_]);
  ++ComputeCommandBufferIndex_ %= Constants::kComputeCommandBufferNum;
  WaitFence(pComputeCommandQueue_.Get(), &ComputeSyncStates_[ComputeCommandBufferIndex_]);
  pLastComputeSyncState_ = &ComputeSyncStates_[ComputeCommandBufferIndex_];

  ComputeReserveDatas_[ComputeCommandBufferIndex_].Clear();
  ComputeStateChangeFlags_.Clear();
}

void CDeviceContext::Flush()
{
  HRESULT hr = S_OK;

  ID3D12GraphicsCommandList* pCurrentGraphicsCommandList = GetCurrentGraphicsCommandList();
  pCurrentGraphicsCommandList->Close();

  //  コンピュートパイプラインの待機
  if (pLastComputeSyncState_)
  {
    WaitFence(pComputeCommandQueue_.Get(), pLastComputeSyncState_);
    pLastComputeSyncState_ = nullptr;
  }

  ID3D12CommandList* pCurrentCommandList = pCurrentGraphicsCommandList;
  pGraphicsCommandQueue_->ExecuteCommandLists(1, &pCurrentCommandList);

  SingnalFence(pGraphicsCommandQueue_.Get(), &GraphicsSyncStates_[GraphicsCommandBufferIndex_]);
  ++GraphicsCommandBufferIndex_ %= Constants::kGraphicsCommandBufferNum;
  WaitFence(pGraphicsCommandQueue_.Get(), &GraphicsSyncStates_[GraphicsCommandBufferIndex_]);

  GraphicsReserveData* pCurrentGraphicsReseveData = GetCurrentGraphicsReserveData();

  pCurrentGraphicsReseveData->Clear();
  pCurrentGraphicsReseveData->PipelineStates.resize(1);
  pCurrentGraphicsReseveData->PipelineStates[0] = std::move(pCurrentGraphicsReseveData->PipelineStates.back());

  hr = GetCurrentGraphicsCommandList()->Reset(GetCurrentGraphicsCommandAllocator(), pCurrentGraphicsReseveData->PipelineStates[0].Get());
  _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
  GraphicsCommandListState_ = CommandListState::eIdle;
}

//--------------------------------------------------

void CDeviceContext::Present()
{
  pSwapChain_->GetD3D12RenderTexture()->TextureData.TransitionResourceBarrier(GetCurrentGraphicsCommandList(), D3D12_RESOURCE_STATE_PRESENT);

  Flush();

  HRESULT hr = S_OK;

  hr = pSwapChain_->GetSwapChain()->Present(0, 0);
  _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
}

//--------------------------------------------------

void CDeviceContext::PreprocessingDraw()
{
  HRESULT hr = S_OK;

  ID3D12GraphicsCommandList* pCurrentGraphicsCommandList = GetCurrentGraphicsCommandList();

  if (GraphicsCommandListState_ == CommandListState::eIdle)
  {
    pCurrentGraphicsCommandList->SetGraphicsRootSignature(pGraphicsRootSignature_.Get());
    ID3D12DescriptorHeap* ppDescriptorHeaps[] = { DescriptorHeaps_[static_cast<vdl::uint>(DescriptorHeapType::eCBV_SRV_UAV)].GetDescriptorHeap(), DescriptorHeaps_[static_cast<vdl::uint>(DescriptorHeapType::eSampler)].GetDescriptorHeap() };
    pCurrentGraphicsCommandList->SetDescriptorHeaps(static_cast<vdl::uint>(vdl::Macro::ArraySize(ppDescriptorHeaps)), ppDescriptorHeaps);
    GraphicsCommandListState_ = CommandListState::eBegin;
  }

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
    _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));

    pCurrentGraphicsCommandList->SetPipelineState(pPipelineState.Get());

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

        //  データの読み込み
        std::vector<DescriptorData> ShaderResouceDatas;
        {
          for (vdl::uint ShaderResourceCount = 0; ShaderResourceCount < ShaderResourceNum; ++ShaderResourceCount)
          {
            const vdl::ShaderResource& ShaderResource = ShaderResources[ShaderResourceCount];

            ITexture* pTexture = nullptr;
            {
              if (std::get_if<vdl::Texture>(&ShaderResource))
              {
                const vdl::Texture& Texture = std::get<vdl::Texture>(ShaderResource);
                if (!Texture.isEmpty())
                {
                  pTexture = pTextureManager_->GetTexture(Texture.GetID());
                }
              }
              else if (std::get_if<vdl::CubeTexture>(&ShaderResource))
              {
                const vdl::CubeTexture& CubeTexture = std::get<vdl::CubeTexture>(ShaderResource);
                if (!CubeTexture.isEmpty())
                {
                  pTexture = pTextureManager_->GetTexture(CubeTexture.GetID());
                }
              }
            }

            //  Texture
            if (pTexture)
            {
              DescriptorData& ShaderResouceData = ShaderResouceDatas.emplace_back();
              {
                ShaderResouceData.Bind = ShaderResourceCount;
              }

              constexpr D3D12_RESOURCE_STATES kResourceState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

              switch (pTexture->GetType())
              {
              case TextureType::eDepthStencilTexture:
                assert(false);
              case TextureType::eDepthTexture:
              {
                CDepthTexture* pDepthTexture = Cast<CDepthTexture>(pTexture);
                if (pDepthTexture->pParent->TextureData.ResourceState != kResourceState)
                {
                  pDepthTexture->pParent->TextureData.TransitionResourceBarrier(pCurrentGraphicsCommandList, kResourceState);
                }
                ShaderResouceData.CPUDescriptorHandle = pDepthTexture->pShaderResourceViewHeap->GetCPUDescriptorHandleForHeapStart();
              }
              break;
              case TextureType::eStencilTexture:
              {
                CStencilTexture* pStencilTexture = Cast<CStencilTexture>(pTexture);
                if (pStencilTexture->pParent->TextureData.ResourceState != kResourceState)
                {
                  pStencilTexture->pParent->TextureData.TransitionResourceBarrier(pCurrentGraphicsCommandList, kResourceState);
                }
                ShaderResouceData.CPUDescriptorHandle = pStencilTexture->pShaderResourceViewHeap->GetCPUDescriptorHandleForHeapStart();
              }
              break;
              default:
              {
                CTexture* pColorTexture = Cast<CTexture>(pTexture);
                if (pColorTexture->TextureData.ResourceState != kResourceState)
                {
                  pColorTexture->TextureData.TransitionResourceBarrier(pCurrentGraphicsCommandList, kResourceState);
                }
                ShaderResouceData.CPUDescriptorHandle = pColorTexture->TextureData.pShaderResourceViewHeap->GetCPUDescriptorHandleForHeapStart();
              }
              break;
              }
            }
            //  UnorderedAccessBuffer
            else if (std::get_if<vdl::Detail::UnorderedAccessBufferData>(&ShaderResource))
            {
              const vdl::Detail::UnorderedAccessBufferData& UnorderedAccessBuffer = std::get<vdl::Detail::UnorderedAccessBufferData>(ShaderResource);

              if (!UnorderedAccessBuffer.isEmpty())
              {
                CUnordererdAccessBuffer* pUnorderedAccessBuffer = Cast<CUnordererdAccessBuffer>(pBufferManager_->GetBuffer(UnorderedAccessBuffer.GetID()));

                DescriptorData& ShaderResouceData = ShaderResouceDatas.emplace_back();
                {
                  ShaderResouceData.Bind = ShaderResourceCount;
                  ShaderResouceData.CPUDescriptorHandle = pUnorderedAccessBuffer->pShaderResourceViewHeap->GetCPUDescriptorHandleForHeapStart();
                }
              }
            }
          }
        }

        if (ShaderResouceDatas.empty())
        {
          return;
        }

        constexpr DescriptorHeapType kDescriptorHeapType = DescriptorHeapType::eCBV_SRV_UAV;
        DescriptorHeap& DescriptorHeap = DescriptorHeaps_[static_cast<vdl::uint>(kDescriptorHeapType)];

        const vdl::uint DescriptorHeapOffset = DescriptorHeap.Secure(ShaderResourceNum);

        const D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHandle = DescriptorHeap.GetCPUDescriptorHandle(DescriptorHeapOffset);
        const vdl::uint DescriptorIncrementSize = DescriptorHeap.GetDescriptorIncrementSize();

        for (auto& ShaderResouceData : ShaderResouceDatas)
        {
          D3D12_CPU_DESCRIPTOR_HANDLE DestDescriptorHandle = DescriptorHandle;
          DestDescriptorHandle.ptr += DescriptorIncrementSize * ShaderResouceData.Bind;

          pD3D12Device_->CopyDescriptorsSimple(1, DestDescriptorHandle, ShaderResouceData.CPUDescriptorHandle, Cast(kDescriptorHeapType));
        }

        constexpr DescriptorType kDescriptorType = DescriptorType::eShaderResource;
        pCurrentGraphicsCommandList->SetGraphicsRootDescriptorTable(GetDescriptorOffset(_Type, kDescriptorType), DescriptorHeap.GetGPUDescriptorHandle(DescriptorHeapOffset));

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

        //  データの読み込み
        std::vector<DescriptorData> SamplerDatas;
        {
          for (vdl::uint SamplerCount = 0; SamplerCount < SamplerNum; ++SamplerCount)
          {
            DescriptorData& SamplerData = SamplerDatas.emplace_back();
            {
              SamplerData.CPUDescriptorHandle = GetSamplerDescriptorHeap(Samplers[SamplerCount])->GetCPUDescriptorHandleForHeapStart();
              SamplerData.Bind = SamplerCount;
            }
          }
        }

        if (SamplerDatas.empty())
        {
          return;
        }

        constexpr DescriptorHeapType kDescriptorHeapType = DescriptorHeapType::eSampler;
        DescriptorHeap& DescriptorHeap = DescriptorHeaps_[static_cast<vdl::uint>(kDescriptorHeapType)];
        const vdl::uint DescriptorHeapOffset = DescriptorHeap.Secure(SamplerNum);

        const D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHandle = DescriptorHeap.GetCPUDescriptorHandle(DescriptorHeapOffset);
        const vdl::uint DescriptorIncrementSize = DescriptorHeap.GetDescriptorIncrementSize();

        for (auto& SamplerData : SamplerDatas)
        {
          D3D12_CPU_DESCRIPTOR_HANDLE DestDescriptorHandle = DescriptorHandle;
          DestDescriptorHandle.ptr += DescriptorIncrementSize * SamplerData.Bind;

          pD3D12Device_->CopyDescriptorsSimple(1, DestDescriptorHandle, SamplerData.CPUDescriptorHandle, Cast(kDescriptorHeapType));
        }

        constexpr DescriptorType kDescriptorType = DescriptorType::eSampler;
        pCurrentGraphicsCommandList->SetGraphicsRootDescriptorTable(GetDescriptorOffset(_Type, kDescriptorType), DescriptorHeap.GetGPUDescriptorHandle(DescriptorHeapOffset));
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

        //  データの読み込み
        std::vector<DescriptorData> ConstantBufferDatas;
        {
          for (vdl::uint ConstantBufferCount = 0; ConstantBufferCount < ConstantBufferNum; ++ConstantBufferCount)
          {
            if (ConstantBuffers[ConstantBufferCount].isEmpty())
            {
              continue;
            }

            CCopyConstantBuffer* pConstantBuffer = Cast<CCopyConstantBuffer>(pBufferManager_->GetBuffer(ConstantBuffers[ConstantBufferCount].GetID()));

            DescriptorData& BufferData = ConstantBufferDatas.emplace_back();
            {
              BufferData.CPUDescriptorHandle = pConstantBuffer->pConstantBufferViewHeap->GetCPUDescriptorHandleForHeapStart();
              BufferData.Bind = ConstantBufferCount;
            }
          }
        }

        if (ConstantBufferDatas.empty())
        {
          return;
        }

        constexpr DescriptorHeapType kDescriptorHeapType = DescriptorHeapType::eCBV_SRV_UAV;
        DescriptorHeap& DescriptorHeap = DescriptorHeaps_[static_cast<vdl::uint>(kDescriptorHeapType)];
        const vdl::uint DescriptorHeapOffset = DescriptorHeap.Secure(ConstantBufferNum);

        const D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHandle = DescriptorHeap.GetCPUDescriptorHandle(DescriptorHeapOffset);
        const vdl::uint DescriptorIncrementSize = DescriptorHeap.GetDescriptorIncrementSize();

        for (auto& ConstantBufferData : ConstantBufferDatas)
        {
          D3D12_CPU_DESCRIPTOR_HANDLE DestDescriptorHandle = DescriptorHandle;
          DestDescriptorHandle.ptr += DescriptorIncrementSize * ConstantBufferData.Bind;

          pD3D12Device_->CopyDescriptorsSimple(1, DestDescriptorHandle, ConstantBufferData.CPUDescriptorHandle, Cast(kDescriptorHeapType));
        }

        constexpr DescriptorType kDescriptorType = DescriptorType::eConstantBuffer;
        pCurrentGraphicsCommandList->SetGraphicsRootDescriptorTable(GetDescriptorOffset(_Type, kDescriptorType), DescriptorHeap.GetGPUDescriptorHandle(DescriptorHeapOffset));

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

    pCurrentGraphicsCommandList->IASetVertexBuffers(0, 1, &BufferView);

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

    pCurrentGraphicsCommandList->IASetVertexBuffers(1, 1, &BufferView);

    CurrentGraphicsReserveData.InstanceBuffers.push_back(CurrentGraphicsState_.InstanceBuffer);
  }

  if (GraphicsStateChangeFlags_.Has(GraphicsCommandType::eSetIndexBuffer) && !CurrentGraphicsState_.IndexBuffer.isEmpty())
  {
    const CIndexBuffer* pIndexBuffer = Cast<CIndexBuffer>(pBufferManager_->GetBuffer(CurrentGraphicsState_.IndexBuffer.GetID()));

    pCurrentGraphicsCommandList->IASetIndexBuffer(&pIndexBuffer->View);

    CurrentGraphicsReserveData.IndexBuffers.push_back(CurrentGraphicsState_.IndexBuffer);
  }

  if (GraphicsStateChangeFlags_.Has(GraphicsCommandType::eSetTopology))
  {
    pCurrentGraphicsCommandList->IASetPrimitiveTopology(Cast(CurrentGraphicsState_.Topology));
  }

  if (GraphicsStateChangeFlags_.Has(GraphicsCommandType::eSetScissor))
  {
    pCurrentGraphicsCommandList->RSSetScissorRects(1, &Cast(CurrentGraphicsState_.RasterizerState.ScissorEnable ? CurrentGraphicsState_.Scissor
      : vdl::Scissor(CurrentGraphicsState_.Viewport.LeftTop, CurrentGraphicsState_.Viewport.Size)));
  }

  if (GraphicsStateChangeFlags_.Has(GraphicsCommandType::eSetViewport))
  {
    pCurrentGraphicsCommandList->RSSetViewports(1, &Cast(CurrentGraphicsState_.Viewport));
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
          pRenderTexture->TextureData.TransitionResourceBarrier(pCurrentGraphicsCommandList, kResourceState);
        }

        RenderTargetDescriptors[RenderTextureCount] = pRenderTexture->pRenderTargetViewHeap->GetCPUDescriptorHandleForHeapStart();
      }
    }

    if (OutputManager.DepthStencilTexture.isEmpty())
    {
      pCurrentGraphicsCommandList->OMSetRenderTargets(GraphicsRenderTargetCount_, RenderTargetDescriptors.data(), false, nullptr);
    }
    else
    {
      constexpr D3D12_RESOURCE_STATES kResourceState = D3D12_RESOURCE_STATE_DEPTH_WRITE;

      CDepthStencilTexture* pDepthStencilTexture = Cast<CDepthStencilTexture>(pTextureManager_->GetTexture(OutputManager.DepthStencilTexture.GetID()));
      if (pDepthStencilTexture->TextureData.ResourceState != kResourceState)
      {
        pDepthStencilTexture->TextureData.TransitionResourceBarrier(pCurrentGraphicsCommandList, kResourceState);
      }

      pCurrentGraphicsCommandList->OMSetRenderTargets(GraphicsRenderTargetCount_, RenderTargetDescriptors.data(), false,
        &pDepthStencilTexture->pDepthStencilViewHeap->GetCPUDescriptorHandleForHeapStart());
    }
  }

  GraphicsStateChangeFlags_.Clear();
}

void CDeviceContext::SingnalFence(ID3D12CommandQueue* _pQueue, SyncState* _pSyncState)
{
  _pQueue->Signal(_pSyncState->pFence.Get(), ++_pSyncState->Value);
}

void CDeviceContext::WaitFence(ID3D12CommandQueue* _pQueue, SyncState* _pSyncState)
{
  if (_pSyncState->pFence->GetCompletedValue() < _pSyncState->Value)
  {
    _pSyncState->pFence->SetEventOnCompletion(_pSyncState->Value, FenceEvent_);
    ::WaitForSingleObjectEx(FenceEvent_, INFINITE, false);
  }
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

ID3D12DescriptorHeap* CDeviceContext::GetSamplerDescriptorHeap(const vdl::Sampler& _Sampler)
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

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> pDescriptorHeap;
    pDevice_->CreateDescriptorHeap(pDescriptorHeap.GetAddressOf(), Cast(DescriptorHeapType::eSampler));

    pD3D12Device_->CreateSampler(&SamplerDesc, pDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

    Samplers_.insert(std::make_pair(_Sampler, std::move(pDescriptorHeap)));
  }

  return Samplers_.at(_Sampler).Get();
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
