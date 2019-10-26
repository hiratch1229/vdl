#include "CDevice.hpp"

#include <vdl/Engine.hpp>
#include <vdl/DeviceContext/DirectX11/CDeviceContext.hpp>

#include <vdl/Format/DirectX/Format.hpp>
#include <vdl/Buffer/DirectX11/CBuffer.hpp>
#include <vdl/Texture/DirectX11/CTexture.hpp>
#include <vdl/Shader/DirectX11/CShader.hpp>
#include <vdl/Misc/Windows/Misc.hpp>

#include <vdl/Constants/Constants.hpp>

#include <vdl/Image.hpp>
#include <vdl/Macro.hpp>

#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <assert.h>

namespace
{
  constexpr const char* kShaderTargets[static_cast<vdl::uint>(ShaderType::eNum)] = { "vs_5_0", "hs_5_0", "ds_5_0", "gs_5_0", "ps_5_0", "cs_5_0" };

  inline void ComplieShader(ID3DBlob** _ppCode, const char* _Target, const char* _FilePath, const char* _EntryPoint)
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
      _EntryPoint, _Target, CompileFlag, 0, _ppCode, pError.GetAddressOf());
    _ASSERT_EXPR_A(SUCCEEDED(hr), static_cast<const char*>(pError->GetBufferPointer()));
  }

  inline void ComplieShader(ID3DBlob** _ppCode, const char* _Target, const char* _Source, vdl::uint _DataSize, const char* _EntryPoint)
  {
    vdl::uint CompileFlag = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined _DEBUG | DEBUG
    CompileFlag |= D3DCOMPILE_DEBUG;
#endif

    HRESULT hr = S_OK;

    Microsoft::WRL::ComPtr<ID3DBlob> pError;
    hr = ::D3DCompile(_Source, _DataSize, nullptr, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
      _EntryPoint, _Target, CompileFlag, 0, _ppCode, pError.GetAddressOf());
    _ASSERT_EXPR_A(SUCCEEDED(hr), static_cast<const char*>(pError->GetBufferPointer()));
  }

  inline DXGI_FORMAT Cast(IndexType _Type)
  {
    switch (_Type)
    {
    case IndexType::eUint16:
      return DXGI_FORMAT_R16_UINT;
    case IndexType::eUint32:
      return DXGI_FORMAT_R32_UINT;
    default: assert(false);
    }

    return DXGI_FORMAT_R16_UINT;
  }
}

void CDevice::Initialize()
{
  pDeviceContext_ = static_cast<CDeviceContext*>(Engine::Get<IDeviceContext>());

  //  エラーチェック用
  HRESULT hr = S_OK;

#if defined( DEBUG ) || defined( _DEBUG )
  constexpr vdl::uint kCreateDeviceFlag = D3D11_CREATE_DEVICE_DEBUG;
  constexpr vdl::uint kDxgiCreateFactoryFlag = DXGI_CREATE_FACTORY_DEBUG;
#else
  constexpr vdl::uint kCreateDeviceFlag = 0;
  constexpr vdl::uint kDxgiCreateFactoryFlag = 0;
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
  constexpr vdl::uint kFeatureLevelNum = static_cast<vdl::uint>(vdl::Macro::ArraySize(kFeatureLevels));

  Microsoft::WRL::ComPtr<IDXGIFactory7> pFactory;
  {
    hr = ::CreateDXGIFactory2(kDxgiCreateFactoryFlag, IID_PPV_ARGS(pFactory.GetAddressOf()));
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
    hr = ::D3D11CreateDevice(pAdapter.Get(), kDriverTypes[i], nullptr, kCreateDeviceFlag, kFeatureLevels,
      kFeatureLevelNum, D3D11_SDK_VERSION, pD3D11Device_.GetAddressOf(), &FeatureLevel, pD3D11ImmediateContext_.GetAddressOf());

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
  assert(_ppVertexBuffer);

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

  hr = pD3D11Device_->CreateBuffer(&BufferDesc, nullptr, pVertexBuffer->pBuffer.GetAddressOf());
  _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));

  (*_ppVertexBuffer) = std::move(pVertexBuffer);
}

