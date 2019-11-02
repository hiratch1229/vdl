#include "CDeviceContext.hpp"

#include <vdl/Engine.hpp>
#include <vdl/Device/DirectX11/CDevice.hpp>
#include <vdl/SwapChain/DirectX11/CSwapChain.hpp>
#include <vdl/TextureManager/ITextureManager.hpp>
#include <vdl/BufferManager/IBufferManager.hpp>
#include <vdl/ShaderManager/IShaderManager.hpp>

#include <vdl/Topology/DirectX/Topology.hpp>
#include <vdl/Scissor/DirectX/Scissor.hpp>
#include <vdl/Viewport/DirectX11/Viewport.hpp>
#include <vdl/Buffer/DirectX11/CBuffer.hpp>
#include <vdl/Texture/DirectX11/CTexture.hpp>
#include <vdl/Shader/DirectX11/CShader.hpp>

#include <vdl/Constants/Constants.hpp>

#include <vdl/Misc/Windows/Misc.hpp>

#include <vdl/Texture.hpp>
#include <vdl/Shader.hpp>

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
}

void CDeviceContext::Initialize()
{
  pSwapChain_ = static_cast<CSwapChain*>(Engine::Get<ISwapChain>());
  pTextureManager_ = Engine::Get<ITextureManager>();
  pBufferManager_ = Engine::Get<IBufferManager>();
  pShaderManager_ = Engine::Get<IShaderManager>();

  CDevice* pDevice = static_cast<CDevice*>(Engine::Get<IDevice>());
  pD3D11Device_ = pDevice->GetDevice();
  pD3D11ImmediateContext_ = pDevice->GetImmediateContext();
}

void CDeviceContext::SetVertexBuffer(const VertexBuffer& _VertexBuffer)
{
  if (_VertexBuffer.isEmpty())
  {
    return;
  }

  assert(pBufferManager_->GetBuffer(_VertexBuffer.GetID())->GetType() == BufferType::eVertexBuffer);

  const CVertexBuffer* pVertexBuffer = static_cast<CVertexBuffer*>(pBufferManager_->GetBuffer(_VertexBuffer.GetID()));

  constexpr vdl::uint kOffset = 0;
  pD3D11ImmediateContext_->IASetVertexBuffers(0, 1, pVertexBuffer->pBuffer.GetAddressOf(), &pVertexBuffer->Stride, &kOffset);
}

void CDeviceContext::SetInstanceBuffer(const InstanceBuffer& _InstanceBuffer)
{
  if (_InstanceBuffer.isEmpty())
  {
    return;
  }

  assert(pBufferManager_->GetBuffer(_InstanceBuffer.GetID())->GetType() == BufferType::eInstanceBuffer);

  const CInstanceBuffer* pInstanceBuffer = static_cast<CInstanceBuffer*>(pBufferManager_->GetBuffer(_InstanceBuffer.GetID()));

  constexpr vdl::uint kOffset = 0;
  pD3D11ImmediateContext_->IASetVertexBuffers(1, 1, pInstanceBuffer->pBuffer.GetAddressOf(), &pInstanceBuffer->Stride, &kOffset);
}

void CDeviceContext::SetIndexBuffer(const IndexBuffer& _IndexBuffer)
{
  if (_IndexBuffer.isEmpty())
  {
    return;
  }

  assert(pBufferManager_->GetBuffer(_IndexBuffer.GetID())->GetType() == BufferType::eIndexBuffer);

  const CIndexBuffer* pIndexBuffer = static_cast<CIndexBuffer*>(pBufferManager_->GetBuffer(_IndexBuffer.GetID()));

  constexpr vdl::uint kOffset = 0;
  pD3D11ImmediateContext_->IASetIndexBuffer(pIndexBuffer->pBuffer.Get(), pIndexBuffer->IndexFormat, kOffset);
}

