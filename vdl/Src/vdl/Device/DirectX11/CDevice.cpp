#include "CDevice.hpp"

#include <vdl/Image.hpp>
#include <vdl/Macro.hpp>

#include <vdl/Constants/Constants.hpp>

#include <vdl/Format/DirectX/Format.hpp>
#include <vdl/Buffer/DirectX11/CBuffer.hpp>
#include <vdl/Texture/DirectX11/CTexture.hpp>
#include <vdl/Shader/DirectX11/CShader.hpp>
#include <vdl/Misc/Windows/Misc.hpp>

#include <d3dcompiler.h>
#include <assert.h>

namespace
{
  inline void ComplieShader(ID3D10Blob** _pCode, const char* _Target, const char* _Source, vdl::uint _DataSize, const char* _EntryPoint)
  {
    vdl::uint CompileFlag = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined _DEBUG | DEBUG
    CompileFlag |= D3DCOMPILE_DEBUG;
#endif

    HRESULT hr = S_OK;

    Microsoft::WRL::ComPtr<ID3D10Blob> pError;
    hr = ::D3DCompile(_Source, _DataSize, nullptr, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
      _EntryPoint, _Target, CompileFlag, 0, _pCode, pError.GetAddressOf());
    _ASSERT_EXPR_A(SUCCEEDED(hr), static_cast<const char*>(pError->GetBufferPointer()));
  }
}

void CDevice::Initialize()
{
  //  エラーチェック用
  HRESULT hr = S_OK;

  //  デバイスフラグ
  vdl::uint CreateDeviceFlag = 0;
  vdl::uint DxgiCreateFactoryFlag = 0;
#if defined( DEBUG ) || defined( _DEBUG )
  CreateDeviceFlag |= D3D11_CREATE_DEVICE_DEBUG;
  DxgiCreateFactoryFlag |= DXGI_CREATE_FACTORY_DEBUG;
#endif

  constexpr D3D_DRIVER_TYPE kDriverTypes[] =
  {
    D3D_DRIVER_TYPE_UNKNOWN,
    D3D_DRIVER_TYPE_HARDWARE,
    D3D_DRIVER_TYPE_WARP,
    D3D_DRIVER_TYPE_REFERENCE
  };

  constexpr D3D_FEATURE_LEVEL kFeatureLevels[] =
  {
    D3D_FEATURE_LEVEL_11_1,
    D3D_FEATURE_LEVEL_11_0,
    D3D_FEATURE_LEVEL_10_1,
    D3D_FEATURE_LEVEL_10_0,
    D3D_FEATURE_LEVEL_9_3,
    D3D_FEATURE_LEVEL_9_2,
    D3D_FEATURE_LEVEL_9_1
  };
  constexpr vdl::uint kFeatureLevelNum = vdl::Macro::ArraySize(kFeatureLevels);

  Microsoft::WRL::ComPtr<IDXGIFactory7> pFactory;
  {
    hr = ::CreateDXGIFactory2(DxgiCreateFactoryFlag, IID_PPV_ARGS(pFactory.GetAddressOf()));
    _ASSERT_EXPR_A(SUCCEEDED(hr), hResultTrace(hr));
  }

  bool useWrapAdapter = false;

  Microsoft::WRL::ComPtr<IDXGIAdapter4> pAdapter;
  {
    DXGI_ADAPTER_DESC3 AdapterDesc;

    for (vdl::uint i = 0; ; ++i)
    {
      //  高性能な物から優先で列挙
      if (pFactory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(pAdapter.GetAddressOf())) == DXGI_ERROR_NOT_FOUND)
      {
        useWrapAdapter = true;
        break;
      }

      pAdapter->GetDesc3(&AdapterDesc);

      //  ハードウェアアダプターでサポートされているものを探す
      if (!(AdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE))
      {
        break;
      }
    }
  }

  if (useWrapAdapter)
  {
    hr = pFactory->EnumWarpAdapter(IID_PPV_ARGS(pAdapter.GetAddressOf()));
    _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
  }

  D3D_FEATURE_LEVEL FeatureLevel;
  for (auto i : kDriverTypes)
  {
    //  デバイスを作成
    hr = ::D3D11CreateDevice(pAdapter.Get(), kDriverTypes[i], nullptr, CreateDeviceFlag, kFeatureLevels,
      kFeatureLevelNum, D3D11_SDK_VERSION, pDevice_.GetAddressOf(), &FeatureLevel, pImmediateContext_.GetAddressOf());

    //  成功時に終了
    if (SUCCEEDED(hr))
    {
      break;
    }
  }
  _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
}

