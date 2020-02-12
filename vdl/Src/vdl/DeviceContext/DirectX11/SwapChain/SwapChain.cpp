#include "SwapChain.hpp"

#include <vdl/Engine.hpp>
#include <vdl/TextureManager/ITextureManager.hpp>
#include <vdl/Texture/DirectX11/CTexture.hpp>

#include <vdl/Format/Format.hpp>
#include <vdl/Constants/Constants.hpp>

#include <vdl/Misc/Windows/Misc.hpp>

void SwapChain::Initialize(ID3D11Device* _pDevice, IDXGISwapChain* _pSwapChain)
{
  pSwapChain_ = _pSwapChain;

  //  エラーチェック用
  HRESULT hr = S_OK;

  //  バックバッファの作成
  {
    Microsoft::WRL::ComPtr<ID3D11Texture2D> pBackBuffer;
    hr = pSwapChain_->GetBuffer(0, IID_PPV_ARGS(pBackBuffer.GetAddressOf()));
    ERROR_CHECK(hr);

    hr = _pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, pRenderTargetView_.GetAddressOf());
    ERROR_CHECK(hr);
  }

  //  レンダーテクスチャの作成
  {
    RenderTextures_[0] = vdl::RenderTexture(Constants::kDefaultWindowSize, vdl::FormatType::eSwapChain);

    CSwapChainRenderTexture* pRenderTexture = new CSwapChainRenderTexture;

    Engine::Get<ITextureManager>()->SetTexture(RenderTextures_[0].GetID(), pRenderTexture);
  }

  //  深度ステンシルテクスチャの作成
  DepthStencilTexture_ = vdl::DepthStencilTexture(Constants::kDefaultWindowSize, vdl::FormatType::eDefaultDepthStencil);
}
