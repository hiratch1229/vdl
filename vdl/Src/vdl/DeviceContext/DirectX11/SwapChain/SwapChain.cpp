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

  //  �G���[�`�F�b�N�p
  HRESULT hr = S_OK;

  //  �o�b�N�o�b�t�@�̍쐬
  {
    Microsoft::WRL::ComPtr<ID3D11Texture2D> pBackBuffer;
    hr = pSwapChain_->GetBuffer(0, IID_PPV_ARGS(pBackBuffer.GetAddressOf()));
    ERROR_CHECK(hr);

    hr = _pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, pRenderTargetView_.GetAddressOf());
    ERROR_CHECK(hr);
  }

  //  �����_�[�e�N�X�`���̍쐬
  {
    RenderTextures_[0] = vdl::RenderTexture(Constants::kDefaultWindowSize, vdl::FormatType::eSwapChain);

    CSwapChainRenderTexture* pRenderTexture = new CSwapChainRenderTexture;
    {
      pRenderTexture->TextureSize = Constants::kDefaultWindowSize;
      pRenderTexture->pRenderTargetView = pRenderTargetView_.Get();
    }
    Engine::Get<ITextureManager>()->SetTexture(RenderTextures_[0].GetID(), pRenderTexture);
  }

  //  �[�x�X�e���V���e�N�X�`���̍쐬
  DepthStencilTexture_ = vdl::DepthStencilTexture(Constants::kDefaultWindowSize, vdl::FormatType::eDefaultDepthStencil);
}
