#include "CSwapChain.hpp"

#include <vdl/Engine.hpp>
#include <vdl/Window/IWindow.hpp>
#include <vdl/Device/DirectX12/CDevice.hpp>
#include <vdl/DeviceContext/DirectX12/CDeviceContext.hpp>
#include <vdl/Renderer/IRenderer.hpp>
#include <vdl/TextureManager/ITextureManager.hpp>

#include <vdl/Format/DirectX/Format.hpp>
#include <vdl/Misc/Windows/Misc.hpp>

void CSwapChain::Initialize()
{
  pWindow_ = Engine::Get<IWindow>();
  pDeviceContext_ = Cast<CDeviceContext>(Engine::Get<IDeviceContext>());
  pRenderer_ = Engine::Get<IRenderer>();

  CDevice* pDevice = Cast<CDevice>(Engine::Get<IDevice>());
  ID3D12Device5* pD3D12Device = pDevice->GetDevice();
  pSwapChain_ = pDevice->GetSwapChain();
  
  DescriptorAllocator* pRenderTextureHeapAllocator = pDevice->GetDescriptorAllocator(DescriptorHeapType::eRTV);

  constexpr DXGI_FORMAT kSwapChainFormat = Cast(vdl::FormatType::eSwapChain);

  //  �G���[�`�F�b�N�p
  HRESULT hr = S_OK;

  //  �o�b�N�o�b�t�@�̍쐬
  {
    //  �o�b�N�o�b�t�@�̃C���f�b�N�X���擾
    CurrentBufferIndex_ = pSwapChain_->GetCurrentBackBufferIndex();

    D3D12_DESCRIPTOR_HEAP_DESC RenderTargetDescriptorHeapDesc;
    {
      RenderTargetDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
      RenderTargetDescriptorHeapDesc.NumDescriptors = 1;
      RenderTargetDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
      RenderTargetDescriptorHeapDesc.NodeMask = 0;
    }

    const vdl::uint RenderTargetViewDescriptorSize = pD3D12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    D3D12RenderTextures_.resize(Constants::kBackBufferNum);
    for (vdl::uint i = 0; i < Constants::kBackBufferNum; ++i)
    {
      CRenderTexture& D3D12RenderTexture = D3D12RenderTextures_[i];

      D3D12RenderTexture.TextureSize = Constants::kDefaultWindowSize;
      D3D12RenderTexture.DXFormat = kSwapChainFormat;
      D3D12RenderTexture.Format = vdl::FormatType::eSwapChain;
      //D3D12RenderTexture.TextureData.ResourceState = D3D12_RESOURCE_STATE_PRESENT;

      //  �����_�[�^�[�Q�b�g�̃f�X�N���v�^�q�[�v�쐬
      pRenderTextureHeapAllocator->Allocate(&D3D12RenderTexture.RenderTargetViewHeap);

      //  �����_�[�^�[�Q�b�g�r���[�̍쐬
      {
        hr = pSwapChain_->GetBuffer(i, IID_PPV_ARGS(D3D12RenderTexture.TextureData.pResource.GetAddressOf()));
        ERROR_CHECK(hr);

        pD3D12Device->CreateRenderTargetView(D3D12RenderTexture.TextureData.pResource.Get(), nullptr, D3D12RenderTexture.RenderTargetViewHeap.CPUHandle);
      }
    }
  }

  //  �����_�[�e�N�X�`���̍쐬
  {
    RenderTextures_[0] = vdl::RenderTexture(Constants::kDefaultWindowSize, vdl::FormatType::eSwapChain);

    CSwapChainRenderTexture* pRenderTexture = new CSwapChainRenderTexture;

    Engine::Get<ITextureManager>()->SetTexture(RenderTextures_[0].GetID(), pRenderTexture);
  }

  //  �[�x�X�e���V���e�N�X�`���̍쐬
  DepthStencilTexture_ = vdl::DepthStencilTexture(Constants::kDefaultWindowSize, vdl::FormatType::eDefaultDepthStencil);
}

void CSwapChain::ScreenClear()
{
  pDeviceContext_->ClearRenderTexture(RenderTextures_[0], pWindow_->GetScreenClearColor());
  pDeviceContext_->ClearDepthStencilTexture(DepthStencilTexture_, Constants::kDefaultClearDepth, Constants::kDefaultClearStencil);
  pRenderer_->SetRenderTextures(RenderTextures_, DepthStencilTexture_);
}

void CSwapChain::Present()
{
  pDeviceContext_->Present();

  ++CurrentBufferIndex_ %= Constants::kBackBufferNum;
}

void CSwapChain::ChangeWindowMode()
{
  //  TODO
}

void CSwapChain::ScreenShot()
{
  //  TODO
}