void CDevice::CreateVertexBuffer(IBuffer** _ppVertexBuffer, vdl::uint _Stride, vdl::uint _BufferSize)
{
  assert(*_ppVertexBuffer);

  CVertexBuffer* pVertexBuffer = new CVertexBuffer;
  pVertexBuffer->Stride = _Stride;

  HRESULT hr = S_OK;

  D3D11_BUFFER_DESC BufferDesc;
  {
    BufferDesc.ByteWidth = _BufferSize;
    BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    BufferDesc.MiscFlags = 0;
    BufferDesc.StructureByteStride = 0;
  }

  hr = pDevice_->CreateBuffer(&BufferDesc, nullptr, pVertexBuffer->pBuffer.GetAddressOf());
  _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));

  (*_ppVertexBuffer) = std::move(pVertexBuffer);
}

void CDevice::CreateVertexBuffer(IBuffer** _ppVertexBuffer, const void* _Vertices, vdl::uint _Stride, vdl::uint _BufferSize)
{
  assert(*_ppVertexBuffer);

  CVertexBuffer* pVertexBuffer = new CVertexBuffer;
  pVertexBuffer->Stride = _Stride;

  HRESULT hr = S_OK;

  D3D11_BUFFER_DESC BufferDesc;
  {
    BufferDesc.ByteWidth = _BufferSize;
    BufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    BufferDesc.CPUAccessFlags = 0;
    BufferDesc.MiscFlags = 0;
    BufferDesc.StructureByteStride = 0;
  }

  D3D11_SUBRESOURCE_DATA InitialData;
  {
    InitialData.pSysMem = _Vertices;
    InitialData.SysMemPitch = 0;
    InitialData.SysMemSlicePitch = 0;
  }

  hr = pDevice_->CreateBuffer(&BufferDesc, &InitialData, pVertexBuffer->pBuffer.GetAddressOf());
  _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));

  (*_ppVertexBuffer) = std::move(pVertexBuffer);
}

void CDevice::CreateInstanceBuffer(IBuffer** _ppInstanceBuffer, vdl::uint _Stride, vdl::uint _BufferSize)
{
  assert(*_ppInstanceBuffer);

  CInstanceBuffer* pInstanceBuffer = new CInstanceBuffer;
  pInstanceBuffer->Stride = _Stride;

  HRESULT hr = S_OK;

  D3D11_BUFFER_DESC BufferDesc;
  {
    BufferDesc.ByteWidth = _BufferSize;
    BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    BufferDesc.MiscFlags = 0;
    BufferDesc.StructureByteStride = 0;
  }

  hr = pDevice_->CreateBuffer(&BufferDesc, nullptr, pInstanceBuffer->pBuffer.GetAddressOf());
  _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));

  (*_ppInstanceBuffer) = std::move(pInstanceBuffer);
}

void CDevice::CreateIndexBuffer(IBuffer** _ppIndexBuffer, vdl::uint _BufferSize, IndexType _IndexType)
{
  assert(*_ppIndexBuffer);

  CIndexBuffer* pIndexBuffer = new CIndexBuffer;
  pIndexBuffer->IndexType = _IndexType;

  HRESULT hr = S_OK;

  D3D11_BUFFER_DESC BufferDesc;
  {
    BufferDesc.ByteWidth = _BufferSize;
    BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    BufferDesc.MiscFlags = 0;
    BufferDesc.StructureByteStride = 0;
  }

  hr = pDevice_->CreateBuffer(&BufferDesc, nullptr, pIndexBuffer->pBuffer.GetAddressOf());
  _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));

  (*_ppIndexBuffer) = std::move(pIndexBuffer);
}