void CDeviceContext::SetInputLayout(vdl::InputLayoutType _InputLayout)
{
  assert(isFoundInputLayout(_InputLayout));

  pD3D11ImmediateContext_->IASetInputLayout(InputLayouts_[_InputLayout].Get());
}

void CDeviceContext::SetTopology(vdl::TopologyType _Topology)
{
  pD3D11ImmediateContext_->IASetPrimitiveTopology(Cast(_Topology));
}

void CDeviceContext::SetScissor(const vdl::Scissor& _Scissor)
{
  pD3D11ImmediateContext_->RSSetScissorRects(1, &Cast(_Scissor));
}

void CDeviceContext::SetViewport(const vdl::Viewport& _Viewport)
{
  pD3D11ImmediateContext_->RSSetViewports(1, &Cast(_Viewport));
}

void CDeviceContext::CDeviceContext::SetRenderTextures(const vdl::RenderTextures& _RenderTextures, const vdl::DepthStencilTexture& _DepthStenilTexture)
{
  auto GetRenderTargetView = [&](const vdl::RenderTexture& _RenderTexture)->ID3D11RenderTargetView *
  {
    assert(!_RenderTexture.isEmpty());

    return static_cast<CRenderTexture*>(pTextureManager_->GetTexture(_RenderTexture.GetID()))->pRenderTargetView.Get();
  };

  std::vector<ID3D11RenderTargetView*> pRenderTeargetViews(1);
  ID3D11DepthStencilView* pDepthStencilView;

  pRenderTeargetViews[0] = (_RenderTextures[0].isEmpty() ? pSwapChain_->GetRenderTargetView() : GetRenderTargetView(_RenderTextures[0]));

  for (vdl::uint RenderTextureCount = 1; RenderTextureCount < Constants::kMaxRenderTextureNum; ++RenderTextureCount)
  {
    if (_RenderTextures[RenderTextureCount].isEmpty())
    {
      break;
    }

    pRenderTeargetViews.push_back(GetRenderTargetView(_RenderTextures[RenderTextureCount]));
  }
  pDepthStencilView = (_DepthStenilTexture.isEmpty() ? pSwapChain_->GetDepthStencilView()
    : static_cast<CDepthStencilTexture*>(pTextureManager_->GetTexture(_DepthStenilTexture.GetID()))->pDepthStencilView.Get());
  assert(!pRenderTeargetViews.empty() && pDepthStencilView);

  pD3D11ImmediateContext_->OMSetRenderTargets(static_cast<vdl::uint>(pRenderTeargetViews.size()), pRenderTeargetViews.data(), pDepthStencilView);
}

void CDeviceContext::SetBlendState(const vdl::BlendState& _BlendState)
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
      _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
    }

    BlendStates_.insert(std::make_pair(_BlendState, pBlendState));
  }

  pD3D11ImmediateContext_->OMSetBlendState(BlendStates_.at(_BlendState).Get(), nullptr, 0xFFFFFFFF);
}

void CDeviceContext::SetDepthStencilState(const vdl::DepthStencilState& _DepthStencilState)
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
        DepthStencilDesc.StencilReadMask = _DepthStencilState.StencilReadMask;
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
      _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
    }

    DepthStencilStates_.insert(std::make_pair(_DepthStencilState, pDepthStencilState));
  }

  pD3D11ImmediateContext_->OMSetDepthStencilState(DepthStencilStates_.at(_DepthStencilState).Get(), 1);
}

void CDeviceContext::SetRasterizerState(const vdl::RasterizerState& _RasterizerState)
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
      _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
    }

    RasterizerStates_.insert(std::make_pair(_RasterizerState, pRasterizerState));
  }

  pD3D11ImmediateContext_->RSSetState(RasterizerStates_.at(_RasterizerState).Get());
}

