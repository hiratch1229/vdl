#include "CDevice.hpp"

#include <vdl/Engine.hpp>
#include <vdl/BufferManager/IBufferManager.hpp>

#include <vdl/Format/DirectX/Format.hpp>
#include <vdl/Shader/DirectX12/CShader.hpp>
#include <vdl/Misc/Windows/Misc.hpp>

#include <vdl/Constants/Constants.hpp>

#include <vdl/Image.hpp>
#include <vdl/Macro.hpp>
#include <vdl/DetectMemoryLeak.hpp>

#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <assert.h>

namespace
{
  constexpr const char* kShaderTargets[static_cast<vdl::uint>(ShaderType::eNum)] = { "vs_5_0", "hs_5_0", "ds_5_0", "gs_5_0", "ps_5_0", "cs_5_0" };

  inline void ComplieShader(ShaderData* _pShaderData, const char* _Target, const char* _FilePath, const char* _EntryPoint)
  {
    vdl::uint CompileFlag = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined _DEBUG | DEBUG
    CompileFlag |= D3DCOMPILE_DEBUG;
#endif

    wchar_t wFilePath[Constants::kMaxCharacterNum]{};
    ::mbstowcs_s(nullptr, wFilePath, _FilePath, Constants::kMaxCharacterNum);

    HRESULT hr = S_OK;

    Microsoft::WRL::ComPtr<ID3DBlob> pError;
    hr = ::D3DCompileFromFile(wFilePath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
      _EntryPoint, _Target, CompileFlag, 0, _pShaderData->pCode.GetAddressOf(), pError.GetAddressOf());
    _ASSERT_EXPR_A(SUCCEEDED(hr), static_cast<const char*>(pError->GetBufferPointer()));

    _pShaderData->ByteCode.pShaderBytecode = _pShaderData->pCode->GetBufferPointer();
    _pShaderData->ByteCode.BytecodeLength = _pShaderData->pCode->GetBufferSize();
  }

  inline void ComplieShader(ShaderData* _pShaderData, const char* _Target, const char* _Source, vdl::uint _DataSize, const char* _EntryPoint)
  {
    vdl::uint CompileFlag = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined _DEBUG | DEBUG
    CompileFlag |= D3DCOMPILE_DEBUG;
#endif

    HRESULT hr = S_OK;

    Microsoft::WRL::ComPtr<ID3DBlob> pError;
    hr = ::D3DCompile(_Source, _DataSize, nullptr, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
      _EntryPoint, _Target, CompileFlag, 0, _pShaderData->pCode.GetAddressOf(), pError.GetAddressOf());
    _ASSERT_EXPR_A(SUCCEEDED(hr), static_cast<const char*>(pError->GetBufferPointer()));

    _pShaderData->ByteCode.pShaderBytecode = _pShaderData->pCode->GetBufferPointer();
    _pShaderData->ByteCode.BytecodeLength = _pShaderData->pCode->GetBufferSize();
  }

  inline DXGI_FORMAT Cast(IndexType _IndexType)
  {
    switch (_IndexType)
    {
    case IndexType::eUint16:
      return DXGI_FORMAT_R16_UINT;
    case IndexType::eUint32:
      return DXGI_FORMAT_R32_UINT;
    default: assert(false);
    }

    return DXGI_FORMAT_UNKNOWN;
  }
}

void CDevice::Initialize()
{
  pBufferManager_ = Engine::Get<IBufferManager>();

  //  �G���[�`�F�b�N�p
  HRESULT hr = S_OK;

#if defined( DEBUG ) || defined( _DEBUG )
  constexpr vdl::uint kDxgiCreateFactoryFlag = DXGI_CREATE_FACTORY_DEBUG;
  //  �f�o�b�O���C���[��L���ɂ���
  {
    Microsoft::WRL::ComPtr<ID3D12Debug3> pDebug;

    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(pDebug.GetAddressOf()))))
    {
      //pDebug->SetEnableGPUBasedValidation(true);
      pDebug->EnableDebugLayer();
    }
  }
#else
  constexpr vdl::uint kDxgiCreateFactoryFlag = 0;
