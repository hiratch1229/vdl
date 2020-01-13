#pragma once
#include "../IDevice.hpp"

#include <vdl/pch/DirectX11/pch.hpp>

class CDeviceContext;
class ITextureManager;
class IBufferManager;

class CDevice : public IDevice
{
  Microsoft::WRL::ComPtr<ID3D11Device> pD3D11Device_;
  Microsoft::WRL::ComPtr<ID3D11DeviceContext> pD3D11ImmediateContext_;
private:
  CDeviceContext* pDeviceContext_;
  ITextureManager* pTextureManager_;
  IBufferManager* pBufferManager_;
public:
  [[nodiscard]] ID3D11Device* GetDevice()const { return pD3D11Device_.Get(); }
  [[nodiscard]] ID3D11DeviceContext* GetImmediateContext()const { return pD3D11ImmediateContext_.Get(); }
public:
  CDevice() = default;

  PlatformFlags GetPlatform()const final { return PlatformFlag::eDirectX11; }

  void Initialize()override;

  void CreateVertexBuffer(IBuffer** _ppVertexBuffer, vdl::uint _BufferSize)override;

  void CreateVertexBuffer(IBuffer** _ppVertexBuffer, const void* _Vertices, vdl::uint _BufferSize)override;

  void CreateInstanceBuffer(IBuffer** _ppInstanceBuffer, vdl::uint _BufferSize)override;

  void CreateIndexBuffer(IBuffer** _ppIndexBuffer, vdl::uint _BufferSize, IndexType _IndexType)override;

  void CreateIndexBuffer(IBuffer** _ppIndexBuffer, const void* _Indices, vdl::uint _BufferSize, IndexType _IndexType)override;

  void CreateConstantBuffer(IBuffer** _ppConstantBuffer, vdl::uint _BufferSize)override;

  void CreateUnorderedAccessBuffer(IBuffer** _ppUnorderedAccessBuffer, vdl::uint _Stride, vdl::uint _BufferSize, const void* _Buffer)override;

  vdl::Detail::ConstantBufferData CloneConstantBuffer(const vdl::Detail::ConstantBufferData& _ConstantBuffer)override;

  void CreateTexture(ITexture** _ppTexture, const vdl::Image& _Image)override;

  void CreateCubeTexture(ITexture** _ppCubeTexture, const std::array<vdl::Image, 6>& _Images)override;

  void CreateRenderTexture(ITexture** _ppRenderTexture, const vdl::uint2& _TextureSize, vdl::FormatType _Format)override;

  void CreateDepthStencilTexture(ITexture** _ppDepthStencilTexture, const vdl::uint2& _TextureSize, vdl::FormatType _Format)override;

  void CreateDepthTexture(ITexture** _ppDepthTexture, IDepthStencilTexture* _pDepthStencilTexture)override;

  void CreateStencilTexture(ITexture** _ppStencilTexture, IDepthStencilTexture* _pDepthStencilTexture)override;

  void CreateUnorderedAccessTexture(ITexture** _ppUnorderedAccessTexture, const vdl::uint2& _TextureSize, vdl::FormatType _Format)override;

  void WriteMemory(IBuffer* _pDstBuffer, const void* _pSrcBuffer, vdl::uint _BufferSize)const override;

  void LoadShader(IShader** _ppShader, const char* _FilePath, const char* _EntryPoint, ShaderType _Type)override;

  void LoadShader(IShader** _ppShader, const char* _Source, vdl::uint _DataSize, const char* _EntryPoint, ShaderType _Type)override;

  void LoadShader(IVertexShader** _ppVertexShader, const char* _FilePath, const char* _EntryPoint, vdl::InputLayoutType _InputLayout)override;

  void LoadShader(IVertexShader** _ppVertexShader, const char* _Source, vdl::uint _DataSize, const char* _EntryPoint, vdl::InputLayoutType _InputLayout)override;

  void WaitIdle()override {}
};
