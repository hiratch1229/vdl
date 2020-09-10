#include "CDevice.hpp"

#include <vdl/Engine.hpp>
#include <vdl/Window/IWindow.hpp>
#include <vdl/DeviceContext/DirectX11/CDeviceContext.hpp>
#include <vdl/TextureManager/ITextureManager.hpp>
#include <vdl/BufferManager/IBufferManager.hpp>
#include <vdl/Format/DirectX/Format.hpp>
#include <vdl/Buffer/DirectX11/CBuffer.hpp>
#include <vdl/Texture/DirectX11/CTexture.hpp>
#include <vdl/Shader/DirectX11/CShader.hpp>
#include <vdl/Misc/Windows/Misc.hpp>

#include <vdl/Format/Format.hpp>
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
  constexpr vdl::uint kCompileFlag = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_OPTIMIZATION_LEVEL3;

  inline void ComplieShader(ID3DBlob** _ppCode, const char* _Target, const char* _FilePath, const char* _EntryPoint)
  {
    const std::filesystem::path OriginalFilePath = _FilePath;
    const std::filesystem::path BinaryFileDirectory = std::filesystem::path(Constants::kBinaryFileDirectory) / std::filesystem::path(_FilePath).remove_filename();
    const std::filesystem::path BinaryFilePath = std::filesystem::path((BinaryFileDirectory / std::filesystem::path(_FilePath).filename()).string() + "_" + std::string(_EntryPoint)).concat(Constants::kShaderBinaryFileFormat);
    const bool existOriginalFile = std::filesystem::exists(OriginalFilePath);

    HRESULT hr = S_OK;

    //  バイナリファイルが存在して、元ファイルの更新日時が古い場合読み込み
    if (std::filesystem::exists(BinaryFilePath) && !(existOriginalFile && ::isFileUpdate(OriginalFilePath, BinaryFilePath)))
    {
      std::vector<vdl::uint8_t> Code;
      ::ImportFromBinary(BinaryFilePath.string().c_str(), Code);

      const size_t CodeSize = Code.size();
      hr = ::D3DCreateBlob(CodeSize, _ppCode);
      ERROR_CHECK(hr);
      ::memcpy((*_ppCode)->GetBufferPointer(), Code.data(), CodeSize);
    }
    else
    {
      _ASSERT_EXPR_A(existOriginalFile, std::string(std::string(_FilePath) + "が見つかりません。").c_str());

      wchar_t wFilePath[Constants::kMaxCharacterNum]{};
      ::mbstowcs_s(nullptr, wFilePath, _FilePath, Constants::kMaxCharacterNum);

      Microsoft::WRL::ComPtr<ID3DBlob> pError;
      hr = ::D3DCompileFromFile(wFilePath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
        _EntryPoint, _Target, kCompileFlag, 0, _ppCode, pError.GetAddressOf());
      _ASSERT_EXPR_A(SUCCEEDED(hr), static_cast<const char*>(pError->GetBufferPointer()));

      //  フォルダが存在しない場合作成
      if (!std::filesystem::exists(BinaryFileDirectory))
      {
        std::filesystem::create_directories(BinaryFileDirectory);
      }

      //  バイナリファイルに書き出し
      {
        std::vector<vdl::uint8_t> Code;
        const size_t CodeSize = (*_ppCode)->GetBufferSize();
        Code.resize(CodeSize);
        ::memcpy(Code.data(), (*_ppCode)->GetBufferPointer(), CodeSize);
        ::ExportToBinary(BinaryFilePath.string().c_str(), Code);
      }
    }
  }

  inline void ComplieShader(ID3DBlob** _ppCode, const char* _Target, const char* _Source, vdl::uint _DataSize, const char* _EntryPoint)
  {
    HRESULT hr = S_OK;

    Microsoft::WRL::ComPtr<ID3DBlob> pError;
    hr = ::D3DCompile(_Source, _DataSize, nullptr, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
      _EntryPoint, _Target, kCompileFlag, 0, _ppCode, pError.GetAddressOf());
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
  pDeviceContext_ = Cast<CDeviceContext>(Engine::Get<IDeviceContext>());
  pTextureManager_ = Engine::Get<ITextureManager>();
  pBufferManager_ = Engine::Get<IBufferManager>();

  const HWND hWnd = static_cast<HWND>(Engine::Get<IWindow>()->GetHandle());

  //  エラーチェック用
  HRESULT hr = S_OK;

#if defined _DEBUG
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

  //  ファクトリーの作成
  Microsoft::WRL::ComPtr<IDXGIFactory6> pFactory;
  {
    Microsoft::WRL::ComPtr<IDXGIFactory2> pFactory2;

    hr = ::CreateDXGIFactory2(kDxgiCreateFactoryFlag, IID_PPV_ARGS(pFactory2.GetAddressOf()));
    ERROR_CHECK(hr);

    pFactory2.As(&pFactory);
  }

  bool useWrapAdapter = false;

  //  アダプターの検索
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

  //  GPUが無い場合CPUでエミュレートする
  if (useWrapAdapter)
  {
    hr = pFactory->EnumWarpAdapter(IID_PPV_ARGS(pAdapter.GetAddressOf()));
    ERROR_CHECK(hr);
  }

  //  デバイスの作成
  {
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
    ERROR_CHECK(hr);
  }

  //  スワップチェーンの作成
  {
    DXGI_SWAP_CHAIN_DESC SwapChainDesc;
    {
      SwapChainDesc.BufferDesc.Width = Constants::kDefaultWindowSize.x;
      SwapChainDesc.BufferDesc.Height = Constants::kDefaultWindowSize.y;
      SwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
      SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
      SwapChainDesc.BufferDesc.Format = Cast(vdl::FormatType::eSwapChain);
      SwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
      SwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
      SwapChainDesc.SampleDesc.Count = 1;
      SwapChainDesc.SampleDesc.Quality = 0;
      SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
      SwapChainDesc.BufferCount = Constants::kBackBufferNum;
      SwapChainDesc.OutputWindow = hWnd;
      SwapChainDesc.Windowed = true;
      SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
      SwapChainDesc.Flags = 0;
    }

    hr = pFactory->CreateSwapChain(pD3D11Device_.Get(), &SwapChainDesc, pDXGISwapChain_.GetAddressOf());
    ERROR_CHECK(hr);

    //  ALT+Enterの無効化
    hr = pFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);
    ERROR_CHECK(hr);
  }

}