void CDevice::CreateIndexBuffer(IBuffer** _ppIndexBuffer, const void* _Indices, vdl::uint _BufferSize, IndexType _IndexType)
{
  assert(*_ppIndexBuffer);

  CIndexBuffer* pIndexBuffer = new CIndexBuffer;
  pIndexBuffer->IndexType = _IndexType;

  HRESULT hr = S_OK;

  D3D11_BUFFER_DESC BufferDesc;
  {
    BufferDesc.ByteWidth = _BufferSize;
    BufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    BufferDesc.CPUAccessFlags = 0;
    BufferDesc.MiscFlags = 0;
    BufferDesc.StructureByteStride = 0;
  }

  D3D11_SUBRESOURCE_DATA InitialData;
  {
    InitialData.pSysMem = _Indices;
    InitialData.SysMemPitch = 0;
    InitialData.SysMemSlicePitch = 0;
  }

  hr = pDevice_->CreateBuffer(&BufferDesc, &InitialData, pIndexBuffer->pBuffer.GetAddressOf());
  _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));

  (*_ppIndexBuffer) = std::move(pIndexBuffer);
}

void CDevice::CreateConstantBuffer(IBuffer** _ppConstantBuffer, vdl::uint _BufferSize)
{
  assert(*_ppConstantBuffer);

  CConstantBuffer* pConstantBuffer = new CConstantBuffer(_BufferSize);

  (*_ppConstantBuffer) = std::move(pConstantBuffer);
}

void CDevice::CreateConstantBuffer(IBuffer** _ppConstantBuffer, const void* _Data, vdl::uint _BufferSize)
{
  assert(*_ppConstantBuffer);

  CConstantBuffer* pConstantBuffer = new CConstantBuffer;
  pConstantBuffer->BufferSize = _BufferSize;

  HRESULT hr = S_OK;

  D3D11_BUFFER_DESC BufferDesc;
  {
    BufferDesc.ByteWidth = _BufferSize;
    BufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    BufferDesc.CPUAccessFlags = 0;
    BufferDesc.MiscFlags = 0;
    BufferDesc.StructureByteStride = 0;
  }

  D3D11_SUBRESOURCE_DATA InitialData;
  {
    InitialData.pSysMem = _Data;
    InitialData.SysMemPitch = 0;
    InitialData.SysMemSlicePitch = 0;
  }

  hr = pDevice_->CreateBuffer(&BufferDesc, &InitialData, pConstantBuffer->pBuffer.GetAddressOf());
  _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));

  (*_ppConstantBuffer) = std::move(pConstantBuffer);
}

void CDevice::CreateTexture(ITexture** _ppTexture, const vdl::Image& _Image)
{
  assert(*_ppTexture);

  constexpr DXGI_FORMAT kTextureFormat = Cast(Constants::kTextureFormat);

  CTexture* pTexture = new CTexture;
  pTexture->TextureSize = _Image.GetSize();

  HRESULT hr = S_OK;

  D3D11_TEXTURE2D_DESC Texture2DDesc;
  {
    Texture2DDesc.Width = pTexture->TextureSize.x;
    Texture2DDesc.Height = pTexture->TextureSize.y;
    Texture2DDesc.MipLevels = 1;
    Texture2DDesc.ArraySize = 1;
    Texture2DDesc.Format = kTextureFormat;
    Texture2DDesc.SampleDesc.Count = 1;
    Texture2DDesc.SampleDesc.Quality = 0;
    Texture2DDesc.Usage = D3D11_USAGE_IMMUTABLE;
    Texture2DDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    Texture2DDesc.CPUAccessFlags = 0;
    Texture2DDesc.MiscFlags = 0;
  }

  D3D11_SUBRESOURCE_DATA InitialData;
  {
    InitialData.pSysMem = _Image.Buffer();
    InitialData.SysMemPitch = _Image.Stride();
    InitialData.SysMemSlicePitch = _Image.BufferSize();
  }

  Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture2D;
  {
    hr = pDevice_->CreateTexture2D(&Texture2DDesc, &InitialData, pTexture2D.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
  }

  D3D11_SHADER_RESOURCE_VIEW_DESC ShaderResourceViewDesc;
  {
    ShaderResourceViewDesc.Format = kTextureFormat;
    ShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    ShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
    ShaderResourceViewDesc.Texture2D.MipLevels = 1;
  }

  hr = pDevice_->CreateShaderResourceView(pTexture2D.Get(), &ShaderResourceViewDesc, pTexture->pShaderResourceView.GetAddressOf());
  _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));

  (*_ppTexture) = std::move(pTexture);
}