void CDevice::CreateVertexBuffer(IBuffer** _ppVertexBuffer, const void* _Vertices, vdl::uint _Stride, vdl::uint _BufferSize)
{
  assert(_ppVertexBuffer);

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

  hr = pD3D11Device_->CreateBuffer(&BufferDesc, &InitialData, pVertexBuffer->pBuffer.GetAddressOf());
  _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));

  (*_ppVertexBuffer) = std::move(pVertexBuffer);
}

void CDevice::CreateInstanceBuffer(IBuffer** _ppInstanceBuffer, vdl::uint _Stride, vdl::uint _BufferSize)
{
  assert(_ppInstanceBuffer);

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

  hr = pD3D11Device_->CreateBuffer(&BufferDesc, nullptr, pInstanceBuffer->pBuffer.GetAddressOf());
  _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));

  (*_ppInstanceBuffer) = std::move(pInstanceBuffer);
}

void CDevice::CreateIndexBuffer(IBuffer** _ppIndexBuffer, vdl::uint _BufferSize, IndexType _IndexType)
{
  assert(_ppIndexBuffer);

  CIndexBuffer* pIndexBuffer = new CIndexBuffer;
  pIndexBuffer->IndexFormat = Cast(_IndexType);

  HRESULT hr = S_OK;

  D3D11_BUFFER_DESC BufferDesc;
  {
    BufferDesc.ByteWidth = _BufferSize;
    BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    BufferDesc.MiscFlags = 0;
    BufferDesc.StructureByteStride = 0;
  }

  hr = pD3D11Device_->CreateBuffer(&BufferDesc, nullptr, pIndexBuffer->pBuffer.GetAddressOf());
  _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));

  (*_ppIndexBuffer) = std::move(pIndexBuffer);
}

void CDevice::CreateIndexBuffer(IBuffer** _ppIndexBuffer, const void* _Indices, vdl::uint _BufferSize, IndexType _IndexType)
{
  assert(_ppIndexBuffer);

  CIndexBuffer* pIndexBuffer = new CIndexBuffer;
  pIndexBuffer->IndexFormat = Cast(_IndexType);

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

  hr = pD3D11Device_->CreateBuffer(&BufferDesc, &InitialData, pIndexBuffer->pBuffer.GetAddressOf());
  _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));

  (*_ppIndexBuffer) = std::move(pIndexBuffer);
}

void CDevice::CreateConstantBuffer(IBuffer** _ppConstantBuffer, vdl::uint _BufferSize)
{
  assert(_ppConstantBuffer);

  CConstantBuffer* pConstantBuffer = new CConstantBuffer(_BufferSize);

  (*_ppConstantBuffer) = std::move(pConstantBuffer);
}

void CDevice::CreateUnorderedAccessBuffer(IBuffer** _ppUnorderedAccessBuffer, vdl::uint _Stride, vdl::uint _BufferSize, const void* _Buffer)
{
  assert(_ppUnorderedAccessBuffer);

  CUnordererdAccessBuffer* pUnordererdAccessBuffer = new CUnordererdAccessBuffer(_Stride, _BufferSize);

  HRESULT hr = S_OK;

  D3D11_BUFFER_DESC BufferDesc;
  {
    BufferDesc.ByteWidth = _BufferSize;
    BufferDesc.Usage = D3D11_USAGE_DEFAULT;
    BufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
    BufferDesc.CPUAccessFlags = 0;
    BufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    BufferDesc.StructureByteStride = _Stride;
  }

  if (_Buffer)
  {
    D3D11_SUBRESOURCE_DATA InitialData;
    {
      InitialData.pSysMem = _Buffer;
      InitialData.SysMemPitch = _Stride;
      InitialData.SysMemSlicePitch = 0;
    }

    hr = pD3D11Device_->CreateBuffer(&BufferDesc, &InitialData, pUnordererdAccessBuffer->pBuffer.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
  }
  else
  {
    hr = pD3D11Device_->CreateBuffer(&BufferDesc, nullptr, pUnordererdAccessBuffer->pBuffer.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
  }

  hr = pD3D11Device_->CreateShaderResourceView(pUnordererdAccessBuffer->pBuffer.Get(), nullptr, pUnordererdAccessBuffer->pShaderResourceView.GetAddressOf());
  _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));

  hr = pD3D11Device_->CreateUnorderedAccessView(pUnordererdAccessBuffer->pBuffer.Get(), nullptr, pUnordererdAccessBuffer->pUnorderedAccessView.GetAddressOf());
  _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));

  (*_ppUnorderedAccessBuffer) = std::move(pUnordererdAccessBuffer);
}