void CDevice::CreateVertexBuffer(IBuffer** _ppVertexBuffer, vdl::uint _BufferSize)
{
  assert(_ppVertexBuffer);

  CVertexBuffer* pVertexBuffer = new CVertexBuffer;

  HRESULT hr = S_OK;

  //  バッファの作成
  {
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
    ERROR_CHECK(hr);
  }

  (*_ppVertexBuffer) = std::move(pVertexBuffer);
}

void CDevice::CreateVertexBuffer(IBuffer** _ppVertexBuffer, const void* _Vertices, vdl::uint _BufferSize)
{
  assert(_ppVertexBuffer);

  CVertexBuffer* pVertexBuffer = new CVertexBuffer;

  HRESULT hr = S_OK;

  //  バッファの作成
  {
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
    ERROR_CHECK(hr);
  }

  (*_ppVertexBuffer) = std::move(pVertexBuffer);
}

void CDevice::CreateInstanceBuffer(IBuffer** _ppInstanceBuffer, vdl::uint _BufferSize)
{
  assert(_ppInstanceBuffer);

  CInstanceBuffer* pInstanceBuffer = new CInstanceBuffer;

  HRESULT hr = S_OK;

  //  バッファの作成
  {
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
    ERROR_CHECK(hr);
  }

  (*_ppInstanceBuffer) = std::move(pInstanceBuffer);
}

void CDevice::CreateIndexBuffer(IBuffer** _ppIndexBuffer, vdl::uint _BufferSize, IndexType _IndexType)
{
  assert(_ppIndexBuffer);

  CIndexBuffer* pIndexBuffer = new CIndexBuffer;
  pIndexBuffer->IndexFormat = Cast(_IndexType);

  HRESULT hr = S_OK;

  //  バッファの作成
  {
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
    ERROR_CHECK(hr);
  }

  (*_ppIndexBuffer) = std::move(pIndexBuffer);
}