void CDeviceContext::VSSetShader(const vdl::VertexShader& _VertexShader)
{
  ID3D11VertexShader* pVertexShader = nullptr;
  {
    if (!_VertexShader.isEmpty())
    {
      assert(pShaderManager_->GetShader(_VertexShader.GetID())->GetType() == ShaderType::eVertexShader);
      pVertexShader = static_cast<CVertexShader*>(pShaderManager_->GetShader(_VertexShader.GetID()))->pVertexShader.Get();
    }
  }

  pD3D11ImmediateContext_->VSSetShader(pVertexShader, nullptr, 0);
}

void CDeviceContext::VSSetShaderResources(vdl::uint _StartSlot, vdl::uint _ShaderResourceNum, const vdl::ShaderResource _ShaderResources[])
{
  std::vector<ID3D11ShaderResourceView*> pShaderResources(_ShaderResourceNum);
  {
    for (vdl::uint ShaderResourceCount = 0; ShaderResourceCount < _ShaderResourceNum; ++ShaderResourceCount)
    {
      pShaderResources[ShaderResourceCount] = GetShaderResourceView(_ShaderResources[ShaderResourceCount]);
    }
  }

  pD3D11ImmediateContext_->VSSetShaderResources(_StartSlot, _ShaderResourceNum, pShaderResources.data());
}

void CDeviceContext::VSSetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[])
{
  std::vector<ID3D11SamplerState*> pSamplers(_SamplerNum);
  {
    for (vdl::uint SamplerCount = 0; SamplerCount < _SamplerNum; ++SamplerCount)
    {
      pSamplers[SamplerCount] = GetSamplerState(_Samplers[SamplerCount]);
    }
  }

  pD3D11ImmediateContext_->VSSetSamplers(_StartSlot, _SamplerNum, pSamplers.data());
}

void CDeviceContext::VSSetConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[])
{
  std::vector<ID3D11Buffer*> pConstantBuffers(_ConstantBufferNum);
  {
    for (vdl::uint ConstantBufferCount = 0; ConstantBufferCount < _ConstantBufferNum; ++ConstantBufferCount)
    {
      pConstantBuffers[ConstantBufferCount] = GetConstantBuffer(_ConstantBuffers[ConstantBufferCount]);
    }
  }

  pD3D11ImmediateContext_->VSSetConstantBuffers(_StartSlot, _ConstantBufferNum, pConstantBuffers.data());
}

void CDeviceContext::HSSetShader(const vdl::HullShader& _HullShader)
{
  ID3D11HullShader* pHullShader = nullptr;
  {
    if (!_HullShader.isEmpty())
    {
      assert(pShaderManager_->GetShader(_HullShader.GetID())->GetType() == ShaderType::eHullShader);
      pHullShader = static_cast<CHullShader*>(pShaderManager_->GetShader(_HullShader.GetID()))->pHullShader.Get();
    }
  }

  pD3D11ImmediateContext_->HSSetShader(pHullShader, nullptr, 0);
}

void CDeviceContext::HSSetShaderResources(vdl::uint _StartSlot, vdl::uint _ShaderResourceNum, const vdl::ShaderResource _ShaderResources[])
{
  std::vector<ID3D11ShaderResourceView*> pShaderResources(_ShaderResourceNum);
  {
    for (vdl::uint ShaderResourceCount = 0; ShaderResourceCount < _ShaderResourceNum; ++ShaderResourceCount)
    {
      pShaderResources[ShaderResourceCount] = GetShaderResourceView(_ShaderResources[ShaderResourceCount]);
    }
  }

  pD3D11ImmediateContext_->HSSetShaderResources(_StartSlot, _ShaderResourceNum, pShaderResources.data());
}

void CDeviceContext::HSSetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[])
{
  std::vector<ID3D11SamplerState*> pSamplers(_SamplerNum);
  {
    for (vdl::uint SamplerCount = 0; SamplerCount < _SamplerNum; ++SamplerCount)
    {
      pSamplers[SamplerCount] = GetSamplerState(_Samplers[SamplerCount]);
    }
  }

  pD3D11ImmediateContext_->HSSetSamplers(_StartSlot, _SamplerNum, pSamplers.data());
}