void CDevice::CreateTexture(ITexture** _ppTexture, const vdl::Image& _Image)
{
  assert(_ppTexture);

  constexpr DXGI_FORMAT kTextureFormat = Cast(Constants::kTextureFormat);

  CTexture* pTexture = new CTexture;
  pTexture->TextureSize = _Image.GetTextureSize();

  HRESULT hr = S_OK;

  D3D11_TEXTURE2D_DESC TextureDesc;
  {
    TextureDesc.Width = pTexture->TextureSize.x;
    TextureDesc.Height = pTexture->TextureSize.y;
    TextureDesc.MipLevels = 1;
    TextureDesc.ArraySize = 1;
    TextureDesc.Format = kTextureFormat;
    TextureDesc.SampleDesc.Count = 1;
    TextureDesc.SampleDesc.Quality = 0;
    TextureDesc.Usage = D3D11_USAGE_IMMUTABLE;
    TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    TextureDesc.CPUAccessFlags = 0;
    TextureDesc.MiscFlags = 0;
  }

  D3D11_SUBRESOURCE_DATA InitialData;
  {
    InitialData.pSysMem = _Image.Buffer();
    InitialData.SysMemPitch = _Image.Stride();
    InitialData.SysMemSlicePitch = _Image.BufferSize();
  }

  Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture2D;
  {
    hr = pD3D11Device_->CreateTexture2D(&TextureDesc, &InitialData, pTexture2D.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
  }

  D3D11_SHADER_RESOURCE_VIEW_DESC ShaderResourceViewDesc;
  {
    ShaderResourceViewDesc.Format = kTextureFormat;
    ShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    ShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
    ShaderResourceViewDesc.Texture2D.MipLevels = 1;
  }

  hr = pD3D11Device_->CreateShaderResourceView(pTexture2D.Get(), &ShaderResourceViewDesc, pTexture->pShaderResourceView.GetAddressOf());
  _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));

  (*_ppTexture) = std::move(pTexture);
}

void CDevice::CreateRenderTexture(ITexture** _ppRenderTexture, const vdl::uint2& _TextureSize, vdl::FormatType _Format)
{
  assert(_ppRenderTexture);

  CRenderTexture* pRenderTexture = new CRenderTexture;
  pRenderTexture->TextureSize = _TextureSize;

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
    RenderTargetDesc.Usage = D3D11_USAGE_DEFAULT;
    RenderTargetDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    RenderTargetDesc.CPUAccessFlags = 0;
    RenderTargetDesc.MiscFlags = 0;
  }

  Microsoft::WRL::ComPtr<ID3D11Texture2D> pRenderTarget;
  {
    hr = pD3D11Device_->CreateTexture2D(&RenderTargetDesc, nullptr, pRenderTarget.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
  }

  hr = pD3D11Device_->CreateShaderResourceView(pRenderTarget.Get(), nullptr, pRenderTexture->pShaderResourceView.GetAddressOf());
  _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));

  hr = pD3D11Device_->CreateRenderTargetView(pRenderTarget.Get(), nullptr, pRenderTexture->pRenderTargetView.GetAddressOf());
  _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));

  (*_ppRenderTexture) = std::move(pRenderTexture);
}