void CDevice::CreateIndexBuffer(IBuffer** _ppIndexBuffer, const void* _Indices, vdl::uint _BufferSize, IndexType _IndexType)
{
  assert(_ppIndexBuffer);

  CIndexBuffer* pIndexBuffer = new CIndexBuffer;
  pIndexBuffer->IndexFormat = Cast(_IndexType);

  HRESULT hr = S_OK;

  //  バッファの作成
  {
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
    ERROR_CHECK(hr);
  }

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

  //  バッファの作成
  {
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
      ERROR_CHECK(hr);
    }
    else
    {
      hr = pD3D11Device_->CreateBuffer(&BufferDesc, nullptr, pUnordererdAccessBuffer->pBuffer.GetAddressOf());
      ERROR_CHECK(hr);
    }
  }

  //  ビューの作成
  {
    hr = pD3D11Device_->CreateShaderResourceView(pUnordererdAccessBuffer->pBuffer.Get(), nullptr, pUnordererdAccessBuffer->pShaderResourceView.GetAddressOf());
    ERROR_CHECK(hr);

    hr = pD3D11Device_->CreateUnorderedAccessView(pUnordererdAccessBuffer->pBuffer.Get(), nullptr, pUnordererdAccessBuffer->pUnorderedAccessView.GetAddressOf());
    ERROR_CHECK(hr);
  }

  (*_ppUnorderedAccessBuffer) = std::move(pUnordererdAccessBuffer);
}

