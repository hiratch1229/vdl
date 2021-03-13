#include "CDeviceContext.hpp"

#include <vdl/Engine.hpp>
#include <vdl/Device/DirectX11/CDevice.hpp>
#include <vdl/Window/IWindow.hpp>
#include <vdl/TextureManager/ITextureManager.hpp>
#include <vdl/BufferManager/IBufferManager.hpp>
#include <vdl/ShaderManager/IShaderManager.hpp>
#include <vdl/Renderer/IRenderer.hpp>
#include <vdl/CommandList/GraphicsCommandList/GraphicsCommandList.hpp>
#include <vdl/CommandList/ComputeCommandList/ComputeCommandList.hpp>

#include <vdl/Topology/DirectX/Topology.hpp>
#include <vdl/Scissor/DirectX/Scissor.hpp>
#include <vdl/Viewport/DirectX11/Viewport.hpp>
#include <vdl/Buffer/DirectX11/CBuffer.hpp>
#include <vdl/Texture/DirectX11/CTexture.hpp>
#include <vdl/Shader/DirectX11/CShader.hpp>

#include <vdl/Constants/Constants.hpp>
#include <vdl/Misc/Windows/Misc.hpp>

#include <ThirdParty/ImGui/imgui.h>

#include <vdl/Texture.hpp>
#include <vdl/Shader.hpp>
#include <vdl/Vertex.hpp>
#include <vdl/DetectMemoryLeak.hpp>

#include <assert.h>

namespace
{
  inline D3D11_BLEND Cast(vdl::BlendType _Type)
  {
    switch (_Type)
    {
    case vdl::BlendType::eZero:
      return D3D11_BLEND_ZERO;
    case vdl::BlendType::eOne:
      return D3D11_BLEND_ONE;
    case vdl::BlendType::eSrcColor:
      return D3D11_BLEND_SRC_COLOR;
    case vdl::BlendType::eInvSrcColor:
      return D3D11_BLEND_INV_SRC_COLOR;
    case vdl::BlendType::eSrcAlpha:
      return D3D11_BLEND_SRC_ALPHA;
    case vdl::BlendType::eInvSrcAlpha:
      return D3D11_BLEND_INV_SRC_ALPHA;
    case vdl::BlendType::eDestAlpha:
      return D3D11_BLEND_DEST_ALPHA;
    case vdl::BlendType::eInvDestAlpha:
      return D3D11_BLEND_INV_DEST_ALPHA;
    case vdl::BlendType::eDestColor:
      return D3D11_BLEND_DEST_COLOR;
    case vdl::BlendType::eInvDestColor:
      return D3D11_BLEND_INV_DEST_COLOR;
    case vdl::BlendType::eSrcAlphaSat:
      return D3D11_BLEND_SRC_ALPHA_SAT;
    case vdl::BlendType::eSrc1Color:
      return D3D11_BLEND_SRC1_COLOR;
    case vdl::BlendType::eInvSrc1Color:
      return D3D11_BLEND_INV_SRC1_COLOR;
    case vdl::BlendType::eSrc1Alpha:
      return D3D11_BLEND_SRC1_ALPHA;
    case vdl::BlendType::eInvSrc1Alpha:
      return D3D11_BLEND_INV_SRC1_ALPHA;
    default: assert(false);
    }

    return D3D11_BLEND_ZERO;
  }

  inline D3D11_BLEND_OP Cast(vdl::BlendOpType _Type)
  {
    switch (_Type)
    {
    case vdl::BlendOpType::eAdd:
      return D3D11_BLEND_OP_ADD;
    case vdl::BlendOpType::eSubtract:
      return D3D11_BLEND_OP_SUBTRACT;
    case vdl::BlendOpType::eRevSubtract:
      return D3D11_BLEND_OP_REV_SUBTRACT;
    case vdl::BlendOpType::eMin:
      return D3D11_BLEND_OP_MIN;
    case vdl::BlendOpType::eMax:
      return D3D11_BLEND_OP_MAX;
    default: assert(false);
    }

    return D3D11_BLEND_OP_ADD;
  }

  inline D3D11_COLOR_WRITE_ENABLE Cast(vdl::ColorWriteEnableType _Type)
  {
    switch (_Type)
    {
    case vdl::ColorWriteEnableType::eRed:
      return D3D11_COLOR_WRITE_ENABLE_RED;
    case vdl::ColorWriteEnableType::eGreen:
      return D3D11_COLOR_WRITE_ENABLE_GREEN;
    case vdl::ColorWriteEnableType::eBlue:
      return D3D11_COLOR_WRITE_ENABLE_BLUE;
    case vdl::ColorWriteEnableType::eAlpha:
      return D3D11_COLOR_WRITE_ENABLE_ALPHA;
    case vdl::ColorWriteEnableType::eAll:
      return D3D11_COLOR_WRITE_ENABLE_ALL;
    default: assert(false);
    }

    return D3D11_COLOR_WRITE_ENABLE_RED;
  }

  inline D3D11_DEPTH_WRITE_MASK Cast(vdl::DepthWriteMaskType _Type)
  {
    switch (_Type)
    {
    case vdl::DepthWriteMaskType::eZero:
      return D3D11_DEPTH_WRITE_MASK_ZERO;
    case vdl::DepthWriteMaskType::eAll:
      return D3D11_DEPTH_WRITE_MASK_ALL;
    default: assert(false);
    }

    return D3D11_DEPTH_WRITE_MASK_ZERO;
  }

  inline D3D11_COMPARISON_FUNC Cast(vdl::ComparisonFuncType _Type)
  {
    switch (_Type)
    {
    case vdl::ComparisonFuncType::eNever:
      return D3D11_COMPARISON_NEVER;
    case vdl::ComparisonFuncType::eLess:
      return D3D11_COMPARISON_LESS;
    case vdl::ComparisonFuncType::eEqual:
      return D3D11_COMPARISON_EQUAL;
    case vdl::ComparisonFuncType::eLessEqual:
      return D3D11_COMPARISON_LESS_EQUAL;
    case vdl::ComparisonFuncType::eGreater:
      return D3D11_COMPARISON_GREATER;
    case vdl::ComparisonFuncType::eNotEqual:
      return D3D11_COMPARISON_NOT_EQUAL;
    case vdl::ComparisonFuncType::eGreaterEqual:
      return D3D11_COMPARISON_GREATER_EQUAL;
    case vdl::ComparisonFuncType::eAlways:
      return D3D11_COMPARISON_ALWAYS;
    default: assert(false);
    }

    return D3D11_COMPARISON_NEVER;
  }

  inline D3D11_STENCIL_OP Cast(vdl::StencilOpType _Type)
  {
    switch (_Type)
    {
    case vdl::StencilOpType::eKeep:
      return D3D11_STENCIL_OP_KEEP;
    case vdl::StencilOpType::eZero:
      return D3D11_STENCIL_OP_ZERO;
    case vdl::StencilOpType::eRreplace:
      return D3D11_STENCIL_OP_REPLACE;
    case vdl::StencilOpType::eIncrSat:
      return D3D11_STENCIL_OP_INCR_SAT;
    case vdl::StencilOpType::eDecrSat:
      return D3D11_STENCIL_OP_DECR_SAT;
    case vdl::StencilOpType::eInvert:
      return D3D11_STENCIL_OP_INVERT;
    case vdl::StencilOpType::eIncr:
      return D3D11_STENCIL_OP_INCR;
    case vdl::StencilOpType::eDecr:
      return D3D11_STENCIL_OP_DECR;
    default: assert(false);
    }

    return D3D11_STENCIL_OP_KEEP;
  }