void CDevice::CreateDepthStecilTexture(ITexture** _ppDepthStecilTexture, const vdl::uint2& _TextureSize, vdl::FormatType _Format)
{
  assert(_ppDepthStecilTexture);

  CDepthStencilTexture* pDepthStencilTexture = new CDepthStencilTexture;
  pDepthStencilTexture->TextureSize = _TextureSize;

  HRESULT hr = S_OK;

  D3D11_TEXTURE2D_DESC DepthStencilBufferDesc;
  {
    DepthStencilBufferDesc.Width = pDepthStencilTexture->TextureSize.x;
    DepthStencilBufferDesc.Height = pDepthStencilTexture->TextureSize.y;
    DepthStencilBufferDesc.MipLevels = 1;
    DepthStencilBufferDesc.ArraySize = 1;
    DepthStencilBufferDesc.Format = TextureFormatFromDepthStencilFormat(_Format);
    DepthStencilBufferDesc.SampleDesc.Count = 1;
    DepthStencilBufferDesc.SampleDesc.Quality = 0;
    DepthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    DepthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    DepthStencilBufferDesc.CPUAccessFlags = 0;
    DepthStencilBufferDesc.MiscFlags = 0;
  }

  Microsoft::WRL::ComPtr<ID3D11Texture2D> pDepthStencilBuffer;
  {
    hr = pD3D11Device_->CreateTexture2D(&DepthStencilBufferDesc, nullptr, pDepthStencilBuffer.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
  }

  D3D11_SHADER_RESOURCE_VIEW_DESC ShaderResourceViewDesc;
  {
    ShaderResourceViewDesc.Format = ShaderResourceFormatFromDepthStencilFormat(_Format);
    ShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    ShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
    ShaderResourceViewDesc.Texture2D.MipLevels = 1;
  }

  hr = pD3D11Device_->CreateShaderResourceView(pDepthStencilBuffer.Get(), &ShaderResourceViewDesc, pDepthStencilTexture->pShaderResourceView.GetAddressOf());
  _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));

  D3D11_DEPTH_STENCIL_VIEW_DESC DepthStencilViewDesc;
  {
    DepthStencilViewDesc.Format = Cast(_Format);
    DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    DepthStencilViewDesc.Flags = 0;
    DepthStencilViewDesc.Texture2D.MipSlice = 0;
  }

  hr = pD3D11Device_->CreateDepthStencilView(pDepthStencilBuffer.Get(), &DepthStencilViewDesc, pDepthStencilTexture->pDepthStencilView.GetAddressOf());
  _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));

  (*_ppDepthStecilTexture) = std::move(pDepthStencilTexture);
}