void CDevice::CreateRenderTexture(ITexture** _ppRenderTexture, const vdl::uint2& _TextureSize, vdl::Format _Format)
{
  assert(*_ppRenderTexture);

  CRenderTexture* pRenderTexture = new CRenderTexture;
  pRenderTexture->TextureSize = _TextureSize;

  constexpr DXGI_FORMAT kTextureFormat = Cast(Constants::kTextureFormat);

  HRESULT hr = S_OK;

  D3D11_TEXTURE2D_DESC RenderTargetDesc;
  {
    RenderTargetDesc.Width = pRenderTexture->TextureSize.x;
    RenderTargetDesc.Height = pRenderTexture->TextureSize.y;
    RenderTargetDesc.MipLevels = 1;
    RenderTargetDesc.ArraySize = 1;
    RenderTargetDesc.Format = Cast(_Format);
    RenderTargetDesc.SampleDesc.Count = 1;
    RenderTargetDesc.SampleDesc.Quality = 0;
    RenderTargetDesc.Usage = D3D11_USAGE_IMMUTABLE;
    RenderTargetDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    RenderTargetDesc.CPUAccessFlags = 0;
    RenderTargetDesc.MiscFlags = 0;
  }

  Microsoft::WRL::ComPtr<ID3D11Texture2D> pRenderTarget;
  {
    hr = pDevice_->CreateTexture2D(&RenderTargetDesc, nullptr, pRenderTarget.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
  }

  hr = pDevice_->CreateShaderResourceView(pRenderTarget.Get(), nullptr, pRenderTexture->pShaderResourceView.GetAddressOf());
  _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));

  hr = pDevice_->CreateRenderTargetView(pRenderTarget.Get(), nullptr, pRenderTexture->pRenderTargetView.GetAddressOf());
  _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));

  (*_ppRenderTexture) = std::move(pRenderTexture);
}

void CDevice::CreateDepthStecilTexture(ITexture** _ppDepthStecilTexture, const vdl::uint2& _TextureSize, vdl::Format _Format)
{
  assert(*_ppDepthStecilTexture);

  CDepthStencilTexture* pDepthStencilTexture = new CDepthStencilTexture;
  pDepthStencilTexture->TextureSize = _TextureSize;

  constexpr DXGI_FORMAT kTextureFormat = Cast(Constants::kTextureFormat);

  HRESULT hr = S_OK;

  D3D11_TEXTURE2D_DESC DepthStencilBufferDesc;
  {
    DepthStencilBufferDesc.Width = pDepthStencilTexture->TextureSize.x;
    DepthStencilBufferDesc.Height = pDepthStencilTexture->TextureSize.y;
    DepthStencilBufferDesc.MipLevels = 1;
    DepthStencilBufferDesc.ArraySize = 1;
    DepthStencilBufferDesc.Format = Cast(_Format);
    DepthStencilBufferDesc.SampleDesc.Count = 1;
    DepthStencilBufferDesc.SampleDesc.Quality = 0;
    DepthStencilBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    DepthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    DepthStencilBufferDesc.CPUAccessFlags = 0;
    DepthStencilBufferDesc.MiscFlags = 0;
  }

  Microsoft::WRL::ComPtr<ID3D11Texture2D> pDepthStencilBuffer;
  {
    hr = pDevice_->CreateTexture2D(&DepthStencilBufferDesc, nullptr, pDepthStencilBuffer.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
  }

  hr = pDevice_->CreateShaderResourceView(pDepthStencilBuffer.Get(), nullptr, pDepthStencilTexture->pShaderResourceView.GetAddressOf());
  _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));

  hr = pDevice_->CreateDepthStencilView(pDepthStencilBuffer.Get(), nullptr, pDepthStencilTexture->pDepthStencilView.GetAddressOf());
  _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));

  (*_ppDepthStecilTexture) = std::move(pDepthStencilTexture);
}