  inline D3D11_CULL_MODE Cast(vdl::CullModeType _Type)
  {
    switch (_Type)
    {
    case vdl::CullModeType::eNone:
      return D3D11_CULL_NONE;
    case vdl::CullModeType::eFront:
      return D3D11_CULL_FRONT;
    case vdl::CullModeType::eBack:
      return D3D11_CULL_BACK;
    default: assert(false);
    }

    return D3D11_CULL_NONE;
  }

  inline D3D11_FILL_MODE Cast(vdl::FillModeType _Type)
  {
    switch (_Type)
    {
    case vdl::FillModeType::eWireframe:
      return D3D11_FILL_WIREFRAME;
    case vdl::FillModeType::eSolid:
      return D3D11_FILL_SOLID;
    default: assert(false);
    }

    return D3D11_FILL_WIREFRAME;
  }

  inline D3D11_FILTER Cast(vdl::FilterType _Type)
  {
    switch (_Type)
    {
    case vdl::FilterType::eMinMagMipPoint:
      return D3D11_FILTER_MIN_MAG_MIP_POINT;
    case vdl::FilterType::eMinMagPointMipLinear:
      return D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
    case vdl::FilterType::eMinPointMagLinearMipPoint:
      return D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
    case vdl::FilterType::eMinPointMagMipLinear:
      return D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;
    case vdl::FilterType::eMinLinearMagMipPoint:
      return D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
    case vdl::FilterType::eMinLinearMagPointMipLinear:
      return D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
    case vdl::FilterType::eMinMagLinearMipPoint:
      return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
    case vdl::FilterType::eMinMagMipLinear:
      return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    case vdl::FilterType::eAnisotropic:
      return D3D11_FILTER_ANISOTROPIC;
    case vdl::FilterType::eComparisonMinMagMipPoint:
      return D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
    case vdl::FilterType::eComparisonMinMagPointMipLinear:
      return D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR;
    case vdl::FilterType::eComparisonMinPointMagLinearMipPoint:
      return D3D11_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT;
    case vdl::FilterType::eComparisonMinPointMagMipLinear:
      return D3D11_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR;
    case vdl::FilterType::eComparisonMinLinearMagMipPoint:
      return D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT;
    case vdl::FilterType::eComparisonMinLinearMagPointMipLinear:
      return D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
    case vdl::FilterType::eComparisonMinMagLinearMipPoint:
      return D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
    case vdl::FilterType::eComparisonMinMagMipLinear:
      return D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
    case vdl::FilterType::eComparisonAnisotropic:
      return D3D11_FILTER_COMPARISON_ANISOTROPIC;
    default: assert(false);
    }

    return D3D11_FILTER_MIN_MAG_MIP_POINT;
  }