#endif

  constexpr D3D_FEATURE_LEVEL kMininumFeatureLevel = D3D_FEATURE_LEVEL_11_0;

  //  �t�@�N�g���[�̍쐬
  {
    Microsoft::WRL::ComPtr<IDXGIFactory2> pFactory2;

    hr = ::CreateDXGIFactory2(kDxgiCreateFactoryFlag, IID_PPV_ARGS(pFactory2.GetAddressOf()));
    _ASSERT_EXPR_A(SUCCEEDED(hr), hResultTrace(hr));

    pFactory2.As(&pFactory_);
  }

  bool useWrapAdapter = false;

  Microsoft::WRL::ComPtr<IDXGIAdapter4> pAdapter;
  {
    DXGI_ADAPTER_DESC3 AdapterDesc;

    for (vdl::uint i = 0; ; ++i)
    {
      //  �����\�ȕ�����D��ŗ�
      if (pFactory_->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(pAdapter.GetAddressOf())) == DXGI_ERROR_NOT_FOUND)
      {
        useWrapAdapter = true;
        break;
      }

      pAdapter->GetDesc3(&AdapterDesc);

      //  �n�[�h�E�F�A�A�_�v�^�[�ŃT�|�[�g����Ă�����̂�T��
      if (!(AdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE))
      {
        break;
      }
    }
  }

  if (useWrapAdapter)
  {
    hr = pFactory_->EnumWarpAdapter(IID_PPV_ARGS(pAdapter.GetAddressOf()));
    _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
  }

  hr = ::D3D12CreateDevice(pAdapter.Get(), kMininumFeatureLevel, IID_PPV_ARGS(pDevice_.GetAddressOf()));
  _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));

  ConstantBufferAllocater_.Initialize();

  //  �R�}���h�L���[�̍쐬
  {
    D3D12_COMMAND_QUEUE_DESC CommandQueueDesc;
    {
      CommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
      CommandQueueDesc.Priority = 0;
      CommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
      CommandQueueDesc.NodeMask = 0;
    }

    hr = pDevice_->CreateCommandQueue(&CommandQueueDesc, IID_PPV_ARGS(pCommandQueue_.GetAddressOf()));
    _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
  }

  //  �R�}���h�A���P�[�^�̍쐬
  {
    hr = pDevice_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(pCommandAllocator_.GetAddressOf()));
    _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
  }

  //  �R�}���h���X�g�̍쐬
  {
    hr = pDevice_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, pCommandAllocator_.Get(), nullptr, IID_PPV_ARGS(pCommandList_.GetAddressOf()));
    _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));

    hr = pCommandList_->Close();
    _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
  }

  //  �t�F���X�̍쐬
  {
    hr = pDevice_->CreateFence(FenceValue_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(pFence_.GetAddressOf()));
    _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));

    FenceEvent_ = CreateEvent(nullptr, false, false, nullptr);
    assert(FenceEvent_ != nullptr);
  }

#if defined( DEBUG ) || defined( _DEBUG )
  Microsoft::WRL::ComPtr<ID3D12InfoQueue> pInfoQueue_;
  pDevice_.As(&pInfoQueue_);

  pInfoQueue_->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
  pInfoQueue_->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);

  D3D12_MESSAGE_SEVERITY Serveritys[] =
  {
    D3D12_MESSAGE_SEVERITY_INFO
  };

  D3D12_MESSAGE_ID DenyIDs[] =
  {
    D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,
    D3D12_MESSAGE_ID_CLEARDEPTHSTENCILVIEW_MISMATCHINGCLEARVALUE,
  };

  D3D12_INFO_QUEUE_FILTER FileterDesc = {};
  {
    FileterDesc.DenyList.pIDList = DenyIDs;
    FileterDesc.DenyList.NumIDs = static_cast<vdl::uint>(vdl::Macro::ArraySize(DenyIDs));
    FileterDesc.DenyList.pSeverityList = Serveritys;
    FileterDesc.DenyList.NumSeverities = static_cast<vdl::uint>(vdl::Macro::ArraySize(Serveritys));
  }
  hr = pInfoQueue_->PushStorageFilter(&FileterDesc);
#endif
}

void CDevice::CreateVertexBuffer(IBuffer** _ppVertexBuffer, vdl::uint _BufferSize)
{
  assert(_ppVertexBuffer);

  CVertexBuffer* pVertexBuffer = new CVertexBuffer;
  CreateBuffer(&pVertexBuffer->BufferData, _BufferSize, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ);

  pVertexBuffer->View.BufferLocation = pVertexBuffer->BufferData.pResource->GetGPUVirtualAddress();
  pVertexBuffer->View.SizeInBytes = _BufferSize;

  (*_ppVertexBuffer) = std::move(pVertexBuffer);
}

