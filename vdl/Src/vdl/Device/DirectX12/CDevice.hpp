#pragma once
#include "../IDevice.hpp"

#include <vdl/MemoryAllocator/MemoryAllocator.hpp>

#include <vdl/Buffer/DirectX12/CBuffer.hpp>
#include <vdl/Texture/DirectX12/CTexture.hpp>
#include <vdl/CommandList/DirectX12/CommandList.hpp>

#include <vdl/pch/DirectX12/pch.hpp>

class IBufferManager;

class CDevice : public IDevice
{
  Microsoft::WRL::ComPtr<ID3D12Device5> pDevice_;
  Microsoft::WRL::ComPtr<ID3D12CommandQueue> pCommandQueue_;
  CommandList CommandList_;
  Microsoft::WRL::ComPtr<ID3D12Fence> pFence_;
  Microsoft::WRL::ComPtr<IDXGIFactory6> pFactory_;
  HANDLE FenceEvent_;
  vdl::uint FenceValue_ = 0;
private:
  IBufferManager* pBufferManager_;
  std::unique_ptr<CConstantBuffer> pConstantBuffer_;
  MemoryAllocator ConstantBufferAllocator_;
public:
  [[nodiscard]] ID3D12Device5* GetDevice() { return pDevice_.Get(); }
  [[nodiscard]] IDXGIFactory6* GetFactory() { return pFactory_.Get(); }
public:
  void CreateDescriptorHeap(ID3D12DescriptorHeap** _ppDescriptorHeap, D3D12_DESCRIPTOR_HEAP_TYPE _Type)const;
public:
  void ExecuteAndWait(ID3D12CommandList* _pCommandList);
private:
  void CreateResource(ID3D12Resource** _ppResource, const D3D12_RESOURCE_DESC& _ResourceDesc, D3D12_HEAP_TYPE _HeapType, D3D12_RESOURCE_STATES _InitialResourceState)const;
  void CreateBuffer(BufferData* _pBufferData, vdl::uint _BufferSize, D3D12_HEAP_TYPE _HeapType, D3D12_RESOURCE_STATES _InitialResourceState, D3D12_RESOURCE_FLAGS _ResourceFlags = D3D12_RESOURCE_FLAG_NONE)const;
  void CreateStagingBuffer(BufferData* _pBuffer, const void* _Buffer, vdl::uint _BufferSize)const;
  void CopyBuffer(BufferData* _pSrcBuffer, BufferData* _pDstBuffer, vdl::uint _BufferSize, D3D12_RESOURCE_STATES _AfterState);
  void CreateTexture(TextureData* _pTextureData, const D3D12_RESOURCE_DESC& _TextureDesc, D3D12_RESOURCE_STATES _InitialResourceState)const;
public:
  CDevice() = default;

  PlatformFlags GetPlatform()const final { return PlatformFlag::eDirectX12; }

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

  void WriteMemory(IBuffer * _pDstBuffer, const void* _pSrcBuffer, vdl::uint _BufferSize)const override;

  void LoadShader(IShader** _ppShader, const char* _FilePath, const char* _EntryPoint, ShaderType _Type)override;

  void LoadShader(IShader** _ppShader, const char* _Source, vdl::uint _DataSize, const char* _EntryPoint, ShaderType _Type)override;

  void LoadShader(IVertexShader** _ppVertexShader, const char* _FilePath, const char* _EntryPoint, vdl::InputLayoutType _InputLayout)override;

  void LoadShader(IVertexShader** _ppVertexShader, const char* _Source, vdl::uint _DataSize, const char* _EntryPoint, vdl::InputLayoutType _InputLayout)override;

  void WaitIdle()override {}
};