  inline D3D11_TEXTURE_ADDRESS_MODE Cast(vdl::AddressModeType _Type)
  {
    switch (_Type)
    {
    case vdl::AddressModeType::eWrap:
      return D3D11_TEXTURE_ADDRESS_WRAP;
    case vdl::AddressModeType::eMirror:
      return D3D11_TEXTURE_ADDRESS_MIRROR;
    case vdl::AddressModeType::eClamp:
      return D3D11_TEXTURE_ADDRESS_CLAMP;
    case vdl::AddressModeType::eBorder:
      return D3D11_TEXTURE_ADDRESS_BORDER;
    case vdl::AddressModeType::eMirrorOnce:
      return D3D11_TEXTURE_ADDRESS_MIRROR_ONCE;
    default: assert(false);
    }

    return D3D11_TEXTURE_ADDRESS_WRAP;
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
}

void CDeviceContext::Initialize()
{
  CDevice* pDevice = Cast<CDevice>(Engine::Get<IDevice>());
  pDevice_ = pDevice;
  pWindow_ = Engine::Get<IWindow>();
  pTextureManager_ = Engine::Get<ITextureManager>();
  pBufferManager_ = Engine::Get<IBufferManager>();
  pShaderManager_ = Engine::Get<IShaderManager>();
  pRenderer_ = Engine::Get<IRenderer>();

  pD3D11Device_ = pDevice->GetDevice();
  pD3D11ImmediateContext_ = pDevice->GetImmediateContext();

  SwapChain_.Initialize(pD3D11Device_, pDevice->GetSwapChain());
}

void CDeviceContext::CDeviceContext::SetRenderTextures(const vdl::RenderTextures& _RenderTextures, const vdl::DepthStencilTexture& _DepthStencilTexture)
{
  std::vector<ID3D11RenderTargetView*> pRenderTeargetViews;
  {
    for (vdl::uint RenderTextureCount = 0; RenderTextureCount < Constants::kMaxRenderTextureNum; ++RenderTextureCount)
    {
      if (_RenderTextures[RenderTextureCount].isEmpty())
      {
        break;
      }

      pRenderTeargetViews.push_back(GetRenderTargetView(_RenderTextures[RenderTextureCount]));
    }

    if (pRenderTeargetViews.empty())
    {
      pRenderTeargetViews = { nullptr };
    }
  }

  ID3D11DepthStencilView* pDepthStencilView = nullptr;
  {
    if (!_DepthStencilTexture.isEmpty())
    {
      pDepthStencilView = Cast<CDepthStencilTexture>(pTextureManager_->GetTexture(_DepthStencilTexture.GetID()))->pDepthStencilView.Get();
    }
  }

  pD3D11ImmediateContext_->OMSetRenderTargets(static_cast<vdl::uint>(pRenderTeargetViews.size()), pRenderTeargetViews.data(), pDepthStencilView);
}

void CDeviceContext::ClearRenderTexture(const vdl::RenderTexture& _RenderTexture, const vdl::Color4F& _ClearColor)
{
  assert(!_RenderTexture.isEmpty());

  pD3D11ImmediateContext_->ClearRenderTargetView(GetRenderTargetView(_RenderTexture), &_ClearColor.Red);
}

void CDeviceContext::ClearDepthStencilTexture(const vdl::DepthStencilTexture& _DepthStencilTexture, float _ClearDepth, vdl::uint _ClearStencil)
{
  assert(!_DepthStencilTexture.isEmpty());

  pD3D11ImmediateContext_->ClearDepthStencilView(Cast<CDepthStencilTexture>(pTextureManager_->GetTexture(_DepthStencilTexture.GetID()))->pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, _ClearDepth, _ClearStencil);
}

void CDeviceContext::ClearUnorderedAccessTexture(const vdl::UnorderedAccessTexture& _UnorderedAccessTexture, const vdl::Color4F& _ClearColor)
{
  assert(!_UnorderedAccessTexture.isEmpty());

  pD3D11ImmediateContext_->ClearUnorderedAccessViewFloat(Cast<CUnorderedAccessTexture>(pTextureManager_->GetTexture(_UnorderedAccessTexture.GetID()))->pUnorderedAccessView.Get(), &_ClearColor.Red);
}

void CDeviceContext::Execute(const BaseGraphicsCommandList& _GraphicsCommandList)
{
  InstanceBuffer InstanceBuffer;

  for (auto& GraphicsCommand : _GraphicsCommandList.GetGraphicsCommands())
  {
    switch (GraphicsCommand.first)
    {
    case GraphicsCommandFlag::eDraw:
    {
      if (!InstanceBuffer.isEmpty())
      {
        const auto& InstanceDatas = _GraphicsCommandList.GetInstanceDatas(GraphicsCommand.second);
        pDevice_->WriteMemory(pBufferManager_->GetBuffer(InstanceBuffer.GetID()), InstanceDatas.data(), static_cast<vdl::uint>(InstanceDatas.size()));
      }

      const DrawData& DrawData = _GraphicsCommandList.GetDrawData(GraphicsCommand.second);
      pD3D11ImmediateContext_->DrawInstanced(DrawData.VertexCount, DrawData.InstanceCount, DrawData.FirstVertex, DrawData.FirstInstance);
    }
    break;
    case GraphicsCommandFlag::eDrawIndexed:
    {
      if (!InstanceBuffer.isEmpty())
      {
        const auto& InstanceDatas = _GraphicsCommandList.GetInstanceDatas(GraphicsCommand.second);
        pDevice_->WriteMemory(pBufferManager_->GetBuffer(InstanceBuffer.GetID()), InstanceDatas.data(), static_cast<vdl::uint>(InstanceDatas.size()));
      }

      const DrawIndexedData& DrawIndexedData = _GraphicsCommandList.GetDrawIndexedData(GraphicsCommand.second);
      pD3D11ImmediateContext_->DrawIndexedInstanced(DrawIndexedData.IndexCount, DrawIndexedData.InstanceCount, DrawIndexedData.FirstIndex, DrawIndexedData.VertexOffset, DrawIndexedData.FirstInstance);
    }
    break;
    case GraphicsCommandFlag::eSetVertexBuffer:
    {
      constexpr vdl::uint kOffset = 0;
      const vdl::uint Stride = GetVertexBufferStride();
      const CVertexBuffer* pVertexBuffer = Cast<CVertexBuffer>(pBufferManager_->GetBuffer(_GraphicsCommandList.GetVertexBuffer(GraphicsCommand.second).GetID()));
      pD3D11ImmediateContext_->IASetVertexBuffers(0, 1, pVertexBuffer->pBuffer.GetAddressOf(), &Stride, &kOffset);
    }
    break;
    case GraphicsCommandFlag::eSetInstanceBuffer:
    {
      InstanceBuffer = _GraphicsCommandList.GetInstanceBuffer();

      constexpr vdl::uint kOffset = 0;
      const vdl::uint Stride = GetInstanceBufferStride();
      const CInstanceBuffer* pInstanceBuffer = Cast<CInstanceBuffer>(pBufferManager_->GetBuffer(_GraphicsCommandList.GetInstanceBuffer().GetID()));
      pD3D11ImmediateContext_->IASetVertexBuffers(1, 1, pInstanceBuffer->pBuffer.GetAddressOf(), &Stride, &kOffset);
    }
    break;
    case GraphicsCommandFlag::eSetIndexBuffer:
    {
      constexpr vdl::uint kOffset = 0;
      const CIndexBuffer* pIndexBuffer = Cast<CIndexBuffer>(pBufferManager_->GetBuffer(_GraphicsCommandList.GetIndexBuffer(GraphicsCommand.second).GetID()));
      pD3D11ImmediateContext_->IASetIndexBuffer(pIndexBuffer->pBuffer.Get(), pIndexBuffer->IndexFormat, kOffset);
    }
    break;
    case GraphicsCommandFlag::eSetInputLayout:
    {
      CurrentInputLayoutType_ = _GraphicsCommandList.GetInputLayout(GraphicsCommand.second);
      pD3D11ImmediateContext_->IASetInputLayout(InputLayouts_[CurrentInputLayoutType_].Get());
    }
    break;
    case GraphicsCommandFlag::eSetTopology:
    {
      pD3D11ImmediateContext_->IASetPrimitiveTopology(Cast(_GraphicsCommandList.GetTopology(GraphicsCommand.second)));
    }
    break;
    case GraphicsCommandFlag::eSetScissor:
    {
      RECT Scissor = Cast(_GraphicsCommandList.GetScissor(GraphicsCommand.second));
      pD3D11ImmediateContext_->RSSetScissorRects(1, &Scissor);
    }
    break;
    case GraphicsCommandFlag::eSetViewport:
    {
      D3D11_VIEWPORT ViewPort = Cast(_GraphicsCommandList.GetViewport(GraphicsCommand.second));
      pD3D11ImmediateContext_->RSSetViewports(1, &ViewPort);
    }
    break;
    case GraphicsCommandFlag::eSetBlendState:
    {
      pD3D11ImmediateContext_->OMSetBlendState(GetBlendState(_GraphicsCommandList.GetBlendState(GraphicsCommand.second)), nullptr, 0xFFFFFFFF);
    }
    break;
    case GraphicsCommandFlag::eSetDepthStencilState:
    {
      const vdl::DepthStencilState& DepthStencilState = _GraphicsCommandList.GetDepthStencilState(GraphicsCommand.second);
      pD3D11ImmediateContext_->OMSetDepthStencilState(GetDepthStencilState(DepthStencilState), DepthStencilState.StencilReference);
    }
    break;
    case GraphicsCommandFlag::eSetRasterizerState:
    {
      pD3D11ImmediateContext_->RSSetState(GetRasterizerState(_GraphicsCommandList.GetRasterizerState(GraphicsCommand.second)));
    }
    break;
    case GraphicsCommandFlag::eSetVertexShader:
    {
      ID3D11VertexShader* pVertexShader = nullptr;
      {
        if (const vdl::VertexShader& VertexShader = _GraphicsCommandList.GetVertexShader(GraphicsCommand.second);
          !VertexShader.isEmpty())
        {
          pVertexShader = Cast<CVertexShader>(pShaderManager_->GetShader(VertexShader.GetID()))->pVertexShader.Get();
        }
      }

      pD3D11ImmediateContext_->VSSetShader(pVertexShader, nullptr, 0);
    }
    break;
    case GraphicsCommandFlag::eSetHullShader:
    {
      ID3D11HullShader* pHullShader = nullptr;
      {
        if (const vdl::HullShader& HullShader = _GraphicsCommandList.GetHullShader(GraphicsCommand.second);
          !HullShader.isEmpty())
        {
          pHullShader = Cast<CHullShader>(pShaderManager_->GetShader(HullShader.GetID()))->pHullShader.Get();
        }
      }

      pD3D11ImmediateContext_->HSSetShader(pHullShader, nullptr, 0);
    }
    break;
    case GraphicsCommandFlag::eSetDomainShader:
    {
      ID3D11DomainShader* pDomainShader = nullptr;
      {
        if (const vdl::DomainShader& DomainShader = _GraphicsCommandList.GetDomainShader(GraphicsCommand.second);
          !DomainShader.isEmpty())
        {
          pDomainShader = Cast<CDomainShader>(pShaderManager_->GetShader(DomainShader.GetID()))->pDomainShader.Get();
        }
      }

      pD3D11ImmediateContext_->DSSetShader(pDomainShader, nullptr, 0);
    }
    break;
    case GraphicsCommandFlag::eSetGeometryShader:
    {
      ID3D11GeometryShader* pGeometryShader = nullptr;
      {
        if (const vdl::GeometryShader& GeometryShader = _GraphicsCommandList.GetGeometryShader(GraphicsCommand.second);
          !GeometryShader.isEmpty())
        {
          pGeometryShader = Cast<CGeometryShader>(pShaderManager_->GetShader(GeometryShader.GetID()))->pGeometryShader.Get();
        }
      }

      pD3D11ImmediateContext_->GSSetShader(pGeometryShader, nullptr, 0);
    }
    break;
    case GraphicsCommandFlag::eSetPixelShader:
    {
      ID3D11PixelShader* pPixelShader = nullptr;
      {
        if (const vdl::PixelShader& PixelShader = _GraphicsCommandList.GetPixelShader(GraphicsCommand.second);
          !PixelShader.isEmpty())
        {
          pPixelShader = Cast<CPixelShader>(pShaderManager_->GetShader(PixelShader.GetID()))->pPixelShader.Get();
        }
      }

      pD3D11ImmediateContext_->PSSetShader(pPixelShader, nullptr, 0);
    }
    break;
    case GraphicsCommandFlag::eSetVertexStageShaderResource:
    {
      const auto& ShaderResources = _GraphicsCommandList.GetShaderResources<ShaderType::eVertexShader>(GraphicsCommand.second);
      const vdl::uint ShaderResouceNum = static_cast<vdl::uint>(ShaderResources.size());

      std::vector<ID3D11ShaderResourceView*> pShaderResources(ShaderResouceNum);
      {
        for (vdl::uint ShaderResourceCount = 0; ShaderResourceCount < ShaderResouceNum; ++ShaderResourceCount)
        {
          pShaderResources[ShaderResourceCount] = GetShaderResourceView(ShaderResources[ShaderResourceCount]);
        }
      }

      pD3D11ImmediateContext_->VSSetShaderResources(0, ShaderResouceNum, pShaderResources.data());
    }
    break;
    case GraphicsCommandFlag::eSetHullStageShaderResource:
    {
      const auto& ShaderResources = _GraphicsCommandList.GetShaderResources<ShaderType::eHullShader>(GraphicsCommand.second);
      const vdl::uint ShaderResouceNum = static_cast<vdl::uint>(ShaderResources.size());

      std::vector<ID3D11ShaderResourceView*> pShaderResources(ShaderResouceNum);
      {
        for (vdl::uint ShaderResourceCount = 0; ShaderResourceCount < ShaderResouceNum; ++ShaderResourceCount)
        {
          pShaderResources[ShaderResourceCount] = GetShaderResourceView(ShaderResources[ShaderResourceCount]);
        }
      }

      pD3D11ImmediateContext_->HSSetShaderResources(0, ShaderResouceNum, pShaderResources.data());
    }
    break;
    case GraphicsCommandFlag::eSetDomainStageShaderResource:
    {
      const auto& ShaderResources = _GraphicsCommandList.GetShaderResources<ShaderType::eDomainShader>(GraphicsCommand.second);
      const vdl::uint ShaderResouceNum = static_cast<vdl::uint>(ShaderResources.size());

      std::vector<ID3D11ShaderResourceView*> pShaderResources(ShaderResouceNum);
      {
        for (vdl::uint ShaderResourceCount = 0; ShaderResourceCount < ShaderResouceNum; ++ShaderResourceCount)
        {
          pShaderResources[ShaderResourceCount] = GetShaderResourceView(ShaderResources[ShaderResourceCount]);
        }
      }

      pD3D11ImmediateContext_->DSSetShaderResources(0, ShaderResouceNum, pShaderResources.data());
    }
    break;
    case GraphicsCommandFlag::eSetGeometryStageShaderResource:
    {
      const auto& ShaderResources = _GraphicsCommandList.GetShaderResources<ShaderType::eGeometryShader>(GraphicsCommand.second);
      const vdl::uint ShaderResouceNum = static_cast<vdl::uint>(ShaderResources.size());

      std::vector<ID3D11ShaderResourceView*> pShaderResources(ShaderResouceNum);
      {
        for (vdl::uint ShaderResourceCount = 0; ShaderResourceCount < ShaderResouceNum; ++ShaderResourceCount)
        {
          pShaderResources[ShaderResourceCount] = GetShaderResourceView(ShaderResources[ShaderResourceCount]);
        }
      }

      pD3D11ImmediateContext_->GSSetShaderResources(0, ShaderResouceNum, pShaderResources.data());
    }
    break;
    case GraphicsCommandFlag::eSetPixelStageShaderResource:
    {
      const auto& ShaderResources = _GraphicsCommandList.GetShaderResources<ShaderType::ePixelShader>(GraphicsCommand.second);
      const vdl::uint ShaderResouceNum = static_cast<vdl::uint>(ShaderResources.size());

      std::vector<ID3D11ShaderResourceView*> pShaderResources(ShaderResouceNum);
      {
        for (vdl::uint ShaderResourceCount = 0; ShaderResourceCount < ShaderResouceNum; ++ShaderResourceCount)
        {
          pShaderResources[ShaderResourceCount] = GetShaderResourceView(ShaderResources[ShaderResourceCount]);
        }
      }

      pD3D11ImmediateContext_->PSSetShaderResources(0, ShaderResouceNum, pShaderResources.data());
    }
    break;
    case GraphicsCommandFlag::eSetVertexStageSampler:
    {
      const auto& Samplers = _GraphicsCommandList.GetSamplers<ShaderType::eVertexShader>(GraphicsCommand.second);
      const vdl::uint SamplerNum = static_cast<vdl::uint>(Samplers.size());

      std::vector<ID3D11SamplerState*> pSamplers(SamplerNum);
      {
        for (vdl::uint SamplerCount = 0; SamplerCount < SamplerNum; ++SamplerCount)
        {
          pSamplers[SamplerCount] = GetSamplerState(Samplers[SamplerCount]);
        }
      }

      pD3D11ImmediateContext_->VSSetSamplers(0, SamplerNum, pSamplers.data());
    }
    break;
    case GraphicsCommandFlag::eSetHullStageSampler:
    {
      const auto& Samplers = _GraphicsCommandList.GetSamplers<ShaderType::eHullShader>(GraphicsCommand.second);
      const vdl::uint SamplerNum = static_cast<vdl::uint>(Samplers.size());

      std::vector<ID3D11SamplerState*> pSamplers(SamplerNum);
      {
        for (vdl::uint SamplerCount = 0; SamplerCount < SamplerNum; ++SamplerCount)
        {
          pSamplers[SamplerCount] = GetSamplerState(Samplers[SamplerCount]);
        }
      }

      pD3D11ImmediateContext_->HSSetSamplers(0, SamplerNum, pSamplers.data());
    }
    break;
    case GraphicsCommandFlag::eSetDomainStageSampler:
    {
      const auto& Samplers = _GraphicsCommandList.GetSamplers<ShaderType::eDomainShader>(GraphicsCommand.second);
      const vdl::uint SamplerNum = static_cast<vdl::uint>(Samplers.size());

      std::vector<ID3D11SamplerState*> pSamplers(SamplerNum);
      {
        for (vdl::uint SamplerCount = 0; SamplerCount < SamplerNum; ++SamplerCount)
        {
          pSamplers[SamplerCount] = GetSamplerState(Samplers[SamplerCount]);
        }
      }

      pD3D11ImmediateContext_->DSSetSamplers(0, SamplerNum, pSamplers.data());
    }
    break;
    case GraphicsCommandFlag::eSetGeometryStageSampler:
    {
      const auto& Samplers = _GraphicsCommandList.GetSamplers<ShaderType::eGeometryShader>(GraphicsCommand.second);
      const vdl::uint SamplerNum = static_cast<vdl::uint>(Samplers.size());

      std::vector<ID3D11SamplerState*> pSamplers(SamplerNum);
      {
        for (vdl::uint SamplerCount = 0; SamplerCount < SamplerNum; ++SamplerCount)
        {
          pSamplers[SamplerCount] = GetSamplerState(Samplers[SamplerCount]);
        }
      }

      pD3D11ImmediateContext_->GSSetSamplers(0, SamplerNum, pSamplers.data());
    }
    break;
    case GraphicsCommandFlag::eSetPixelStageSampler:
    {
      const auto& Samplers = _GraphicsCommandList.GetSamplers<ShaderType::ePixelShader>(GraphicsCommand.second);
      const vdl::uint SamplerNum = static_cast<vdl::uint>(Samplers.size());

      std::vector<ID3D11SamplerState*> pSamplers(SamplerNum);
      {
        for (vdl::uint SamplerCount = 0; SamplerCount < SamplerNum; ++SamplerCount)
        {
          pSamplers[SamplerCount] = GetSamplerState(Samplers[SamplerCount]);
        }
      }

      pD3D11ImmediateContext_->PSSetSamplers(0, SamplerNum, pSamplers.data());
    }
    break;
    case GraphicsCommandFlag::eSetVertexStageConstantBuffer:
    {
      const auto& ConstantBuffers = _GraphicsCommandList.GetConstantBuffers<ShaderType::eVertexShader>(GraphicsCommand.second);
      const vdl::uint ConstantBufferNum = static_cast<vdl::uint>(ConstantBuffers.size());

      std::vector<ID3D11Buffer*> pConstantBuffers(ConstantBufferNum);
      {
        for (vdl::uint ConstantBufferCount = 0; ConstantBufferCount < ConstantBufferNum; ++ConstantBufferCount)
        {
          pConstantBuffers[ConstantBufferCount] = GetConstantBuffer(ConstantBuffers[ConstantBufferCount]);
        }
      }

      pD3D11ImmediateContext_->VSSetConstantBuffers(0, ConstantBufferNum, pConstantBuffers.data());
    }
    break;
    case GraphicsCommandFlag::eSetHullStageConstantBuffer:
    {
      const auto& ConstantBuffers = _GraphicsCommandList.GetConstantBuffers<ShaderType::eHullShader>(GraphicsCommand.second);
      const vdl::uint ConstantBufferNum = static_cast<vdl::uint>(ConstantBuffers.size());

      std::vector<ID3D11Buffer*> pConstantBuffers(ConstantBufferNum);
      {
        for (vdl::uint ConstantBufferCount = 0; ConstantBufferCount < ConstantBufferNum; ++ConstantBufferCount)
        {
          pConstantBuffers[ConstantBufferCount] = GetConstantBuffer(ConstantBuffers[ConstantBufferCount]);
        }
      }

      pD3D11ImmediateContext_->HSSetConstantBuffers(0, ConstantBufferNum, pConstantBuffers.data());
    }
    break;
    case GraphicsCommandFlag::eSetDomainStageConstantBuffer:
    {
      const auto& ConstantBuffers = _GraphicsCommandList.GetConstantBuffers<ShaderType::eDomainShader>(GraphicsCommand.second);
      const vdl::uint ConstantBufferNum = static_cast<vdl::uint>(ConstantBuffers.size());

      std::vector<ID3D11Buffer*> pConstantBuffers(ConstantBufferNum);
      {
        for (vdl::uint ConstantBufferCount = 0; ConstantBufferCount < ConstantBufferNum; ++ConstantBufferCount)
        {
          pConstantBuffers[ConstantBufferCount] = GetConstantBuffer(ConstantBuffers[ConstantBufferCount]);
        }
      }

      pD3D11ImmediateContext_->DSSetConstantBuffers(0, ConstantBufferNum, pConstantBuffers.data());
    }
    break;
    case GraphicsCommandFlag::eSetGeometryStageConstantBuffer:
    {
      const auto& ConstantBuffers = _GraphicsCommandList.GetConstantBuffers<ShaderType::eGeometryShader>(GraphicsCommand.second);
      const vdl::uint ConstantBufferNum = static_cast<vdl::uint>(ConstantBuffers.size());

      std::vector<ID3D11Buffer*> pConstantBuffers(ConstantBufferNum);
      {
        for (vdl::uint ConstantBufferCount = 0; ConstantBufferCount < ConstantBufferNum; ++ConstantBufferCount)
        {
          pConstantBuffers[ConstantBufferCount] = GetConstantBuffer(ConstantBuffers[ConstantBufferCount]);
        }
      }

      pD3D11ImmediateContext_->GSSetConstantBuffers(0, ConstantBufferNum, pConstantBuffers.data());
    }
    break;
    case GraphicsCommandFlag::eSetPixelStageConstantBuffer:
    {
      const auto& ConstantBuffers = _GraphicsCommandList.GetConstantBuffers<ShaderType::ePixelShader>(GraphicsCommand.second);
      const vdl::uint ConstantBufferNum = static_cast<vdl::uint>(ConstantBuffers.size());

      std::vector<ID3D11Buffer*> pConstantBuffers(ConstantBufferNum);
      {
        for (vdl::uint ConstantBufferCount = 0; ConstantBufferCount < ConstantBufferNum; ++ConstantBufferCount)
        {
          pConstantBuffers[ConstantBufferCount] = GetConstantBuffer(ConstantBuffers[ConstantBufferCount]);
        }
      }

      pD3D11ImmediateContext_->PSSetConstantBuffers(0, ConstantBufferNum, pConstantBuffers.data());
    }
    break;
    default: assert(false);
    }
  }

  ID3D11ShaderResourceView* pShaderResourceView = nullptr;
  pD3D11ImmediateContext_->PSSetShaderResources(0, 1, &pShaderResourceView);
}

void CDeviceContext::Execute(const ComputeCommandList& _ComputeCommandList)
{
  for (auto& ComputeCommand : _ComputeCommandList.GetComputeCommands())
  {
    switch (ComputeCommand)
    {
    case ComputeCommandFlag::eSetShader:
    {
      ID3D11ComputeShader* pComputeShader = nullptr;
      {
        if (const vdl::ComputeShader& ComputeShader = _ComputeCommandList.GetComputeShader();
          !ComputeShader.isEmpty())
        {
          pComputeShader = Cast<CComputeShader>(pShaderManager_->GetShader(ComputeShader.GetID()))->pComputeShader.Get();
        }
      }

      pD3D11ImmediateContext_->CSSetShader(pComputeShader, nullptr, 0);
    }
    break;
    case ComputeCommandFlag::eSetSampler:
    {
      const auto& Samplers = _ComputeCommandList.GetSamplers();
      const vdl::uint SamplerNum = static_cast<vdl::uint>(Samplers.size());

      std::vector<ID3D11SamplerState*> pSamplers(SamplerNum);
      {
        for (vdl::uint SamplerCount = 0; SamplerCount < SamplerNum; ++SamplerCount)
        {
          pSamplers[SamplerCount] = GetSamplerState(Samplers[SamplerCount]);
        }
      }

      pD3D11ImmediateContext_->CSSetSamplers(0, SamplerNum, pSamplers.data());
    }
    break;
    case ComputeCommandFlag::eSetConstantBuffer:
    {
      const auto& ConstantBuffers = _ComputeCommandList.GetConstantBuffers();
      const vdl::uint ConstantBufferNum = static_cast<vdl::uint>(ConstantBuffers.size());

      std::vector<ID3D11Buffer*> pConstantBuffers(ConstantBufferNum);
      {
        for (vdl::uint ConstantBufferCount = 0; ConstantBufferCount < ConstantBufferNum; ++ConstantBufferCount)
        {
          pConstantBuffers[ConstantBufferCount] = GetConstantBuffer(ConstantBuffers[ConstantBufferCount]);
        }
      }

      pD3D11ImmediateContext_->CSSetConstantBuffers(0, ConstantBufferNum, pConstantBuffers.data());
    }
    break;
    default:
      break;
    }
  }

  //  CSSetShaderResources
  {
    const auto& ShaderResources = _ComputeCommandList.GetShaderResources();
    const vdl::uint ShaderResouceNum = static_cast<vdl::uint>(ShaderResources.size());

    std::vector<ID3D11ShaderResourceView*> pShaderResources(ShaderResouceNum);
    {
      for (vdl::uint ShaderResourceCount = 0; ShaderResourceCount < ShaderResouceNum; ++ShaderResourceCount)
      {
        pShaderResources[ShaderResourceCount] = GetShaderResourceView(ShaderResources[ShaderResourceCount]);
      }
    }

    pD3D11ImmediateContext_->CSSetShaderResources(0, ShaderResouceNum, pShaderResources.data());
  }

  //  CSSetUnorderedAccessViews
  {
    //  シェーダーリソースの全解除
    {
      std::vector<ID3D11ShaderResourceView*> pShaderResourceViews(Constants::kMaxShaderResourceNum);

      pD3D11ImmediateContext_->VSSetShaderResources(0, Constants::kMaxShaderResourceNum, pShaderResourceViews.data());
      pD3D11ImmediateContext_->HSSetShaderResources(0, Constants::kMaxShaderResourceNum, pShaderResourceViews.data());
      pD3D11ImmediateContext_->DSSetShaderResources(0, Constants::kMaxShaderResourceNum, pShaderResourceViews.data());
      pD3D11ImmediateContext_->GSSetShaderResources(0, Constants::kMaxShaderResourceNum, pShaderResourceViews.data());
      pD3D11ImmediateContext_->PSSetShaderResources(0, Constants::kMaxShaderResourceNum, pShaderResourceViews.data());
    }

    const auto& UnorderedAccessObjects = _ComputeCommandList.GetUnorderedAccessObjects();
    const vdl::uint UnorderedAccessObjectNum = static_cast<vdl::uint>(UnorderedAccessObjects.size());

    std::vector<ID3D11UnorderedAccessView*> pUnorderedAccessBuffers(UnorderedAccessObjectNum);
    {
      for (vdl::uint UnorderedAccessObjectCount = 0; UnorderedAccessObjectCount < UnorderedAccessObjectNum; ++UnorderedAccessObjectCount)
      {
        pUnorderedAccessBuffers[UnorderedAccessObjectCount] = GetUnorderedAccessView(UnorderedAccessObjects[UnorderedAccessObjectCount]);
      }
    }

    pD3D11ImmediateContext_->CSSetUnorderedAccessViews(0, UnorderedAccessObjectNum, pUnorderedAccessBuffers.data(), nullptr);
  }

  //  Dispatch
  {
    const vdl::uint3& ThreadGroup = _ComputeCommandList.GetThreadGroup();

    pD3D11ImmediateContext_->Dispatch(ThreadGroup.x, ThreadGroup.y, ThreadGroup.z);
  }

  std::array<ID3D11ShaderResourceView*, Constants::kMaxShaderResourceNum> ShaderResourceViews = {};
  pD3D11ImmediateContext_->CSSetShaderResources(0, Constants::kMaxShaderResourceNum, ShaderResourceViews.data());

  std::array<ID3D11UnorderedAccessView*, Constants::kMaxUnorderedAccessObjectNum> UnorderedAccessViews = {};
  pD3D11ImmediateContext_->CSSetUnorderedAccessViews(0, Constants::kMaxUnorderedAccessObjectNum, UnorderedAccessViews.data(), nullptr);
}

void CDeviceContext::Flush()
{
  std::array<ID3D11ShaderResourceView*, Constants::kMaxShaderResourceNum> ShaderResourceViews = {};

  pD3D11ImmediateContext_->VSSetShaderResources(0, Constants::kMaxShaderResourceNum, ShaderResourceViews.data());
  pD3D11ImmediateContext_->HSSetShaderResources(0, Constants::kMaxShaderResourceNum, ShaderResourceViews.data());
  pD3D11ImmediateContext_->DSSetShaderResources(0, Constants::kMaxShaderResourceNum, ShaderResourceViews.data());
  pD3D11ImmediateContext_->GSSetShaderResources(0, Constants::kMaxShaderResourceNum, ShaderResourceViews.data());
  pD3D11ImmediateContext_->PSSetShaderResources(0, Constants::kMaxShaderResourceNum, ShaderResourceViews.data());
}

void CDeviceContext::Present()
{
  HRESULT hr = S_OK;

  hr = SwapChain_->Present(0, 0);
  ERROR_CHECK(hr);

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
  //HRESULT hr = S_OK;
  //
  //Microsoft::WRL::ComPtr<ID3D11Texture2D> BackBuffer;
  //{
  //  hr = pSwapChain_->GetBuffer(0, IID_PPV_ARGS(BackBuffer.GetAddressOf()));
  //  ERROR_CHECK(hr);
  //}
  //
  //vdl::uint2 TextureSize;
  //Microsoft::WRL::ComPtr<ID3D11Texture2D> CopyBuffer;
  //{
  //  D3D11_TEXTURE2D_DESC Texture2DDesc;
  //  {
  //    BackBuffer->GetDesc(&Texture2DDesc);
  //    Texture2DDesc.BindFlags = 0;
  //    Texture2DDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
  //    Texture2DDesc.Usage = D3D11_USAGE_STAGING;
  //  }
  //
  //  TextureSize = { Texture2DDesc.Width, Texture2DDesc.Height };
  //
  //  hr = pD3D11Device_->CreateTexture2D(&Texture2DDesc, nullptr, CopyBuffer.GetAddressOf());
  //  ERROR_CHECK(hr);
  //
  //  pD3D11ImmediateContext_->CopyResource(CopyBuffer.Get(), BackBuffer.Get());
  //}
  //
  //vdl::Image Image;
  //Image.Resize(TextureSize);
  //{
  //  D3D11_MAPPED_SUBRESOURCE MappedSubresorce;
  //  hr = pD3D11ImmediateContext_->Map(CopyBuffer.Get(), 0, D3D11_MAP_READ, 0, &MappedSubresorce);
  //  ERROR_CHECK(hr);
  //
  //  ::memcpy(Image.Buffer(), MappedSubresorce.pData, Image.BufferSize());
  //
  //  pD3D11ImmediateContext_->Unmap(CopyBuffer.Get(), 0);
  //}
  //
  //Image.SavePNG(Constants::kScreenShotFileDirectory);
}

//--------------------------------------------------

void CDeviceContext::RegisterInputLayout(vdl::InputLayoutType _Key, ID3DBlob* _pCode)
{
  HRESULT hr = S_OK;

  Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
  {
    std::vector<D3D11_INPUT_ELEMENT_DESC> InputElementDesc;

    switch (_Key)
    {
    case vdl::InputLayoutType::eNone:
      break;
    case vdl::InputLayoutType::eTexture:
    {
      InputElementDesc.push_back({ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
      InputElementDesc.push_back({ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });

      InputElementDesc.push_back({ "NDC_TRANSFORM", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 });
      InputElementDesc.push_back({ "NDC_TRANSFORM", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 });
      InputElementDesc.push_back({ "NDC_TRANSFORM", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 });
      InputElementDesc.push_back({ "NDC_TRANSFORM", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 });
      InputElementDesc.push_back({ "TEXCOORD_SCALE", 0, DXGI_FORMAT_R32G32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 });
      InputElementDesc.push_back({ "TEXCOORD_TRANSLATE", 0, DXGI_FORMAT_R32G32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 });
      InputElementDesc.push_back({ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 });
    }
    break;
    case vdl::InputLayoutType::eStaticMesh:
    {
      InputElementDesc.push_back({ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
      InputElementDesc.push_back({ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
      InputElementDesc.push_back({ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
      InputElementDesc.push_back({ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });

      InputElementDesc.push_back({ "WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 });
      InputElementDesc.push_back({ "WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 });
      InputElementDesc.push_back({ "WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 });
      InputElementDesc.push_back({ "WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 });
      InputElementDesc.push_back({ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 });
    }
    break;
    case vdl::InputLayoutType::eSkinnedMesh:
    {
      InputElementDesc.push_back({ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
      InputElementDesc.push_back({ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
      InputElementDesc.push_back({ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
      InputElementDesc.push_back({ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
      InputElementDesc.push_back({ "WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
      InputElementDesc.push_back({ "BONES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });

      InputElementDesc.push_back({ "WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 });
      InputElementDesc.push_back({ "WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 });
      InputElementDesc.push_back({ "WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 });
      InputElementDesc.push_back({ "WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 });
      InputElementDesc.push_back({ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 });
    }
    break;
    case vdl::InputLayoutType::eGUI:
    {
      InputElementDesc.push_back({ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,   0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
      InputElementDesc.push_back({ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,   0, D3D11_APPEND_ALIGNED_ELEMENT,  D3D11_INPUT_PER_VERTEX_DATA, 0 });
      InputElementDesc.push_back({ "COLOR",    0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
    }
    break;
    default: assert(false);
    }

    if (!InputElementDesc.empty())
    {
      hr = pD3D11Device_->CreateInputLayout(InputElementDesc.data(), static_cast<vdl::uint>(InputElementDesc.size()), _pCode->GetBufferPointer(), _pCode->GetBufferSize(), pInputLayout.GetAddressOf());
      ERROR_CHECK(hr);
    }
  }

  InputLayouts_.insert(std::make_pair(_Key, pInputLayout));
}

ID3D11BlendState* CDeviceContext::GetBlendState(const vdl::BlendState& _BlendState)
{
  if (BlendStates_.find(_BlendState) == BlendStates_.end())
  {
    HRESULT hr = S_OK;

    Microsoft::WRL::ComPtr<ID3D11BlendState> pBlendState;
    {
      D3D11_BLEND_DESC BlendDesc;
      {
        BlendDesc.AlphaToCoverageEnable = _BlendState.AlphaToCoverageEnable;
        BlendDesc.IndependentBlendEnable = _BlendState.IndependentBlendEnable;

        const vdl::uint EnableRenderTextureNum = (_BlendState.IndependentBlendEnable ? Constants::kMaxRenderTextureNum : 1);

        for (vdl::uint RenderTextureCount = 0; RenderTextureCount < EnableRenderTextureNum; ++RenderTextureCount)
        {
          D3D11_RENDER_TARGET_BLEND_DESC& RenderTargetBlendDesc = BlendDesc.RenderTarget[RenderTextureCount];
          const vdl::RenderTextureBlendState& RenderTextureBlendState = _BlendState.RenderTexture[RenderTextureCount];

          RenderTargetBlendDesc.BlendEnable = RenderTextureBlendState.BlendEnable;
          RenderTargetBlendDesc.SrcBlend = Cast(RenderTextureBlendState.SrcBlend);
          RenderTargetBlendDesc.DestBlend = Cast(RenderTextureBlendState.DestBlend);
          RenderTargetBlendDesc.BlendOp = Cast(RenderTextureBlendState.BlendOp);
          RenderTargetBlendDesc.SrcBlendAlpha = Cast(RenderTextureBlendState.SrcBlendAlpha);
          RenderTargetBlendDesc.DestBlendAlpha = Cast(RenderTextureBlendState.DestBlendAlpha);
          RenderTargetBlendDesc.BlendOpAlpha = Cast(RenderTextureBlendState.BlendOpAlpha);
          RenderTargetBlendDesc.RenderTargetWriteMask = Cast(RenderTextureBlendState.RenderTargetWriteMask);
        }
      }

      hr = pD3D11Device_->CreateBlendState(&BlendDesc, pBlendState.GetAddressOf());
      ERROR_CHECK(hr);
    }

    BlendStates_.insert(std::make_pair(_BlendState, pBlendState));
  }

  return BlendStates_.at(_BlendState).Get();
}

ID3D11DepthStencilState* CDeviceContext::GetDepthStencilState(const vdl::DepthStencilState& _DepthStencilState)
{
  if (DepthStencilStates_.find(_DepthStencilState) == DepthStencilStates_.end())
  {
    HRESULT hr = S_OK;

    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> pDepthStencilState;
    {
      D3D11_DEPTH_STENCIL_DESC DepthStencilDesc;
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

      hr = pD3D11Device_->CreateDepthStencilState(&DepthStencilDesc, pDepthStencilState.GetAddressOf());
      ERROR_CHECK(hr);
    }

    DepthStencilStates_.insert(std::make_pair(_DepthStencilState, pDepthStencilState));
  }

  return DepthStencilStates_.at(_DepthStencilState).Get();
}

ID3D11RasterizerState* CDeviceContext::GetRasterizerState(const vdl::RasterizerState& _RasterizerState)
{
  if (RasterizerStates_.find(_RasterizerState) == RasterizerStates_.end())
  {
    HRESULT hr = S_OK;

    Microsoft::WRL::ComPtr<ID3D11RasterizerState> pRasterizerState;
    {
      D3D11_RASTERIZER_DESC RasterizerDesc;
      {
        RasterizerDesc.FillMode = Cast(_RasterizerState.FillMode);
        RasterizerDesc.CullMode = Cast(_RasterizerState.CullMode);
        RasterizerDesc.FrontCounterClockwise = _RasterizerState.FrontCounterClockwise;
        RasterizerDesc.DepthBias = _RasterizerState.DepthBias;
        RasterizerDesc.DepthBiasClamp = 0.0f;
        RasterizerDesc.SlopeScaledDepthBias = 0.0f;
        RasterizerDesc.DepthClipEnable = _RasterizerState.DepthClipEnable;
        RasterizerDesc.ScissorEnable = _RasterizerState.ScissorEnable;
        RasterizerDesc.MultisampleEnable = false;
        RasterizerDesc.AntialiasedLineEnable = _RasterizerState.AntialiasedLineEnable;
      }

      hr = pD3D11Device_->CreateRasterizerState(&RasterizerDesc, pRasterizerState.GetAddressOf());
      ERROR_CHECK(hr);
    }

    RasterizerStates_.insert(std::make_pair(_RasterizerState, pRasterizerState));
  }

  return RasterizerStates_.at(_RasterizerState).Get();
}

ID3D11RenderTargetView* CDeviceContext::GetRenderTargetView(const vdl::RenderTexture& _RenderTexture)
{
  assert(!_RenderTexture.isEmpty());

  ITexture* pTexture = pTextureManager_->GetTexture(_RenderTexture.GetID());
  if (pTexture->GetType() == TextureType::eSwapChainRenderTexture)
  {
    return Cast<CSwapChainRenderTexture>(pTexture)->pRenderTargetView;
  }
  else
  {
    return Cast<CRenderTexture>(pTexture)->pRenderTargetView.Get();
  }
}

ID3D11ShaderResourceView* CDeviceContext::GetShaderResourceView(const vdl::ShaderResource& _ShaderResource)
{
  ID3D11ShaderResourceView* pShaderResourceView = nullptr;
  {
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
          pShaderResourceView = pDepthTexture->pShaderResourceView.Get();
        }
        break;
        case TextureType::eStencilTexture:
        {
          CStencilTexture* pStencilTexture = Cast<CStencilTexture>(pTexture);
          pShaderResourceView = pStencilTexture->pShaderResourceView.Get();
        }
        break;
        default:
        {
          CTexture* pColorTexture = Cast<CTexture>(pTexture);
          pShaderResourceView = pColorTexture->pShaderResourceView.Get();
        }
        break;
        }
      }
    }
    //  CubeTexture
    if (const vdl::CubeTexture* pShaderResource = _ShaderResource.GetIf<vdl::CubeTexture>())
    {
      if (!pShaderResource->isEmpty())
      {
        pShaderResourceView = Cast<CCubeTexture>(pTextureManager_->GetTexture(pShaderResource->GetID()))->pShaderResourceView.Get();
      }
    }
    //  UnorderedAccessBuffer
    else if (const vdl::Detail::UnorderedAccessBufferData* pShaderResource = _ShaderResource.GetIf<vdl::Detail::UnorderedAccessBufferData>())
    {
      if (!pShaderResource->isEmpty())
      {
        pShaderResourceView = Cast<CUnordererdAccessBuffer>(pBufferManager_->GetBuffer(pShaderResource->GetID()))->pShaderResourceView.Get();
      }
    }
  }

  return pShaderResourceView;
}

ID3D11SamplerState* CDeviceContext::GetSamplerState(const vdl::Sampler& _Sampler)
{
  if (Samplers_.find(_Sampler) == Samplers_.end())
  {
    HRESULT hr = S_OK;

    Microsoft::WRL::ComPtr<ID3D11SamplerState> pSamplerState;
    {
      D3D11_SAMPLER_DESC SamplerDesc;
      {
        SamplerDesc.Filter = Cast(_Sampler.Filter);
        SamplerDesc.AddressU = Cast(_Sampler.AddressModeU);
        SamplerDesc.AddressV = Cast(_Sampler.AddressModeV);
        SamplerDesc.AddressW = Cast(_Sampler.AddressModeW);
        SamplerDesc.MipLODBias = 0.0f;
        SamplerDesc.MaxAnisotropy = _Sampler.MaxAnisotropy;
        SamplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
        vdl::Color4F BorderColor = Cast(_Sampler.BorderColor);
        ::memcpy(SamplerDesc.BorderColor, &BorderColor, sizeof(float) * 4);
        SamplerDesc.MinLOD = 0;
        SamplerDesc.MaxLOD = FLT_MAX;
      }

      hr = pD3D11Device_->CreateSamplerState(&SamplerDesc, pSamplerState.GetAddressOf());
      ERROR_CHECK(hr);
    }

    Samplers_.insert(std::make_pair(_Sampler, pSamplerState));
  }

  return Samplers_.at(_Sampler).Get();
}

ID3D11Buffer* CDeviceContext::GetConstantBuffer(const vdl::Detail::ConstantBufferData& _ConstantBuffer)
{
  ID3D11Buffer* pConstantBuffer = nullptr;
  {
    if (!_ConstantBuffer.isEmpty())
    {
      pConstantBuffer = Cast<CCopyConstantBuffer>(pBufferManager_->GetBuffer(_ConstantBuffer.GetID()))->pBuffer.Get();
    }
  }

  return pConstantBuffer;
}

ID3D11UnorderedAccessView* CDeviceContext::GetUnorderedAccessView(const vdl::UnorderedAccessObject& _UnorderedAccessObject)
{
  ID3D11UnorderedAccessView* pUnorderedAccessView = nullptr;
  {
    //  UnorderedAccessTexture
    if (const vdl::UnorderedAccessTexture* pUnorderedAccessObject = _UnorderedAccessObject.GetIf<vdl::UnorderedAccessTexture>())
    {
      if (!pUnorderedAccessObject->isEmpty())
      {
        pUnorderedAccessView = Cast<CUnorderedAccessTexture>(pTextureManager_->GetTexture(pUnorderedAccessObject->GetID()))->pUnorderedAccessView.Get();
      }
    }
    //  UnorderedAccessBuffer
    else if (const vdl::Detail::UnorderedAccessBufferData* pUnorderedAccessObject = _UnorderedAccessObject.GetIf<vdl::Detail::UnorderedAccessBufferData>())
    {
      if (!pUnorderedAccessObject->isEmpty())
      {
        pUnorderedAccessView = Cast<CUnordererdAccessBuffer>(pBufferManager_->GetBuffer(pUnorderedAccessObject->GetID()))->pUnorderedAccessView.Get();
      }
    }
  }

  return pUnorderedAccessView;
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