void CDeviceContext::HSSetConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[])
{
  std::vector<ID3D11Buffer*> pConstantBuffers(_ConstantBufferNum);
  {
    for (vdl::uint ConstantBufferCount = 0; ConstantBufferCount < _ConstantBufferNum; ++ConstantBufferCount)
    {
      pConstantBuffers[ConstantBufferCount] = GetConstantBuffer(_ConstantBuffers[ConstantBufferCount]);
    }
  }

  pD3D11ImmediateContext_->HSSetConstantBuffers(_StartSlot, _ConstantBufferNum, pConstantBuffers.data());
}

void CDeviceContext::DSSetShader(const vdl::DomainShader& _DomainShader)
{
  ID3D11DomainShader* pDomainShader = nullptr;
  {
    if (!_DomainShader.isEmpty())
    {
      assert(pShaderManager_->GetShader(_DomainShader.GetID())->GetType() == ShaderType::eDomainShader);
      pDomainShader = static_cast<CDomainShader*>(pShaderManager_->GetShader(_DomainShader.GetID()))->pDomainShader.Get();
    }
  }

  pD3D11ImmediateContext_->DSSetShader(pDomainShader, nullptr, 0);
}

void CDeviceContext::DSSetShaderResources(vdl::uint _StartSlot, vdl::uint _ShaderResourceNum, const vdl::ShaderResource _ShaderResources[])
{
  std::vector<ID3D11ShaderResourceView*> pShaderResources(_ShaderResourceNum);
  {
    for (vdl::uint ShaderResourceCount = 0; ShaderResourceCount < _ShaderResourceNum; ++ShaderResourceCount)
    {
      pShaderResources[ShaderResourceCount] = GetShaderResourceView(_ShaderResources[ShaderResourceCount]);
    }
  }

  pD3D11ImmediateContext_->DSSetShaderResources(_StartSlot, _ShaderResourceNum, pShaderResources.data());
}

void CDeviceContext::DSSetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[])
{
  std::vector<ID3D11SamplerState*> pSamplers(_SamplerNum);
  {
    for (vdl::uint SamplerCount = 0; SamplerCount < _SamplerNum; ++SamplerCount)
    {
      pSamplers[SamplerCount] = GetSamplerState(_Samplers[SamplerCount]);
    }
  }

  pD3D11ImmediateContext_->DSSetSamplers(_StartSlot, _SamplerNum, pSamplers.data());
}

void CDeviceContext::DSSetConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[])
{
  std::vector<ID3D11Buffer*> pConstantBuffers(_ConstantBufferNum);
  {
    for (vdl::uint ConstantBufferCount = 0; ConstantBufferCount < _ConstantBufferNum; ++ConstantBufferCount)
    {
      pConstantBuffers[ConstantBufferCount] = GetConstantBuffer(_ConstantBuffers[ConstantBufferCount]);
    }
  }

  pD3D11ImmediateContext_->DSSetConstantBuffers(_StartSlot, _ConstantBufferNum, pConstantBuffers.data());
}

void CDeviceContext::GSSetShader(const vdl::GeometryShader& _GeometryShader)
{
  ID3D11GeometryShader* pGeometryShader = nullptr;
  {
    if (!_GeometryShader.isEmpty())
    {
      assert(pShaderManager_->GetShader(_GeometryShader.GetID())->GetType() == ShaderType::eGeometryShader);
      pGeometryShader = static_cast<CGeometryShader*>(pShaderManager_->GetShader(_GeometryShader.GetID()))->pGeometryShader.Get();
    }
  }

  pD3D11ImmediateContext_->GSSetShader(pGeometryShader, nullptr, 0);
}