void CDevice::CreateUnorderedAccessTexture(ITexture** _ppUnorderedAccessTexture, const vdl::uint2& _TextureSize, vdl::FormatType _Format)
{
  assert(_ppUnorderedAccessTexture);

  CUnorderedAccessTexture* pUnorderedAccessTexture = new CUnorderedAccessTexture;
  pUnorderedAccessTexture->TextureSize = _TextureSize;

  HRESULT hr = S_OK;

  D3D11_TEXTURE2D_DESC UnorderedAccessTextureDesc;
  {
    UnorderedAccessTextureDesc.Width = pUnorderedAccessTexture->TextureSize.x;
    UnorderedAccessTextureDesc.Height = pUnorderedAccessTexture->TextureSize.y;
    UnorderedAccessTextureDesc.MipLevels = 1;
    UnorderedAccessTextureDesc.ArraySize = 1;
    UnorderedAccessTextureDesc.Format = Cast(_Format);
    UnorderedAccessTextureDesc.SampleDesc.Count = 1;
    UnorderedAccessTextureDesc.SampleDesc.Quality = 0;
    UnorderedAccessTextureDesc.Usage = D3D11_USAGE_DEFAULT;
    UnorderedAccessTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
    UnorderedAccessTextureDesc.CPUAccessFlags = 0;
    UnorderedAccessTextureDesc.MiscFlags = 0;
  }

  Microsoft::WRL::ComPtr<ID3D11Texture2D> pUnorderedAccessBuffer;
  {
    hr = pD3D11Device_->CreateTexture2D(&UnorderedAccessTextureDesc, nullptr, pUnorderedAccessBuffer.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
  }

  hr = pD3D11Device_->CreateShaderResourceView(pUnorderedAccessBuffer.Get(), nullptr, pUnorderedAccessTexture->pShaderResourceView.GetAddressOf());
  _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));

  hr = pD3D11Device_->CreateUnorderedAccessView(pUnorderedAccessBuffer.Get(), nullptr, pUnorderedAccessTexture->pUnorderedAccessView.GetAddressOf());
  _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));

  (*_ppUnorderedAccessTexture) = std::move(pUnorderedAccessTexture);
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
    hr = pD3D11ImmediateContext_->Map(pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresorce);
    _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));

    ::memcpy(MappedSubresorce.pData, _pSrcBuffer, _BufferSize);

    pD3D11ImmediateContext_->Unmap(pBuffer, 0);
  }
  break;
  case BufferType::eInstanceBuffer:
  {
    ID3D11Buffer* pBuffer = static_cast<CInstanceBuffer*>(_pDstBuffer)->pBuffer.Get();

    D3D11_MAPPED_SUBRESOURCE MappedSubresorce;
    hr = pD3D11ImmediateContext_->Map(pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresorce);
    _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));

    ::memcpy(MappedSubresorce.pData, _pSrcBuffer, _BufferSize);

    pD3D11ImmediateContext_->Unmap(pBuffer, 0);
  }
  break;
  case BufferType::eIndexBuffer:
  {
    ID3D11Buffer* pBuffer = static_cast<CIndexBuffer*>(_pDstBuffer)->pBuffer.Get();

    D3D11_MAPPED_SUBRESOURCE MappedSubresorce;
    hr = pD3D11ImmediateContext_->Map(pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresorce);
    _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));

    ::memcpy(MappedSubresorce.pData, _pSrcBuffer, _BufferSize);

    pD3D11ImmediateContext_->Unmap(pBuffer, 0);
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

  Microsoft::WRL::ComPtr<ID3DBlob> pCode;
  switch (_Type)
  {
    //case ShaderType::eVertexShader:
    //  break;
  case ShaderType::eHullShader:
  {
    CHullShader* pShader = new CHullShader;

    ::ComplieShader(pCode.GetAddressOf(), ShaderTarget, _FilePath, _EntryPoint);

    Microsoft::WRL::ComPtr<ID3D11HullShader> pHullShader;
    {
      hr = pD3D11Device_->CreateHullShader(pCode->GetBufferPointer(), pCode->GetBufferSize(), nullptr, pShader->pHullShader.GetAddressOf());
      _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
    }

    (*_ppShader) = std::move(pShader);
  }
  break;
  case ShaderType::eDomainShader:
  {
    CDomainShader* pShader = new CDomainShader;

    ::ComplieShader(pCode.GetAddressOf(), ShaderTarget, _FilePath, _EntryPoint);

    Microsoft::WRL::ComPtr<ID3D11DomainShader> pDomainShader;
    {
      hr = pD3D11Device_->CreateDomainShader(pCode->GetBufferPointer(), pCode->GetBufferSize(), nullptr, pShader->pDomainShader.GetAddressOf());
      _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
    }

    (*_ppShader) = std::move(pShader);
  }
  break;
  case ShaderType::eGeometryShader:
  {
    CGeometryShader* pShader = new CGeometryShader;

    ::ComplieShader(pCode.GetAddressOf(), ShaderTarget, _FilePath, _EntryPoint);

    Microsoft::WRL::ComPtr<ID3D11GeometryShader> pGeometryShader;
    {
      hr = pD3D11Device_->CreateGeometryShader(pCode->GetBufferPointer(), pCode->GetBufferSize(), nullptr, pShader->pGeometryShader.GetAddressOf());
      _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
    }

    (*_ppShader) = std::move(pShader);
  }
  break;
  case ShaderType::ePixelShader:
  {
    CPixelShader* pShader = new CPixelShader;

    ::ComplieShader(pCode.GetAddressOf(), ShaderTarget, _FilePath, _EntryPoint);

    Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
    {
      hr = pD3D11Device_->CreatePixelShader(pCode->GetBufferPointer(), pCode->GetBufferSize(), nullptr, pShader->pPixelShader.GetAddressOf());
      _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
    }

    (*_ppShader) = std::move(pShader);
  }
  break;
  case ShaderType::eComputeShader:
  {
    CComputeShader* pShader = new CComputeShader;

    ::ComplieShader(pCode.GetAddressOf(), ShaderTarget, _FilePath, _EntryPoint);

    Microsoft::WRL::ComPtr<ID3D11ComputeShader> pComputeShader;
    {
      hr = pD3D11Device_->CreateComputeShader(pCode->GetBufferPointer(), pCode->GetBufferSize(), nullptr, pShader->pComputeShader.GetAddressOf());
      _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
    }

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

  Microsoft::WRL::ComPtr<ID3DBlob> pCode;
  switch (_Type)
  {
    //case ShaderType::eVertexShader:
    //  break;
  case ShaderType::eHullShader:
  {
    CHullShader* pShader = new CHullShader;

    ::ComplieShader(pCode.GetAddressOf(), ShaderTarget, _Source, _DataSize, _EntryPoint);

    Microsoft::WRL::ComPtr<ID3D11HullShader> pHullShader;
    {
      hr = pD3D11Device_->CreateHullShader(pCode->GetBufferPointer(), pCode->GetBufferSize(), nullptr, pShader->pHullShader.GetAddressOf());
      _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
    }

    (*_ppShader) = std::move(pShader);
  }
  break;
  case ShaderType::eDomainShader:
  {
    CDomainShader* pShader = new CDomainShader;

    ::ComplieShader(pCode.GetAddressOf(), ShaderTarget, _Source, _DataSize, _EntryPoint);

    Microsoft::WRL::ComPtr<ID3D11DomainShader> pDomainShader;
    {
      hr = pD3D11Device_->CreateDomainShader(pCode->GetBufferPointer(), pCode->GetBufferSize(), nullptr, pShader->pDomainShader.GetAddressOf());
      _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
    }

    (*_ppShader) = std::move(pShader);
  }
  break;
  case ShaderType::eGeometryShader:
  {
    CGeometryShader* pShader = new CGeometryShader;

    ::ComplieShader(pCode.GetAddressOf(), ShaderTarget, _Source, _DataSize, _EntryPoint);

    Microsoft::WRL::ComPtr<ID3D11GeometryShader> pGeometryShader;
    {
      hr = pD3D11Device_->CreateGeometryShader(pCode->GetBufferPointer(), pCode->GetBufferSize(), nullptr, pShader->pGeometryShader.GetAddressOf());
      _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
    }

    (*_ppShader) = std::move(pShader);
  }
  break;
  case ShaderType::ePixelShader:
  {
    CPixelShader* pShader = new CPixelShader;

    ::ComplieShader(pCode.GetAddressOf(), ShaderTarget, _Source, _DataSize, _EntryPoint);

    Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
    {
      hr = pD3D11Device_->CreatePixelShader(pCode->GetBufferPointer(), pCode->GetBufferSize(), nullptr, pShader->pPixelShader.GetAddressOf());
      _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
    }

    *_ppShader = std::move(pShader);
  }
  break;
  case ShaderType::eComputeShader:
  {
    CComputeShader* pShader = new CComputeShader;

    ::ComplieShader(pCode.GetAddressOf(), ShaderTarget, _Source, _DataSize, _EntryPoint);

    Microsoft::WRL::ComPtr<ID3D11ComputeShader> pComputeShader;
    {
      hr = pD3D11Device_->CreateComputeShader(pCode->GetBufferPointer(), pCode->GetBufferSize(), nullptr, pShader->pComputeShader.GetAddressOf());
      _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
    }

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

  Microsoft::WRL::ComPtr<ID3DBlob> pCode;
  ::ComplieShader(pCode.GetAddressOf(), kShaderTarget, _FilePath, _EntryPoint);

  Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
  {
    hr = pD3D11Device_->CreateVertexShader(pCode->GetBufferPointer(), pCode->GetBufferSize(), nullptr, pShader->pVertexShader.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
  }

  (*_ppVertexShader) = std::move(pShader);

  //  インプットレイアウトの登録
  if (!pDeviceContext_->isFoundInputLayout(_InputLayout))
  {
    pDeviceContext_->RegisterInputLayout(_InputLayout, pCode.Get());
  }
}

void CDevice::LoadShader(IVertexShader** _ppVertexShader, const char* _Source, vdl::uint _DataSize, const char* _EntryPoint, vdl::InputLayoutType _InputLayout)
{
  assert(_ppVertexShader);

  CVertexShader* pShader = new CVertexShader;
  pShader->InputLayout = _InputLayout;

  constexpr const char* kShaderTarget = kShaderTargets[static_cast<vdl::uint>(ShaderType::eVertexShader)];

  HRESULT hr = S_OK;

  Microsoft::WRL::ComPtr<ID3DBlob> pCode;
  ::ComplieShader(pCode.GetAddressOf(), kShaderTarget, _Source, _DataSize, _EntryPoint);

  Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
  {
    hr = pD3D11Device_->CreateVertexShader(pCode->GetBufferPointer(), pCode->GetBufferSize(), nullptr, pShader->pVertexShader.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
  }

  (*_ppVertexShader) = std::move(pShader);

  //  インプットレイアウトの登録
  if (!pDeviceContext_->isFoundInputLayout(_InputLayout))
  {
    pDeviceContext_->RegisterInputLayout(_InputLayout, pCode.Get());
  }
}