void CDevice::LoadShader(IShader** _ppShader, const char* _Source, vdl::uint _DataSize, const char* _EntryPoint, ShaderType _Type)
{
  assert(*_ppShader);

  HRESULT hr = S_OK;

  Microsoft::WRL::ComPtr<ID3D10Blob> pCode;
  switch (_Type)
  {
    //case ShaderType::eVertexShader:
    //  break;
  case ShaderType::eHullShader:
  {
    CHullShader* pHullShader = new CHullShader;

    constexpr const char* kTarget = "hs_5_0";

    ::ComplieShader(pCode.GetAddressOf(), kTarget, _Source, _DataSize, _EntryPoint);

    Microsoft::WRL::ComPtr<ID3D11HullShader> pHullShader;
    {
      hr = pDevice_->CreateHullShader(pCode->GetBufferPointer(), pCode->GetBufferSize(), nullptr, pHullShader->pHullShader.GetAddressOf());
      _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
    }

    *_ppShader = std::move(pHullShader);
  }
  break;
  case ShaderType::eDomainShader:
  {
    CDomainShader* pDomainShader = new CDomainShader;

    constexpr const char* kTarget = "ds_5_0";

    ::ComplieShader(pCode.GetAddressOf(), kTarget, _Source, _DataSize, _EntryPoint);

    Microsoft::WRL::ComPtr<ID3D11DomainShader> pDomainShader;
    {
      hr = pDevice_->CreateDomainShader(pCode->GetBufferPointer(), pCode->GetBufferSize(), nullptr, pDomainShader->pDomainShader.GetAddressOf());
      _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
    }

    *_ppShader = std::move(pDomainShader);
  }
  break;
  case ShaderType::eGeometryShader:
  {
    CGeometryShader* pGeometryShader = new CGeometryShader;

    constexpr const char* kTarget = "gs_5_0";

    ::ComplieShader(pCode.GetAddressOf(), kTarget, _Source, _DataSize, _EntryPoint);

    Microsoft::WRL::ComPtr<ID3D11GeometryShader> pGeometryShader;
    {
      hr = pDevice_->CreateGeometryShader(pCode->GetBufferPointer(), pCode->GetBufferSize(), nullptr, pGeometryShader->pGeometryShader.GetAddressOf());
      _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
    }

    *_ppShader = std::move(pGeometryShader);
  }
  break;
  case ShaderType::ePixelShader:
  {
    CPixelShader* pPixelShader = new CPixelShader;

    constexpr const char* kTarget = "ps_5_0";

    ::ComplieShader(pCode.GetAddressOf(), kTarget, _Source, _DataSize, _EntryPoint);

    Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
    {
      hr = pDevice_->CreatePixelShader(pCode->GetBufferPointer(), pCode->GetBufferSize(), nullptr, pPixelShader->pPixelShader.GetAddressOf());
      _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
    }

    *_ppShader = std::move(pPixelShader);
  }
  break;
  case ShaderType::eComputeShader:
  {
    CComputeShader* pComputeShader = new CComputeShader;

    constexpr const char* kTarget = "cs_5_0";

    ::ComplieShader(pCode.GetAddressOf(), kTarget, _Source, _DataSize, _EntryPoint);

    Microsoft::WRL::ComPtr<ID3D11ComputeShader> pComputeShader;
    {
      hr = pDevice_->CreateComputeShader(pCode->GetBufferPointer(), pCode->GetBufferSize(), nullptr, pComputeShader->pComputeShader.GetAddressOf());
      _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
    }

    *_ppShader = std::move(pComputeShader);
  }
  break;
  default: assert(false);
  }
}