void CDeviceContext::GSSetShaderResources(vdl::uint _StartSlot, vdl::uint _ShaderResourceNum, const vdl::ShaderResource _ShaderResources[])
{
  std::vector<ID3D11ShaderResourceView*> pShaderResources(_ShaderResourceNum);
  {
    for (vdl::uint ShaderResourceCount = 0; ShaderResourceCount < _ShaderResourceNum; ++ShaderResourceCount)
    {
      pShaderResources[ShaderResourceCount] = GetShaderResourceView(_ShaderResources[ShaderResourceCount]);
    }
  }

  pD3D11ImmediateContext_->GSSetShaderResources(_StartSlot, _ShaderResourceNum, pShaderResources.data());
}

void CDeviceContext::GSSetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[])
{
  std::vector<ID3D11SamplerState*> pSamplers(_SamplerNum);
  {
    for (vdl::uint SamplerCount = 0; SamplerCount < _SamplerNum; ++SamplerCount)
    {
      pSamplers[SamplerCount] = GetSamplerState(_Samplers[SamplerCount]);
    }
  }

  pD3D11ImmediateContext_->GSSetSamplers(_StartSlot, _SamplerNum, pSamplers.data());
}

void CDeviceContext::GSSetConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[])
{
  std::vector<ID3D11Buffer*> pConstantBuffers(_ConstantBufferNum);
  {
    for (vdl::uint ConstantBufferCount = 0; ConstantBufferCount < _ConstantBufferNum; ++ConstantBufferCount)
    {
      pConstantBuffers[ConstantBufferCount] = GetConstantBuffer(_ConstantBuffers[ConstantBufferCount]);
    }
  }

  pD3D11ImmediateContext_->GSSetConstantBuffers(_StartSlot, _ConstantBufferNum, pConstantBuffers.data());
}

void CDeviceContext::PSSetShader(const vdl::PixelShader& _PixelShader)
{
  ID3D11PixelShader* pPixelShader = nullptr;
  {
    if (!_PixelShader.isEmpty())
    {
      assert(pShaderManager_->GetShader(_PixelShader.GetID())->GetType() == ShaderType::ePixelShader);
      pPixelShader = static_cast<CPixelShader*>(pShaderManager_->GetShader(_PixelShader.GetID()))->pPixelShader.Get();
    }
  }

  pD3D11ImmediateContext_->PSSetShader(pPixelShader, nullptr, 0);
}

void CDeviceContext::PSSetShaderResources(vdl::uint _StartSlot, vdl::uint _ShaderResourceNum, const vdl::ShaderResource _ShaderResources[])
{
  std::vector<ID3D11ShaderResourceView*> pShaderResources(_ShaderResourceNum);
  {
    for (vdl::uint ShaderResourceCount = 0; ShaderResourceCount < _ShaderResourceNum; ++ShaderResourceCount)
    {
      pShaderResources[ShaderResourceCount] = GetShaderResourceView(_ShaderResources[ShaderResourceCount]);
    }
  }

  pD3D11ImmediateContext_->PSSetShaderResources(_StartSlot, _ShaderResourceNum, pShaderResources.data());
}

void CDeviceContext::PSSetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[])
{
  std::vector<ID3D11SamplerState*> pSamplers(_SamplerNum);
  {
    for (vdl::uint SamplerCount = 0; SamplerCount < _SamplerNum; ++SamplerCount)
    {
      pSamplers[SamplerCount] = GetSamplerState(_Samplers[SamplerCount]);
    }
  }

  pD3D11ImmediateContext_->PSSetSamplers(_StartSlot, _SamplerNum, pSamplers.data());
}

void CDeviceContext::PSSetConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[])
{
  std::vector<ID3D11Buffer*> pConstantBuffers(_ConstantBufferNum);
  {
    for (vdl::uint ConstantBufferCount = 0; ConstantBufferCount < _ConstantBufferNum; ++ConstantBufferCount)
    {
      pConstantBuffers[ConstantBufferCount] = GetConstantBuffer(_ConstantBuffers[ConstantBufferCount]);
    }
  }

  pD3D11ImmediateContext_->PSSetConstantBuffers(_StartSlot, _ConstantBufferNum, pConstantBuffers.data());
}