void CDevice::CreateVertexBuffer(IBuffer** _ppVertexBuffer, const void* _Vertices, vdl::uint _BufferSize)
{
  assert(_ppVertexBuffer);

  CVertexBuffer* pVertexBuffer = new CVertexBuffer;
  CreateBuffer(&pVertexBuffer->BufferData, _BufferSize, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COPY_DEST);

  BufferData StagingBuffer;
  CreateStagingBuffer(&StagingBuffer, _Vertices, _BufferSize);
  CopyBuffer(&StagingBuffer, &pVertexBuffer->BufferData, _BufferSize, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

  pVertexBuffer->View.BufferLocation = pVertexBuffer->BufferData.pResource->GetGPUVirtualAddress();
  pVertexBuffer->View.SizeInBytes = _BufferSize;

  (*_ppVertexBuffer) = std::move(pVertexBuffer);
}

void CDevice::CreateInstanceBuffer(IBuffer** _ppInstanceBuffer, vdl::uint _BufferSize)
{
  assert(_ppInstanceBuffer);

  CInstanceBuffer* pInstanceBuffer = new CInstanceBuffer;
  pInstanceBuffer->BufferSize = _BufferSize * Constants::kInstanceBufferSizeMultiple;
  pInstanceBuffer->Offset = 0;
  CreateBuffer(&pInstanceBuffer->BufferData, pInstanceBuffer->BufferSize, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ);

  (*_ppInstanceBuffer) = std::move(pInstanceBuffer);
}

void CDevice::CreateIndexBuffer(IBuffer** _ppIndexBuffer, vdl::uint _BufferSize, IndexType _IndexType)
{
  assert(_ppIndexBuffer);

  CIndexBuffer* pIndexBuffer = new CIndexBuffer;
  CreateBuffer(&pIndexBuffer->BufferData, _BufferSize, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ);

  pIndexBuffer->View.BufferLocation = pIndexBuffer->BufferData.pResource->GetGPUVirtualAddress();
  pIndexBuffer->View.SizeInBytes = _BufferSize;
  pIndexBuffer->View.Format = Cast(_IndexType);

  (*_ppIndexBuffer) = std::move(pIndexBuffer);
}

void CDevice::CreateIndexBuffer(IBuffer** _ppIndexBuffer, const void* _Indices, vdl::uint _BufferSize, IndexType _IndexType)
{
  assert(_ppIndexBuffer);

  CIndexBuffer* pIndexBuffer = new CIndexBuffer;
  CreateBuffer(&pIndexBuffer->BufferData, _BufferSize, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COPY_DEST);

  BufferData StagingBuffer;
  CreateStagingBuffer(&StagingBuffer, _Indices, _BufferSize);
  CopyBuffer(&StagingBuffer, &pIndexBuffer->BufferData, _BufferSize, D3D12_RESOURCE_STATE_INDEX_BUFFER);

  pIndexBuffer->View.BufferLocation = pIndexBuffer->BufferData.pResource->GetGPUVirtualAddress();
  pIndexBuffer->View.SizeInBytes = _BufferSize;
  pIndexBuffer->View.Format = Cast(_IndexType);

  (*_ppIndexBuffer) = std::move(pIndexBuffer);
}

void CDevice::CreateConstantBuffer(IBuffer** _ppConstantBuffer, vdl::uint _BufferSize)
{
  assert(_ppConstantBuffer);

  CConstantBuffer* pConstantBuffer = new CConstantBuffer;
  pConstantBuffer->BufferSize = _BufferSize;
  CreateBuffer(&pConstantBuffer->BufferData, pConstantBuffer->BufferSize, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ);

  (*_ppConstantBuffer) = std::move(pConstantBuffer);
}

void CDevice::CreateUnorderedAccessBuffer(IBuffer** _ppUnorderedAccessBuffer, vdl::uint _Stride, vdl::uint _BufferSize, const void* _Buffer)
{
  assert(_ppUnorderedAccessBuffer);

  CUnordererdAccessBuffer* pUnorderedAccessBuffer = new CUnordererdAccessBuffer;
  pUnorderedAccessBuffer->BufferSize = _BufferSize;
  if (_Buffer)
  {
    CreateBuffer(&pUnorderedAccessBuffer->BufferData, pUnorderedAccessBuffer->BufferSize, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COPY_DEST);

    BufferData StagingBuffer;
    CreateStagingBuffer(&StagingBuffer, _Buffer, pUnorderedAccessBuffer->BufferSize);
    CopyBuffer(&StagingBuffer, &pUnorderedAccessBuffer->BufferData, _BufferSize, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
  }
  else
  {
    CreateBuffer(&pUnorderedAccessBuffer->BufferData, pUnorderedAccessBuffer->BufferSize, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
  }

  HRESULT hr = S_OK;

  //  �f�B�X�N���v�^�q�[�v�̍쐬
  {
    D3D12_DESCRIPTOR_HEAP_DESC ShaderResourceViewHeapDesc;
    {
      ShaderResourceViewHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
      ShaderResourceViewHeapDesc.NumDescriptors = 1;
      ShaderResourceViewHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
      ShaderResourceViewHeapDesc.NodeMask = 0;
    }

    hr = pDevice_->CreateDescriptorHeap(&ShaderResourceViewHeapDesc, IID_PPV_ARGS(pUnorderedAccessBuffer->pShaderResourceViewHeap.GetAddressOf()));
    _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
  }

  //  �r���[�̍쐬
  {
    D3D12_SHADER_RESOURCE_VIEW_DESC ShaderResourceViewDesc;
    {
      ShaderResourceViewDesc.Format = DXGI_FORMAT_UNKNOWN;
      ShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
      ShaderResourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
      ShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
      ShaderResourceViewDesc.Texture2D.MipLevels = 1;
      ShaderResourceViewDesc.Texture2D.PlaneSlice = 0;
      ShaderResourceViewDesc.Texture2D.ResourceMinLODClamp = 0.0f;
    }
    pDevice_->CreateShaderResourceView(pUnorderedAccessBuffer->BufferData.pResource.Get(), &ShaderResourceViewDesc, pUnorderedAccessBuffer->pShaderResourceViewHeap->GetCPUDescriptorHandleForHeapStart());

    D3D12_UNORDERED_ACCESS_VIEW_DESC UnorderedAccessView;
    {
      UnorderedAccessView.Format = DXGI_FORMAT_UNKNOWN;
      UnorderedAccessView.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
      UnorderedAccessView.Buffer.FirstElement = 0;
      UnorderedAccessView.Buffer.NumElements = pUnorderedAccessBuffer->BufferSize / _Stride;
      UnorderedAccessView.Buffer.StructureByteStride = _Stride;
      UnorderedAccessView.Buffer.CounterOffsetInBytes = 0;
      UnorderedAccessView.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
    }
    pDevice_->CreateUnorderedAccessView(pUnorderedAccessBuffer->BufferData.pResource.Get(), nullptr, &UnorderedAccessView, pUnorderedAccessBuffer->pUnordererdAccessViewHeap->GetCPUDescriptorHandleForHeapStart());
  }

  (*_ppUnorderedAccessBuffer) = std::move(pUnorderedAccessBuffer);
}

vdl::Detail::ConstantBufferData CDevice::CloneConstantBuffer(const vdl::Detail::ConstantBufferData& _ConstantBuffer)
{
  const CConstantBuffer* pSrcConstantBuffer = Cast<CConstantBuffer>(pBufferManager_->GetBuffer(_ConstantBuffer.GetID()));

  vdl::Detail::ConstantBufferData ConstantBuffer(pSrcConstantBuffer->BufferSize);
  {
    CCopyConstantBuffer* pCopyConstantBuffer = new CCopyConstantBuffer;
    {
      pCopyConstantBuffer->BufferSize = pSrcConstantBuffer->BufferSize;
      pCopyConstantBuffer->Offset = ConstantBufferAllocater_.Secure(pCopyConstantBuffer->BufferSize);
      CreateDescriptorHeap(pCopyConstantBuffer->pConstantBufferViewHeap.GetAddressOf(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    }

    D3D12_CONSTANT_BUFFER_VIEW_DESC ConstantBufferView;
    {
      ConstantBufferView.BufferLocation = Cast<CConstantBuffer>(ConstantBufferAllocater_.GetConstantBuffer())->BufferData.pResource->GetGPUVirtualAddress() + pCopyConstantBuffer->Offset;
      ConstantBufferView.SizeInBytes = (pCopyConstantBuffer->BufferSize + 255) & ~255;
    }
    pDevice_->CreateConstantBufferView(&ConstantBufferView, pCopyConstantBuffer->pConstantBufferViewHeap->GetCPUDescriptorHandleForHeapStart());

    ::memcpy(ConstantBufferAllocater_.GetBuffer(pCopyConstantBuffer->Offset), pSrcConstantBuffer->BufferData.pData, pCopyConstantBuffer->BufferSize);

    pBufferManager_->SetBuffer(ConstantBuffer.GetID(), pCopyConstantBuffer);
  }

  return ConstantBuffer;
}

void CDevice::CreateTexture(ITexture** _ppTexture, const vdl::Image& _Image)
{
  assert(_ppTexture);

  constexpr DXGI_FORMAT kTextureFormat = Cast(Constants::kTextureFormat);

  CTexture* pTexture = new CTexture;
  pTexture->TextureSize = _Image.GetTextureSize();

  HRESULT hr = S_OK;

  //  �f�B�X�N���v�^�q�[�v�̍쐬
  CreateDescriptorHeap(pTexture->TextureData.pShaderResourceViewHeap.GetAddressOf(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

  //  �V�F�[�_�[���\�[�X�̍쐬
  {
    D3D12_RESOURCE_DESC TextureDesc;
    {
      TextureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
      TextureDesc.Alignment = 0;
      TextureDesc.Width = pTexture->TextureSize.x;
      TextureDesc.Height = pTexture->TextureSize.y;
      TextureDesc.DepthOrArraySize = 1;
      TextureDesc.MipLevels = 1;
      TextureDesc.Format = kTextureFormat;
      TextureDesc.SampleDesc.Count = 1;
      TextureDesc.SampleDesc.Quality = 0;
      TextureDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
      TextureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
    }

    CreateResource(pTexture->TextureData.pResource.GetAddressOf(), TextureDesc, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COPY_DEST);
    pTexture->TextureData.ResourceState = D3D12_RESOURCE_STATE_COPY_DEST;

    UINT64 UploadSize;
    D3D12_PLACED_SUBRESOURCE_FOOTPRINT PlacedFootprint;
    pDevice_->GetCopyableFootprints(&TextureDesc, 0, 1, 0, &PlacedFootprint, nullptr, nullptr, &UploadSize);

    BufferData StagingBuffer;
    CreateStagingBuffer(&StagingBuffer, nullptr, static_cast<vdl::uint>(UploadSize));

    //  �������̏�������
    {
      const vdl::uint8_t* Buffer = &_Image.Buffer()->Red;

      const vdl::uint Stride = _Image.Stride();
      for (vdl::uint y = 0; y < pTexture->TextureSize.y; ++y)
      {
        ::memcpy(static_cast<vdl::uint8_t*>(StagingBuffer.pData) + PlacedFootprint.Footprint.RowPitch * y, Buffer + Stride * y, Stride);
      }
    }

    //  ���\�[�X�̃R�s�[
    {
      D3D12_TEXTURE_COPY_LOCATION DstCopyLocation;
      {
        DstCopyLocation.pResource = pTexture->TextureData.pResource.Get();
        DstCopyLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
        DstCopyLocation.SubresourceIndex = 0;
      }
      D3D12_TEXTURE_COPY_LOCATION SrcCopyLocation;
      {
        SrcCopyLocation.pResource = StagingBuffer.pResource.Get();
        SrcCopyLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
        SrcCopyLocation.PlacedFootprint = PlacedFootprint;
      }
      pCommandList_->Reset(pCommandAllocator_.Get(), nullptr);
      pCommandList_->CopyTextureRegion(&DstCopyLocation, 0, 0, 0, &SrcCopyLocation, nullptr);
      pTexture->TextureData.TransitionResourceBarrier(pCommandList_.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
      pCommandList_->Close();

      ExecuteAndWait(pCommandList_.Get());
    }

    //  �r���[�̍쐬
    {
      D3D12_SHADER_RESOURCE_VIEW_DESC ShaderResourceViewDesc;
      {
        ShaderResourceViewDesc.Format = TextureDesc.Format;
        ShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        ShaderResourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        ShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
        ShaderResourceViewDesc.Texture2D.MipLevels = 1;
        ShaderResourceViewDesc.Texture2D.PlaneSlice = 0;
        ShaderResourceViewDesc.Texture2D.ResourceMinLODClamp = 0.0f;
      }

      pDevice_->CreateShaderResourceView(pTexture->TextureData.pResource.Get(), &ShaderResourceViewDesc, pTexture->TextureData.pShaderResourceViewHeap->GetCPUDescriptorHandleForHeapStart());
    }
  }

  (*_ppTexture) = std::move(pTexture);
}

void CDevice::CreateCubeTexture(ITexture** _ppTexture, const std::array<vdl::Image, 6>& _Images)
{

}

void CDevice::CreateRenderTexture(ITexture** _ppRenderTexture, const vdl::uint2& _TextureSize, vdl::FormatType _Format)
{
  assert(*_ppRenderTexture);

  CRenderTexture* pRenderTexture = new CRenderTexture;
  pRenderTexture->TextureSize = _TextureSize;
  pRenderTexture->Format = _Format;
  pRenderTexture->DXFormat = Cast(_Format);

  //  �f�B�X�N���v�^�q�[�v�̍쐬
  {
    CreateDescriptorHeap(pRenderTexture->TextureData.pShaderResourceViewHeap.GetAddressOf(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    CreateDescriptorHeap(pRenderTexture->pRenderTargetViewHeap.GetAddressOf(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
  }

  //  �V�F�[�_�[���\�[�X�̍쐬
  {
    D3D12_RESOURCE_DESC TextureDesc;
    {
      TextureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
      TextureDesc.Alignment = 0;
      TextureDesc.Width = _TextureSize.x;
      TextureDesc.Height = _TextureSize.y;
      TextureDesc.DepthOrArraySize = 1;
      TextureDesc.MipLevels = 1;
      TextureDesc.Format = pRenderTexture->DXFormat;
      TextureDesc.SampleDesc.Count = 1;
      TextureDesc.SampleDesc.Quality = 0;
      TextureDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
      TextureDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
    }

    CreateResource(pRenderTexture->TextureData.pResource.GetAddressOf(), TextureDesc, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    pRenderTexture->TextureData.ResourceState = D3D12_RESOURCE_STATE_RENDER_TARGET;

    //  �r���[�̍쐬
    {
      D3D12_SHADER_RESOURCE_VIEW_DESC ShaderResourceViewDesc;
      {
        ShaderResourceViewDesc.Format = pRenderTexture->DXFormat;
        ShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        ShaderResourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        ShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
        ShaderResourceViewDesc.Texture2D.MipLevels = 1;
        ShaderResourceViewDesc.Texture2D.PlaneSlice = 0;
        ShaderResourceViewDesc.Texture2D.ResourceMinLODClamp = 0.0f;
      }

      pDevice_->CreateShaderResourceView(pRenderTexture->TextureData.pResource.Get(), &ShaderResourceViewDesc, pRenderTexture->TextureData.pShaderResourceViewHeap->GetCPUDescriptorHandleForHeapStart());

      D3D12_RENDER_TARGET_VIEW_DESC RenderTargetViewDesc;
      {
        RenderTargetViewDesc.Format = pRenderTexture->DXFormat;
        RenderTargetViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
        RenderTargetViewDesc.Texture2D.MipSlice = 0;
        RenderTargetViewDesc.Texture2D.PlaneSlice = 0;
      }

      pDevice_->CreateRenderTargetView(pRenderTexture->TextureData.pResource.Get(), &RenderTargetViewDesc, pRenderTexture->pRenderTargetViewHeap->GetCPUDescriptorHandleForHeapStart());
    }
  }

  (*_ppRenderTexture) = std::move(pRenderTexture);
}

void CDevice::CreateDepthStencilTexture(ITexture** _ppDepthStencilTexture, const vdl::uint2& _TextureSize, vdl::FormatType _Format)
{
  assert(_ppDepthStencilTexture);

  CDepthStencilTexture* pDepthStencilTexture = new CDepthStencilTexture;
  pDepthStencilTexture->TextureSize = _TextureSize;
  pDepthStencilTexture->Format = _Format;
  pDepthStencilTexture->DXFormat = Cast(_Format);

  //  �f�B�X�N���v�^�q�[�v�̍쐬
  {
    CreateDescriptorHeap(pDepthStencilTexture->TextureData.pShaderResourceViewHeap.GetAddressOf(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    CreateDescriptorHeap(pDepthStencilTexture->pDepthStencilViewHeap.GetAddressOf(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
  }

  //  �V�F�[�_�[���\�[�X�̍쐬
  {
    D3D12_RESOURCE_DESC TextureDesc;
    {
      TextureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
      TextureDesc.Alignment = 0;
      TextureDesc.Width = _TextureSize.x;
      TextureDesc.Height = _TextureSize.y;
      TextureDesc.DepthOrArraySize = 1;
      TextureDesc.MipLevels = 1;
      TextureDesc.Format = TextureFormatFromDepthStencilFormat(_Format);
      TextureDesc.SampleDesc.Count = 1;
      TextureDesc.SampleDesc.Quality = 0;
      TextureDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
      TextureDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
    }

    CreateResource(pDepthStencilTexture->TextureData.pResource.GetAddressOf(), TextureDesc, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_DEPTH_WRITE);
    pDepthStencilTexture->TextureData.ResourceState = D3D12_RESOURCE_STATE_DEPTH_WRITE;

    //  �r���[�̍쐬
    {
      D3D12_DEPTH_STENCIL_VIEW_DESC DepthStencilViewDesc;
      {
        DepthStencilViewDesc.Format = Cast(_Format);
        DepthStencilViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
        DepthStencilViewDesc.Flags = D3D12_DSV_FLAG_NONE;
        DepthStencilViewDesc.Texture2D.MipSlice = 0;
      }

      pDevice_->CreateDepthStencilView(pDepthStencilTexture->TextureData.pResource.Get(), &DepthStencilViewDesc, pDepthStencilTexture->pDepthStencilViewHeap->GetCPUDescriptorHandleForHeapStart());
    }
  }

  (*_ppDepthStencilTexture) = std::move(pDepthStencilTexture);
}

void CDevice::CreateUnorderedAccessTexture(ITexture** _ppUnorderedAccessTexture, const vdl::uint2& _TextureSize, vdl::FormatType _Format)
{
  assert(_ppUnorderedAccessTexture);

  CUnorderedAccessTexture* pUnorderedAccessTexture = new CUnorderedAccessTexture;
  pUnorderedAccessTexture->TextureSize = _TextureSize;
  pUnorderedAccessTexture->Format = _Format;

  const DXGI_FORMAT DXFormat = Cast(_Format);

  //  �f�B�X�N���v�^�q�[�v�̍쐬
  {
    CreateDescriptorHeap(pUnorderedAccessTexture->TextureData.pShaderResourceViewHeap.GetAddressOf(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    CreateDescriptorHeap(pUnorderedAccessTexture->pUnorderedAccessViewHeap.GetAddressOf(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
  }

  //  �V�F�[�_�[���\�[�X�̍쐬
  {
    D3D12_RESOURCE_DESC TextureDesc;
    {
      TextureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
      TextureDesc.Alignment = 0;
      TextureDesc.Width = _TextureSize.x;
      TextureDesc.Height = _TextureSize.y;
      TextureDesc.DepthOrArraySize = 1;
      TextureDesc.MipLevels = 1;
      TextureDesc.Format = DXFormat;
      TextureDesc.SampleDesc.Count = 1;
      TextureDesc.SampleDesc.Quality = 0;
      TextureDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
      TextureDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
    }

    CreateResource(pUnorderedAccessTexture->TextureData.pResource.GetAddressOf(), TextureDesc, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_DEPTH_WRITE);
    pUnorderedAccessTexture->TextureData.ResourceState = D3D12_RESOURCE_STATE_DEPTH_WRITE;

    //  �r���[�̍쐬
    {
      D3D12_SHADER_RESOURCE_VIEW_DESC ShaderResourceViewDesc;
      {
        ShaderResourceViewDesc.Format = DXFormat;
        ShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        ShaderResourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        ShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
        ShaderResourceViewDesc.Texture2D.MipLevels = 1;
        ShaderResourceViewDesc.Texture2D.PlaneSlice = 0;
        ShaderResourceViewDesc.Texture2D.ResourceMinLODClamp = 0.0f;
      }

      pDevice_->CreateShaderResourceView(pUnorderedAccessTexture->TextureData.pResource.Get(), &ShaderResourceViewDesc, pUnorderedAccessTexture->TextureData.pShaderResourceViewHeap->GetCPUDescriptorHandleForHeapStart());

      D3D12_UNORDERED_ACCESS_VIEW_DESC UnorderedAccessDesc;
      {
        UnorderedAccessDesc.Format = DXFormat;
        UnorderedAccessDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
        UnorderedAccessDesc.Texture2D.MipSlice = 1;
        UnorderedAccessDesc.Texture2D.PlaneSlice = 1;
      }

      pDevice_->CreateUnorderedAccessView(pUnorderedAccessTexture->TextureData.pResource.Get(), nullptr, &UnorderedAccessDesc, pUnorderedAccessTexture->pUnorderedAccessViewHeap->GetCPUDescriptorHandleForHeapStart());
    }
  }

  (*_ppUnorderedAccessTexture) = std::move(pUnorderedAccessTexture);
}

void CDevice::WriteMemory(IBuffer* _pDstBuffer, const void* _pSrcBuffer, vdl::uint _BufferSize)const
{
  switch (_pDstBuffer->GetType())
  {
  case BufferType::eVertexBuffer:
  {
    CVertexBuffer* pVertexBuffer = Cast<CVertexBuffer>(_pDstBuffer);
    assert(pVertexBuffer->BufferData.pData);
    ::memcpy(pVertexBuffer->BufferData.pData, _pSrcBuffer, _BufferSize);
  }
  break;
  case BufferType::eInstanceBuffer:
  {
    CInstanceBuffer* pInstanceBuffer = Cast<CInstanceBuffer>(_pDstBuffer);
    if (pInstanceBuffer->Offset + _BufferSize > pInstanceBuffer->BufferSize)
    {
      pInstanceBuffer->Offset = 0;
    }
    ::memcpy(static_cast<vdl::uint8_t*>(pInstanceBuffer->BufferData.pData) + pInstanceBuffer->Offset, _pSrcBuffer, _BufferSize);

    pInstanceBuffer->PreviousOffset = pInstanceBuffer->Offset;
    pInstanceBuffer->Offset += _BufferSize;
  }
  break;
  case BufferType::eIndexBuffer:
  {
    CIndexBuffer* pIndexBuffer = Cast<CIndexBuffer>(_pDstBuffer);
    assert(pIndexBuffer->BufferData.pData);
    ::memcpy(pIndexBuffer->BufferData.pData, _pSrcBuffer, _BufferSize);
  }
  break;
  default: assert(false);
  }
}

void CDevice::LoadShader(IShader** _ppShader, const char* _FilePath, const char* _EntryPoint, ShaderType _Type)
{
  assert(_ppShader);

  HRESULT hr = S_OK;

  const char* ShaderTarget = kShaderTargets[static_cast<vdl::uint>(_Type)];

  switch (_Type)
  {
    //case ShaderType::eVertexShader:
    //  break;
  case ShaderType::eHullShader:
  {
    CHullShader* pShader = new CHullShader;
    ::ComplieShader(&pShader->ShaderData, ShaderTarget, _FilePath, _EntryPoint);

    (*_ppShader) = std::move(pShader);
  }
  break;
  case ShaderType::eDomainShader:
  {
    CDomainShader* pShader = new CDomainShader;
    ::ComplieShader(&pShader->ShaderData, ShaderTarget, _FilePath, _EntryPoint);

    (*_ppShader) = std::move(pShader);
  }
  break;
  case ShaderType::eGeometryShader:
  {
    CGeometryShader* pShader = new CGeometryShader;
    ::ComplieShader(&pShader->ShaderData, ShaderTarget, _FilePath, _EntryPoint);

    (*_ppShader) = std::move(pShader);
  }
  break;
  case ShaderType::ePixelShader:
  {
    CPixelShader* pShader = new CPixelShader;
    ::ComplieShader(&pShader->ShaderData, ShaderTarget, _FilePath, _EntryPoint);

    (*_ppShader) = std::move(pShader);
  }
  break;
  case ShaderType::eComputeShader:
  {
    CComputeShader* pShader = new CComputeShader;
    ::ComplieShader(&pShader->ShaderData, ShaderTarget, _FilePath, _EntryPoint);

    (*_ppShader) = std::move(pShader);
  }
  break;
  default: assert(false);
  }

}

void CDevice::LoadShader(IShader** _ppShader, const char* _Source, vdl::uint _DataSize, const char* _EntryPoint, ShaderType _Type)
{
  assert(_ppShader);

  HRESULT hr = S_OK;

  const char* ShaderTarget = kShaderTargets[static_cast<vdl::uint>(_Type)];

  switch (_Type)
  {
    //case ShaderType::eVertexShader:
    //  break;
  case ShaderType::eHullShader:
  {
    CHullShader* pShader = new CHullShader;
    ::ComplieShader(&pShader->ShaderData, ShaderTarget, _Source, _DataSize, _EntryPoint);

    (*_ppShader) = std::move(pShader);
  }
  break;
  case ShaderType::eDomainShader:
  {
    CDomainShader* pShader = new CDomainShader;
    ::ComplieShader(&pShader->ShaderData, ShaderTarget, _Source, _DataSize, _EntryPoint);

    (*_ppShader) = std::move(pShader);
  }
  break;
  case ShaderType::eGeometryShader:
  {
    CGeometryShader* pShader = new CGeometryShader;
    ::ComplieShader(&pShader->ShaderData, ShaderTarget, _Source, _DataSize, _EntryPoint);

    (*_ppShader) = std::move(pShader);
  }
  break;
  case ShaderType::ePixelShader:
  {
    CPixelShader* pShader = new CPixelShader;
    ::ComplieShader(&pShader->ShaderData, ShaderTarget, _Source, _DataSize, _EntryPoint);

    *_ppShader = std::move(pShader);
  }
  break;
  case ShaderType::eComputeShader:
  {
    CComputeShader* pShader = new CComputeShader;
    ::ComplieShader(&pShader->ShaderData, ShaderTarget, _Source, _DataSize, _EntryPoint);

    (*_ppShader) = std::move(pShader);
  }
  break;
  default: assert(false);
  }
}

void CDevice::LoadShader(IVertexShader** _ppVertexShader, const char* _FilePath, const char* _EntryPoint, vdl::InputLayoutType _InputLayout)
{
  assert(_ppVertexShader);

  CVertexShader* pShader = new CVertexShader;
  pShader->InputLayout = _InputLayout;

  constexpr const char* kShaderTarget = kShaderTargets[static_cast<vdl::uint>(ShaderType::eVertexShader)];

  HRESULT hr = S_OK;

  ::ComplieShader(&pShader->ShaderData, kShaderTarget, _FilePath, _EntryPoint);
  (*_ppVertexShader) = std::move(pShader);
}

void CDevice::LoadShader(IVertexShader** _ppVertexShader, const char* _Source, vdl::uint _DataSize, const char* _EntryPoint, vdl::InputLayoutType _InputLayout)
{
  assert(_ppVertexShader);

  CVertexShader* pShader = new CVertexShader;
  pShader->InputLayout = _InputLayout;

  constexpr const char* kShaderTarget = kShaderTargets[static_cast<vdl::uint>(ShaderType::eVertexShader)];

  HRESULT hr = S_OK;

  ::ComplieShader(&pShader->ShaderData, kShaderTarget, _Source, _DataSize, _EntryPoint);
  (*_ppVertexShader) = std::move(pShader);
}

//--------------------------------------------------

void CDevice::CreateDescriptorHeap(ID3D12DescriptorHeap** _ppDescriptorHeap, D3D12_DESCRIPTOR_HEAP_TYPE _Type)
{
  HRESULT hr = S_OK;

  D3D12_DESCRIPTOR_HEAP_DESC DescriptorHeapDesc;
  {
    DescriptorHeapDesc.Type = _Type;
    DescriptorHeapDesc.NumDescriptors = 1;
    DescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    DescriptorHeapDesc.NodeMask = 0;
  }

  hr = pDevice_->CreateDescriptorHeap(&DescriptorHeapDesc, IID_PPV_ARGS(_ppDescriptorHeap));
  _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
}

void CDevice::ExecuteAndWait(ID3D12CommandList* _pCommandList)
{
  assert(_pCommandList);

  pCommandQueue_->ExecuteCommandLists(1, &_pCommandList);

  pCommandQueue_->Signal(pFence_.Get(), ++FenceValue_);

  if (pFence_->GetCompletedValue() < FenceValue_)
  {
    pFence_->SetEventOnCompletion(FenceValue_, FenceEvent_);
    ::WaitForSingleObject(FenceEvent_, INFINITE);
  }
}

//--------------------------------------------------

void CDevice::CreateResource(ID3D12Resource** _ppResource, const D3D12_RESOURCE_DESC& _ResourceDesc, D3D12_HEAP_TYPE _HeapType, D3D12_RESOURCE_STATES _InitialResourceState)const
{
  assert(_ppResource);

  HRESULT hr = S_OK;

  D3D12_HEAP_PROPERTIES HeapProperties;
  {
    HeapProperties.Type = _HeapType;
    HeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    HeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    HeapProperties.CreationNodeMask = 1;
    HeapProperties.VisibleNodeMask = 1;
  }

  hr = pDevice_->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE, &_ResourceDesc, _InitialResourceState, nullptr, IID_PPV_ARGS(_ppResource));
  _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
}

void CDevice::CreateBuffer(BufferData* _pBuffer, vdl::uint _BufferSize, D3D12_HEAP_TYPE _HeapType, D3D12_RESOURCE_STATES _InitialResourceState)const
{
  assert(_pBuffer);

  D3D12_RESOURCE_DESC ResourceDesc;
  {
    ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    ResourceDesc.Alignment = 0;
    ResourceDesc.Width = _BufferSize;
    ResourceDesc.Height = 1;
    ResourceDesc.DepthOrArraySize = 1;
    ResourceDesc.MipLevels = 1;
    ResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
    ResourceDesc.SampleDesc.Count = 1;
    ResourceDesc.SampleDesc.Quality = 0;
    ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    ResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
  }

  CreateResource(_pBuffer->pResource.GetAddressOf(), ResourceDesc, _HeapType, _InitialResourceState);

  //  �X�e�[�W���O���Ȃ����|�C���^���擾����
  if (_HeapType == D3D12_HEAP_TYPE_UPLOAD)
  {
    HRESULT hr = S_OK;

    hr = _pBuffer->pResource->Map(0, nullptr, &_pBuffer->pData);
    _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
  }
}

void CDevice::CreateStagingBuffer(BufferData* _pBuffer, const void* _Buffer, vdl::uint _BufferSize)const
{
  assert(_pBuffer);

  CreateBuffer(_pBuffer, _BufferSize, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ);

  //  �������֏�������
  if (_Buffer)
  {
    ::memcpy(_pBuffer->pData, _Buffer, _BufferSize);
  }
}

void CDevice::CopyBuffer(BufferData* _pSrcBuffer, BufferData* _pDstBuffer, vdl::uint _BufferSize, D3D12_RESOURCE_STATES _AfterState)
{
  assert(_pSrcBuffer && _pDstBuffer);

  HRESULT hr = S_OK;

  hr = pCommandList_->Reset(pCommandAllocator_.Get(), nullptr);
  _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));

  pCommandList_->CopyResource(_pDstBuffer->pResource.Get(), _pSrcBuffer->pResource.Get());

  D3D12_RESOURCE_BARRIER ResourceBarrier;
  {
    ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    ResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    ResourceBarrier.Transition.pResource = _pDstBuffer->pResource.Get();
    ResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    ResourceBarrier.Transition.StateAfter = _AfterState;
  }
  pCommandList_->ResourceBarrier(1, &ResourceBarrier);
  pCommandList_->Close();

  ExecuteAndWait(pCommandList_.Get());
}