void CDevice::LoadShader(IVertexShader** _ppVertexShader, const char* _Source, vdl::uint _DataSize, const char* _EntryPoint, vdl::InputLayout _InputLayout)
{
  assert(*_ppVertexShader);

  CVertexShader* pVertexShader = new CVertexShader;
  pVertexShader->InputLayout = _InputLayout;

  constexpr const char* kTarget = "vs_5_0";

  HRESULT hr = S_OK;

  Microsoft::WRL::ComPtr<ID3D10Blob> pCode;
  ::ComplieShader(pCode.GetAddressOf(), kTarget, _Source, _DataSize, _EntryPoint);

  Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
  {
    hr = pDevice_->CreateVertexShader(pCode->GetBufferPointer(), pCode->GetBufferSize(), nullptr, pVertexShader->pVertexShader.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
  }

  *_ppVertexShader = std::move(pVertexShader);
}

void CDevice::WriteMemory(IBuffer* _pDstBuffer, const void* _pSrcBuffer, vdl::uint _BufferSize)const
{
  HRESULT hr = S_OK;

  switch (_pDstBuffer->GetType())
  {
  case BufferType::eVertexBuffer:
  {
    ID3D11Buffer* pBuffer = static_cast<CVertexBuffer*>(_pDstBuffer)->pBuffer.Get();

    D3D11_MAPPED_SUBRESOURCE MappedSubresorce;
    hr = pImmediateContext_->Map(pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresorce);
    _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));

    ::memcpy(MappedSubresorce.pData, _pSrcBuffer, _BufferSize);

    pImmediateContext_->Unmap(pBuffer, 0);
  }
  break;
  case BufferType::eInstanceBuffer:
  {
    ID3D11Buffer* pBuffer = static_cast<CInstanceBuffer*>(_pDstBuffer)->pBuffer.Get();

    D3D11_MAPPED_SUBRESOURCE MappedSubresorce;
    hr = pImmediateContext_->Map(pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresorce);
    _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));

    ::memcpy(MappedSubresorce.pData, _pSrcBuffer, _BufferSize);

    pImmediateContext_->Unmap(pBuffer, 0);
  }
  break;
  case BufferType::eIndexBuffer:
  {
    ID3D11Buffer* pBuffer = static_cast<CIndexBuffer*>(_pDstBuffer)->pBuffer.Get();

    D3D11_MAPPED_SUBRESOURCE MappedSubresorce;
    hr = pImmediateContext_->Map(pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresorce);
    _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));

    ::memcpy(MappedSubresorce.pData, _pSrcBuffer, _BufferSize);

    pImmediateContext_->Unmap(pBuffer, 0);
  }
  break;
  case BufferType::eConstantBuffer:
  {
    ID3D11Buffer* pBuffer = static_cast<CConstantBuffer*>(_pDstBuffer)->pBuffer.Get();

    D3D11_MAPPED_SUBRESOURCE MappedSubresorce;
    hr = pImmediateContext_->Map(pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresorce);
    _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));

    ::memcpy(MappedSubresorce.pData, _pSrcBuffer, _BufferSize);

    pImmediateContext_->Unmap(pBuffer, 0);
  }
  break;
  case BufferType::eUnorderedAccessBuffer:
  {
    ID3D11Buffer* pBuffer = static_cast<CUnordererdAccessBuffer*>(_pDstBuffer)->pBuffer.Get();

    D3D11_MAPPED_SUBRESOURCE MappedSubresorce;
    hr = pImmediateContext_->Map(pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresorce);
    _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));

    ::memcpy(MappedSubresorce.pData, _pSrcBuffer, _BufferSize);

    pImmediateContext_->Unmap(pBuffer, 0);
  }
  break;
  default: assert(false);
  }
}