void CDeviceContext::CSSetShader(const vdl::ComputeShader& _ComputeShader)
{
  ID3D11ComputeShader* pComputeShader = nullptr;
  {
    if (!_ComputeShader.isEmpty())
    {
      assert(pShaderManager_->GetShader(_ComputeShader.GetID())->GetType() == ShaderType::eComputeShader);
      pComputeShader = static_cast<CComputeShader*>(pShaderManager_->GetShader(_ComputeShader.GetID()))->pComputeShader.Get();
    }
  }

  pD3D11ImmediateContext_->CSSetShader(pComputeShader, nullptr, 0);
}

void CDeviceContext::CSSetShaderResources(vdl::uint _StartSlot, vdl::uint _ShaderResourceNum, const vdl::ShaderResource _ShaderResources[])
{
  std::vector<ID3D11ShaderResourceView*> pShaderResources(_ShaderResourceNum);
  {
    for (vdl::uint ShaderResourceCount = 0; ShaderResourceCount < _ShaderResourceNum; ++ShaderResourceCount)
    {
      pShaderResources[ShaderResourceCount] = GetShaderResourceView(_ShaderResources[ShaderResourceCount]);
    }
  }

  pD3D11ImmediateContext_->CSSetShaderResources(_StartSlot, _ShaderResourceNum, pShaderResources.data());
}

void CDeviceContext::CSSetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[])
{
  std::vector<ID3D11SamplerState*> pSamplers(_SamplerNum);
  {
    for (vdl::uint SamplerCount = 0; SamplerCount < _SamplerNum; ++SamplerCount)
    {
      pSamplers[SamplerCount] = GetSamplerState(_Samplers[SamplerCount]);
    }
  }

  pD3D11ImmediateContext_->CSSetSamplers(_StartSlot, _SamplerNum, pSamplers.data());
}

void CDeviceContext::CSSetConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[])
{
  std::vector<ID3D11Buffer*> pConstantBuffers(_ConstantBufferNum);
  {
    for (vdl::uint ConstantBufferCount = 0; ConstantBufferCount < _ConstantBufferNum; ++ConstantBufferCount)
    {
      pConstantBuffers[ConstantBufferCount] = GetConstantBuffer(_ConstantBuffers[ConstantBufferCount]);
    }
  }

  pD3D11ImmediateContext_->CSSetConstantBuffers(_StartSlot, _ConstantBufferNum, pConstantBuffers.data());
}

void CDeviceContext::CSSetUnorderedAccessObjects(vdl::uint _StartSlot, vdl::uint _UnorderedAccessObjectNum, const vdl::UnorderedAccessObject _UnorderedAccessObjects[])
{
  //  TODO:
  //  シェーダーリソースの全解除
  {
    std::vector<ID3D11ShaderResourceView*> pShaderResourceViews(Constants::kMaxShaderResourceNum);

    pD3D11ImmediateContext_->VSSetShaderResources(0, Constants::kMaxShaderResourceNum, pShaderResourceViews.data());
    pD3D11ImmediateContext_->HSSetShaderResources(0, Constants::kMaxShaderResourceNum, pShaderResourceViews.data());
    pD3D11ImmediateContext_->DSSetShaderResources(0, Constants::kMaxShaderResourceNum, pShaderResourceViews.data());
    pD3D11ImmediateContext_->GSSetShaderResources(0, Constants::kMaxShaderResourceNum, pShaderResourceViews.data());
    pD3D11ImmediateContext_->PSSetShaderResources(0, Constants::kMaxShaderResourceNum, pShaderResourceViews.data());
  }

  std::vector<ID3D11UnorderedAccessView*> pUnorderedAccessBuffers(_UnorderedAccessObjectNum);
  {
    for (vdl::uint UnorderedAccessObjectCount = 0; UnorderedAccessObjectCount < _UnorderedAccessObjectNum; ++UnorderedAccessObjectCount)
    {
      pUnorderedAccessBuffers[UnorderedAccessObjectCount] = GetUnorderedAccessView(_UnorderedAccessObjects[UnorderedAccessObjectCount]);
    }
  }

  pD3D11ImmediateContext_->CSSetUnorderedAccessViews(_StartSlot, _UnorderedAccessObjectNum, pUnorderedAccessBuffers.data(), nullptr);
}