vdl::Detail::ConstantBufferData CDevice::CloneConstantBuffer(const vdl::Detail::ConstantBufferData& _ConstantBuffer)
{
  const CConstantBuffer* pSrcConstantBuffer = Cast<CConstantBuffer>(pBufferManager_->GetBuffer(_ConstantBuffer.GetID()));

  vdl::Detail::ConstantBufferData ConstantBuffer(pSrcConstantBuffer->BufferSize);
  {
    CCopyConstantBuffer* pCopyConstantBuffer = new CCopyConstantBuffer(pSrcConstantBuffer->BufferSize);
    {
      ::memcpy(pCopyConstantBuffer->Buffer, pSrcConstantBuffer->Buffer, pCopyConstantBuffer->BufferSize);

      HRESULT hr = S_OK;

      //  バッファの作成
      {
        D3D11_BUFFER_DESC BufferDesc;
        {
          BufferDesc.ByteWidth = pCopyConstantBuffer->BufferSize;
          BufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
          BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
          BufferDesc.CPUAccessFlags = 0;
          BufferDesc.MiscFlags = 0;
          BufferDesc.StructureByteStride = 0;
        }
        D3D11_SUBRESOURCE_DATA InitialData;
        {
          InitialData.pSysMem = pCopyConstantBuffer->Buffer;
          InitialData.SysMemPitch = 0;
          InitialData.SysMemSlicePitch = 0;
        }

        hr = pD3D11Device_->CreateBuffer(&BufferDesc, &InitialData, pCopyConstantBuffer->pBuffer.GetAddressOf());
        ERROR_CHECK(hr);
      }
    }

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

  Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture2D;
  //  テクスチャの作成
  {
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

    hr = pD3D11Device_->CreateTexture2D(&TextureDesc, &InitialData, pTexture2D.GetAddressOf());
    ERROR_CHECK(hr);
  }

  //  ビューの作成
  {
    hr = pD3D11Device_->CreateShaderResourceView(pTexture2D.Get(), nullptr, pTexture->pShaderResourceView.GetAddressOf());
    ERROR_CHECK(hr);
  }

  (*_ppTexture) = std::move(pTexture);
}

void CDevice::CreateCubeTexture(ITexture** _ppCubeTexture, const std::array<vdl::Image, 6>& _Images)
{
  assert(_ppCubeTexture);

  constexpr DXGI_FORMAT kTextureFormat = Cast(Constants::kTextureFormat);

  CCubeTexture* pCubeTexture = new CCubeTexture;
  pCubeTexture->TextureSize = _Images[0].GetTextureSize();

  HRESULT hr = S_OK;

  Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture2D;
  //  テクスチャの作成
  {
    D3D11_TEXTURE2D_DESC TextureDesc;
    {
      TextureDesc.Width = pCubeTexture->TextureSize.x;
      TextureDesc.Height = pCubeTexture->TextureSize.y;
      TextureDesc.MipLevels = 1;
      TextureDesc.ArraySize = 6;
      TextureDesc.Format = kTextureFormat;
      TextureDesc.SampleDesc.Count = 1;
      TextureDesc.SampleDesc.Quality = 0;
      TextureDesc.Usage = D3D11_USAGE_IMMUTABLE;
      TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
      TextureDesc.CPUAccessFlags = 0;
      TextureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
    }

    std::array<D3D11_SUBRESOURCE_DATA, 6> InitialDatas;
    {
      const vdl::uint Stride = _Images[0].Stride();
      const vdl::uint BufferSize = _Images[0].BufferSize();

      for (vdl::uint i = 0; i < 6; ++i)
      {
        InitialDatas[i].pSysMem = _Images[i].Buffer();
        InitialDatas[i].SysMemPitch = Stride;
        InitialDatas[i].SysMemSlicePitch = BufferSize;
      }
    }

    hr = pD3D11Device_->CreateTexture2D(&TextureDesc, InitialDatas.data(), pTexture2D.GetAddressOf());
    ERROR_CHECK(hr);
  }

  //  ビューの作成
  {
    hr = pD3D11Device_->CreateShaderResourceView(pTexture2D.Get(), nullptr, pCubeTexture->pShaderResourceView.GetAddressOf());
    ERROR_CHECK(hr);
  }

  (*_ppCubeTexture) = std::move(pCubeTexture);
}

void CDevice::CreateRenderTexture(ITexture** _ppRenderTexture, const vdl::uint2& _TextureSize, vdl::FormatType _Format)
{
  assert(_ppRenderTexture);

  CRenderTexture* pRenderTexture = new CRenderTexture;
  pRenderTexture->TextureSize = _TextureSize;
  pRenderTexture->Format = _Format;

  HRESULT hr = S_OK;

  Microsoft::WRL::ComPtr<ID3D11Texture2D> pRenderTarget;
  //  テクスチャの作成
  {
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

    hr = pD3D11Device_->CreateTexture2D(&RenderTargetDesc, nullptr, pRenderTarget.GetAddressOf());
    ERROR_CHECK(hr);
  }

  //  ビューの作成
  {
    hr = pD3D11Device_->CreateShaderResourceView(pRenderTarget.Get(), nullptr, pRenderTexture->pShaderResourceView.GetAddressOf());
    ERROR_CHECK(hr);

    hr = pD3D11Device_->CreateRenderTargetView(pRenderTarget.Get(), nullptr, pRenderTexture->pRenderTargetView.GetAddressOf());
    ERROR_CHECK(hr);
  }

  (*_ppRenderTexture) = std::move(pRenderTexture);
}

void CDevice::CreateDepthStencilTexture(ITexture** _ppDepthStencilTexture, const vdl::uint2& _TextureSize, vdl::FormatType _Format)
{
  assert(_ppDepthStencilTexture);

  CDepthStencilTexture* pDepthStencilTexture = new CDepthStencilTexture;
  pDepthStencilTexture->TextureSize = _TextureSize;
  pDepthStencilTexture->Format = _Format;

  HRESULT hr = S_OK;

  Microsoft::WRL::ComPtr<ID3D11Texture2D> pDepthStencilBuffer;
  //  テクスチャの作成
  {
    D3D11_TEXTURE2D_DESC DepthStencilBufferDesc;
    {
      DepthStencilBufferDesc.Width = _TextureSize.x;
      DepthStencilBufferDesc.Height = _TextureSize.y;
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

    hr = pD3D11Device_->CreateTexture2D(&DepthStencilBufferDesc, nullptr, pDepthStencilBuffer.GetAddressOf());
    ERROR_CHECK(hr);
  }

  //  ビューの作成
  {
    D3D11_DEPTH_STENCIL_VIEW_DESC DepthStencilViewDesc;
    {
      DepthStencilViewDesc.Format = Cast(_Format);
      DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
      DepthStencilViewDesc.Flags = 0;
      DepthStencilViewDesc.Texture2D.MipSlice = 0;
    }

    hr = pD3D11Device_->CreateDepthStencilView(pDepthStencilBuffer.Get(), &DepthStencilViewDesc, pDepthStencilTexture->pDepthStencilView.GetAddressOf());
    ERROR_CHECK(hr);
  }

  (*_ppDepthStencilTexture) = std::move(pDepthStencilTexture);
}

void CDevice::CreateDepthTexture(ITexture** _ppDepthTexture, IDepthStencilTexture* _pDepthStencilTexture)
{
  assert(_ppDepthTexture);

  CDepthStencilTexture* pDepthStencilTexture = Cast<CDepthStencilTexture>(_pDepthStencilTexture);

  CDepthTexture* pDepthTexture = new CDepthTexture;
  pDepthTexture->pParent = pDepthStencilTexture;

  HRESULT hr = S_OK;

  //  ビューの作成
  {
    Microsoft::WRL::ComPtr<ID3D11Resource> pResource;
    pDepthStencilTexture->pDepthStencilView->GetResource(pResource.GetAddressOf());

    D3D11_SHADER_RESOURCE_VIEW_DESC ShaderResourceViewDesc;
    {
      ShaderResourceViewDesc.Format = DepthFormatFromDepthStencilFormat(pDepthStencilTexture->Format);
      ShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
      ShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
      ShaderResourceViewDesc.Texture2D.MipLevels = 1;
    }

    hr = pD3D11Device_->CreateShaderResourceView(pResource.Get(), &ShaderResourceViewDesc, pDepthTexture->pShaderResourceView.GetAddressOf());
    ERROR_CHECK(hr);
  }

  (*_ppDepthTexture) = std::move(pDepthTexture);
}

void CDevice::CreateStencilTexture(ITexture** _ppStencilTexture, IDepthStencilTexture* _pDepthStencilTexture)
{
  assert(_ppStencilTexture);

  CDepthStencilTexture* pDepthStencilTexture = Cast<CDepthStencilTexture>(_pDepthStencilTexture);

  CStencilTexture* pStencilTexture = new CStencilTexture;
  pStencilTexture->pParent = pDepthStencilTexture;

  HRESULT hr = S_OK;

  //  ビューの作成
  {
    Microsoft::WRL::ComPtr<ID3D11Resource> pResource;
    pDepthStencilTexture->pDepthStencilView->GetResource(pResource.GetAddressOf());

    D3D11_SHADER_RESOURCE_VIEW_DESC ShaderResourceViewDesc;
    {
      ShaderResourceViewDesc.Format = StencilFormatFromDepthStencilFormat(pDepthStencilTexture->Format);
      ShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
      ShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
      ShaderResourceViewDesc.Texture2D.MipLevels = 1;
    }

    hr = pD3D11Device_->CreateShaderResourceView(pResource.Get(), &ShaderResourceViewDesc, pStencilTexture->pShaderResourceView.GetAddressOf());
    ERROR_CHECK(hr);
  }

  (*_ppStencilTexture) = std::move(pStencilTexture);
}

void CDevice::CreateUnorderedAccessTexture(ITexture** _ppUnorderedAccessTexture, const vdl::uint2& _TextureSize, vdl::FormatType _Format)
{
  assert(_ppUnorderedAccessTexture);

  CUnorderedAccessTexture* pUnorderedAccessTexture = new CUnorderedAccessTexture;
  pUnorderedAccessTexture->TextureSize = _TextureSize;
  pUnorderedAccessTexture->Format = _Format;

  HRESULT hr = S_OK;

  Microsoft::WRL::ComPtr<ID3D11Texture2D> pUnorderedAccessBuffer;
  //  テクスチャの作成
  {
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
      UnorderedAccessTextureDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
      UnorderedAccessTextureDesc.CPUAccessFlags = 0;
      UnorderedAccessTextureDesc.MiscFlags = 0;
    }

    hr = pD3D11Device_->CreateTexture2D(&UnorderedAccessTextureDesc, nullptr, pUnorderedAccessBuffer.GetAddressOf());
    ERROR_CHECK(hr);
  }

  //  ビューの作成
  {
    hr = pD3D11Device_->CreateShaderResourceView(pUnorderedAccessBuffer.Get(), nullptr, pUnorderedAccessTexture->pShaderResourceView.GetAddressOf());
    ERROR_CHECK(hr);

    hr = pD3D11Device_->CreateUnorderedAccessView(pUnorderedAccessBuffer.Get(), nullptr, pUnorderedAccessTexture->pUnorderedAccessView.GetAddressOf());
    ERROR_CHECK(hr);
  }

  (*_ppUnorderedAccessTexture) = std::move(pUnorderedAccessTexture);
}

void CDevice::WriteMemory(IBuffer* _pDstBuffer, const void* _pSrcBuffer, vdl::uint _BufferSize)const
{
  HRESULT hr = S_OK;

  switch (_pDstBuffer->GetType())
  {
  case BufferType::eVertexBuffer:
  {
    ID3D11Buffer* pBuffer = Cast<CVertexBuffer>(_pDstBuffer)->pBuffer.Get();

    D3D11_MAPPED_SUBRESOURCE MappedSubresorce;
    hr = pD3D11ImmediateContext_->Map(pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresorce);
    ERROR_CHECK(hr);

    ::memcpy(MappedSubresorce.pData, _pSrcBuffer, _BufferSize);

    pD3D11ImmediateContext_->Unmap(pBuffer, 0);
  }
  break;
  case BufferType::eInstanceBuffer:
  {
    ID3D11Buffer* pBuffer = Cast<CInstanceBuffer>(_pDstBuffer)->pBuffer.Get();

    D3D11_MAPPED_SUBRESOURCE MappedSubresorce;
    hr = pD3D11ImmediateContext_->Map(pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresorce);
    ERROR_CHECK(hr);

    ::memcpy(MappedSubresorce.pData, _pSrcBuffer, _BufferSize);

    pD3D11ImmediateContext_->Unmap(pBuffer, 0);
  }
  break;
  case BufferType::eIndexBuffer:
  {
    ID3D11Buffer* pBuffer = Cast<CIndexBuffer>(_pDstBuffer)->pBuffer.Get();

    D3D11_MAPPED_SUBRESOURCE MappedSubresorce;
    hr = pD3D11ImmediateContext_->Map(pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresorce);
    ERROR_CHECK(hr);

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

  Microsoft::WRL::ComPtr<ID3DBlob> pCode;
  ::ComplieShader(pCode.GetAddressOf(), kShaderTargets[static_cast<vdl::uint>(_Type)], _FilePath, _EntryPoint);
  switch (_Type)
  {
    //case ShaderType::eVertexShader:
    //  break;
  case ShaderType::eHullShader:
  {
    CHullShader* pShader = new CHullShader;


    Microsoft::WRL::ComPtr<ID3D11HullShader> pHullShader;
    {
      hr = pD3D11Device_->CreateHullShader(pCode->GetBufferPointer(), pCode->GetBufferSize(), nullptr, pShader->pHullShader.GetAddressOf());
      ERROR_CHECK(hr);
    }

    (*_ppShader) = std::move(pShader);
  }
  break;
  case ShaderType::eDomainShader:
  {
    CDomainShader* pShader = new CDomainShader;

    Microsoft::WRL::ComPtr<ID3D11DomainShader> pDomainShader;
    {
      hr = pD3D11Device_->CreateDomainShader(pCode->GetBufferPointer(), pCode->GetBufferSize(), nullptr, pShader->pDomainShader.GetAddressOf());
      ERROR_CHECK(hr);
    }

    (*_ppShader) = std::move(pShader);
  }
  break;
  case ShaderType::eGeometryShader:
  {
    CGeometryShader* pShader = new CGeometryShader;

    Microsoft::WRL::ComPtr<ID3D11GeometryShader> pGeometryShader;
    {
      hr = pD3D11Device_->CreateGeometryShader(pCode->GetBufferPointer(), pCode->GetBufferSize(), nullptr, pShader->pGeometryShader.GetAddressOf());
      ERROR_CHECK(hr);
    }

    (*_ppShader) = std::move(pShader);
  }
  break;
  case ShaderType::ePixelShader:
  {
    CPixelShader* pShader = new CPixelShader;

    Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
    {
      hr = pD3D11Device_->CreatePixelShader(pCode->GetBufferPointer(), pCode->GetBufferSize(), nullptr, pShader->pPixelShader.GetAddressOf());
      ERROR_CHECK(hr);
    }

    (*_ppShader) = std::move(pShader);
  }
  break;
  case ShaderType::eComputeShader:
  {
    CComputeShader* pShader = new CComputeShader;

    Microsoft::WRL::ComPtr<ID3D11ComputeShader> pComputeShader;
    {
      hr = pD3D11Device_->CreateComputeShader(pCode->GetBufferPointer(), pCode->GetBufferSize(), nullptr, pShader->pComputeShader.GetAddressOf());
      ERROR_CHECK(hr);
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

  Microsoft::WRL::ComPtr<ID3DBlob> pCode;
  ::ComplieShader(pCode.GetAddressOf(), kShaderTargets[static_cast<vdl::uint>(_Type)], _Source, _DataSize, _EntryPoint);
  switch (_Type)
  {
    //case ShaderType::eVertexShader:
    //  break;
  case ShaderType::eHullShader:
  {
    CHullShader* pShader = new CHullShader;

    Microsoft::WRL::ComPtr<ID3D11HullShader> pHullShader;
    {
      hr = pD3D11Device_->CreateHullShader(pCode->GetBufferPointer(), pCode->GetBufferSize(), nullptr, pShader->pHullShader.GetAddressOf());
      ERROR_CHECK(hr);
    }

    (*_ppShader) = std::move(pShader);
  }
  break;
  case ShaderType::eDomainShader:
  {
    CDomainShader* pShader = new CDomainShader;

    Microsoft::WRL::ComPtr<ID3D11DomainShader> pDomainShader;
    {
      hr = pD3D11Device_->CreateDomainShader(pCode->GetBufferPointer(), pCode->GetBufferSize(), nullptr, pShader->pDomainShader.GetAddressOf());
      ERROR_CHECK(hr);
    }

    (*_ppShader) = std::move(pShader);
  }
  break;
  case ShaderType::eGeometryShader:
  {
    CGeometryShader* pShader = new CGeometryShader;

    Microsoft::WRL::ComPtr<ID3D11GeometryShader> pGeometryShader;
    {
      hr = pD3D11Device_->CreateGeometryShader(pCode->GetBufferPointer(), pCode->GetBufferSize(), nullptr, pShader->pGeometryShader.GetAddressOf());
      ERROR_CHECK(hr);
    }

    (*_ppShader) = std::move(pShader);
  }
  break;
  case ShaderType::ePixelShader:
  {
    CPixelShader* pShader = new CPixelShader;

    Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
    {
      hr = pD3D11Device_->CreatePixelShader(pCode->GetBufferPointer(), pCode->GetBufferSize(), nullptr, pShader->pPixelShader.GetAddressOf());
      ERROR_CHECK(hr);
    }

    *_ppShader = std::move(pShader);
  }
  break;
  case ShaderType::eComputeShader:
  {
    CComputeShader* pShader = new CComputeShader;

    Microsoft::WRL::ComPtr<ID3D11ComputeShader> pComputeShader;
    {
      hr = pD3D11Device_->CreateComputeShader(pCode->GetBufferPointer(), pCode->GetBufferSize(), nullptr, pShader->pComputeShader.GetAddressOf());
      ERROR_CHECK(hr);
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

  HRESULT hr = S_OK;

  Microsoft::WRL::ComPtr<ID3DBlob> pCode;
  ::ComplieShader(pCode.GetAddressOf(), kShaderTargets[static_cast<vdl::uint>(ShaderType::eVertexShader)], _FilePath, _EntryPoint);

  Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
  {
    hr = pD3D11Device_->CreateVertexShader(pCode->GetBufferPointer(), pCode->GetBufferSize(), nullptr, pShader->pVertexShader.GetAddressOf());
    ERROR_CHECK(hr);
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

  HRESULT hr = S_OK;

  Microsoft::WRL::ComPtr<ID3DBlob> pCode;
  ::ComplieShader(pCode.GetAddressOf(), kShaderTargets[static_cast<vdl::uint>(ShaderType::eVertexShader)], _Source, _DataSize, _EntryPoint);

  Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
  {
    hr = pD3D11Device_->CreateVertexShader(pCode->GetBufferPointer(), pCode->GetBufferSize(), nullptr, pShader->pVertexShader.GetAddressOf());
    ERROR_CHECK(hr);
  }

  (*_ppVertexShader) = std::move(pShader);

  //  インプットレイアウトの登録
  if (!pDeviceContext_->isFoundInputLayout(_InputLayout))
  {
    pDeviceContext_->RegisterInputLayout(_InputLayout, pCode.Get());
  }
}