void CDeviceContext::ClearRenderTexture(const vdl::RenderTexture& _RenderTexture, const vdl::ColorF& _ClearColor)
{
  ID3D11RenderTargetView* pRenderTargetView;

  if (const vdl::ID ID = _RenderTexture.GetID();
    ID == std::nullopt)
  {
    pRenderTargetView = pSwapChain_->GetRenderTargetView();
  }
  else
  {
    assert(pTextureManager_->GetTexture(ID)->GetType() == TextureType::eRenderTexture);
    pRenderTargetView = static_cast<CRenderTexture*>(pTextureManager_->GetTexture(ID))->pRenderTargetView.Get();
  }

  pD3D11ImmediateContext_->ClearRenderTargetView(pRenderTargetView, &_ClearColor.Red);
}

void CDeviceContext::ClearDepthStencilTexture(const vdl::DepthStencilTexture& _DepthStencilTexture, float _ClearDepth, vdl::uint _ClearStencil)
{
  ID3D11DepthStencilView* pDepthStencilView;

  if (const vdl::ID ID = _DepthStencilTexture.GetID();
    ID == std::nullopt)
  {
    pDepthStencilView = pSwapChain_->GetDepthStencilView();
  }
  else
  {
    assert(pTextureManager_->GetTexture(ID)->GetType() == TextureType::eDepthStencilTexture);
    pDepthStencilView = static_cast<CDepthStencilTexture*>(pTextureManager_->GetTexture(ID))->pDepthStencilView.Get();
  }

  pD3D11ImmediateContext_->ClearDepthStencilView(pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, _ClearDepth, _ClearStencil);
}

void CDeviceContext::ClearUnorderedAccessTexture(const vdl::UnorderedAccessTexture& _UnorderedAccessTexture, const vdl::ColorF& _ClearColor)
{
  assert(!_UnorderedAccessTexture.isEmpty());
  assert(pTextureManager_->GetTexture(_UnorderedAccessTexture.GetID())->GetType() == TextureType::eUnorderedAccessTexture);

  pD3D11ImmediateContext_->ClearUnorderedAccessViewFloat(static_cast<CUnorderedAccessTexture*>(pTextureManager_->GetTexture(_UnorderedAccessTexture.GetID()))->pUnorderedAccessView.Get(), &_ClearColor.Red);
}

void CDeviceContext::Draw(vdl::uint _VertexCount, vdl::uint _InstanceCount, vdl::uint _FirstVertex, vdl::uint _FirstInstance)
{
  pD3D11ImmediateContext_->DrawInstanced(_VertexCount, _InstanceCount, _FirstVertex, _FirstInstance);
}

void CDeviceContext::DrawIndexed(vdl::uint _IndexCount, vdl::uint _InstanceCount, vdl::uint _FirstIndex, vdl::uint _VertexOffset, vdl::uint _FirstInstance)
{
  pD3D11ImmediateContext_->DrawIndexedInstanced(_IndexCount, _InstanceCount, _FirstIndex, _VertexOffset, _FirstInstance);
}

void CDeviceContext::Dispatch(vdl::uint _ThreadGroupX, vdl::uint _ThreadGroupY, vdl::uint _ThreadGroupZ)
{
  pD3D11ImmediateContext_->Dispatch(_ThreadGroupX, _ThreadGroupY, _ThreadGroupZ);
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
    case vdl::InputLayoutType::eMesh:
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
      _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
    }
  }

  InputLayouts_.insert(std::make_pair(_Key, pInputLayout));
}

ID3D11ShaderResourceView* CDeviceContext::GetShaderResourceView(const vdl::ShaderResource& _ShaderResource)
{
  ID3D11ShaderResourceView* pShaderResourceView = nullptr;
  {
    //  Texture
    if (std::get_if<vdl::Texture>(&_ShaderResource))
    {
      const vdl::Texture& Texture = std::get<vdl::Texture>(_ShaderResource);
      if (!Texture.isEmpty())
      {
        pShaderResourceView = static_cast<CTexture*>(pTextureManager_->GetTexture(Texture.GetID()))->pShaderResourceView.Get();
      }
    }
    //  UnorderedAccessBuffer
    else if (std::get_if<vdl::Detail::UnorderedAccessBufferData>(&_ShaderResource))
    {
      const vdl::Detail::UnorderedAccessBufferData& UnorderedAccessBuffer = std::get<vdl::Detail::UnorderedAccessBufferData>(_ShaderResource);

      if (!UnorderedAccessBuffer.isEmpty())
      {
        assert(pBufferManager_->GetBuffer(UnorderedAccessBuffer.GetID())->GetType() == BufferType::eUnorderedAccessBuffer);
        pShaderResourceView = static_cast<CUnordererdAccessBuffer*>(pBufferManager_->GetBuffer(UnorderedAccessBuffer.GetID()))->pShaderResourceView.Get();
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
        ::memcpy(SamplerDesc.BorderColor, &Cast(_Sampler.BorderColor), sizeof(float) * 4);
        SamplerDesc.MinLOD = 0;
        SamplerDesc.MaxLOD = FLT_MAX;
      }

      hr = pD3D11Device_->CreateSamplerState(&SamplerDesc, pSamplerState.GetAddressOf());
      _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
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
      assert(pBufferManager_->GetBuffer(_ConstantBuffer.GetID())->GetType() == BufferType::eConstantBuffer);
      pConstantBuffer = static_cast<CConstantBuffer*>(pBufferManager_->GetBuffer(_ConstantBuffer.GetID()))->pBuffer.Get();
    }
  }

  return pConstantBuffer;
}

ID3D11UnorderedAccessView* CDeviceContext::GetUnorderedAccessView(const vdl::UnorderedAccessObject& _UnorderedAccessObject)
{
  ID3D11UnorderedAccessView* pUnorderedAccessView = nullptr;
  {
    //  UnorderedAccessTexture
    if (std::get_if<vdl::UnorderedAccessTexture>(&_UnorderedAccessObject))
    {
      const vdl::UnorderedAccessTexture& UnorderedAccessTexture = std::get<vdl::UnorderedAccessTexture>(_UnorderedAccessObject);

      if (!UnorderedAccessTexture.isEmpty())
      {
        assert(pTextureManager_->GetTexture(UnorderedAccessTexture.GetID())->GetType() == TextureType::eUnorderedAccessTexture);
        pUnorderedAccessView = static_cast<CUnorderedAccessTexture*>(pTextureManager_->GetTexture(UnorderedAccessTexture.GetID()))->pUnorderedAccessView.Get();
      }
    }
    //  UnorderedAccessBuffer
    else if (std::get_if<vdl::Detail::UnorderedAccessBufferData>(&_UnorderedAccessObject))
    {
      const vdl::Detail::UnorderedAccessBufferData& UnorderedAccessBuffer = std::get<vdl::Detail::UnorderedAccessBufferData>(_UnorderedAccessObject);

      if (!UnorderedAccessBuffer.isEmpty())
      {
        assert(pBufferManager_->GetBuffer(UnorderedAccessBuffer.GetID())->GetType() == BufferType::eUnorderedAccessBuffer);
        pUnorderedAccessView = static_cast<CUnordererdAccessBuffer*>(pBufferManager_->GetBuffer(UnorderedAccessBuffer.GetID()))->pUnorderedAccessView.Get();
      }
    }
  }

  return pUnorderedAccessView;
}
