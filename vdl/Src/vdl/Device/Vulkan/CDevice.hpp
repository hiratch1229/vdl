#pragma once
#include "../IDevice.hpp"

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.hpp>

#include <vdl/Buffer/Vulkan/CBuffer.hpp>

class CDevice : public IDevice
{
  vk::UniqueInstance Instance_;
  vk::UniqueDevice Device_;
  vk::PhysicalDevice PhysicalDevice_;
  vk::UniqueCommandPool CommandPool_;
  vk::UniqueCommandBuffer CommandBuffer_;
  vk::Queue GraphicsQueue_;
  vk::Queue ComputeQueue_;
  vk::Queue CopyQueue_;
private:
  vdl::uint GraphicsQueueIndex_;
  vdl::uint ComputeQueueIndex_;
  vdl::uint CopyQueueIndex_;
private:
#if defined(DEBUG) | defined(_DEBUG)
  PFN_vkDestroyDebugReportCallbackEXT	DestroyReportFunction_;
  vk::DebugReportCallbackEXT DebugReportCallBack_;
#endif
public:
  [[nodiscard]] const vk::CommandBuffer& GetCommandBuffer()const { return CommandBuffer_.get(); }
private:
  vdl::uint FindQueue(vk::QueueFlags _QueueFlag, const vk::SurfaceKHR& _Surface = vk::SurfaceKHR())const;
  vdl::uint GetMemoryTypeIndex(vdl::uint _MemoryTypeBits, const vk::MemoryPropertyFlags& _MemoryPropertys)const;
  void CreateBuffer(BufferData* _pBuffer, vk::DeviceSize _BufferSize, vk::BufferUsageFlags _Usage, vk::MemoryPropertyFlags _Properties)const;
  void CreateStagingBuffer(BufferData* _pStagingBuffer, const void* _Buffer, vdl::uint _BufferSize)const;
  void CopyBuffer(vk::Buffer _SrcBuffer, vk::Buffer _DstBuffer, vdl::uint _BufferSize)const;
  void WriteMemory(BufferData* _pDstBuffer, const void* _pSrcBuffer, vdl::uint _BufferSize)const;
public:
  void SubmitAndWait(vk::SubmitInfo _SubmitInfo, vk::QueueFlags _QueueFlag)const;
public:
  CDevice() = default;

  ~CDevice();

  void Initialize()override;

  void CreateVertexBuffer(IBuffer** _ppVertexBuffer, vdl::uint _Stride, vdl::uint _BufferSize)override;

  void CreateVertexBuffer(IBuffer** _ppVertexBuffer, const void* _Vertices, vdl::uint _Stride, vdl::uint _BufferSize)override;

  void CreateInstanceBuffer(IBuffer** _ppInstanceBuffer, vdl::uint _Stride, vdl::uint _BufferSize)override;

  void CreateIndexBuffer(IBuffer** _ppIndexBuffer, vdl::uint _BufferSize, IndexType _IndexType)override;

  void CreateIndexBuffer(IBuffer** _ppIndexBuffer, const void* _Indices, vdl::uint _BufferSize, IndexType _IndexType)override;

  void CreateConstantBuffer(IBuffer** _ppConstantBuffer, vdl::uint _BufferSize)override;

  void CreateUnorderedAccessBuffer(IBuffer** _ppUnorderedAccessBuffer, vdl::uint _Stride, vdl::uint _BufferSize, const void* _Buffer)override;

  void CreateTexture(ITexture** _ppTexture, const vdl::Image& _Image)override;

  void CreateRenderTexture(ITexture** _ppRenderTexture, const vdl::uint2& _TextureSize, vdl::FormatType _Format)override;

  void CreateDepthStecilTexture(ITexture** _ppDepthStecilTexture, const vdl::uint2& _TextureSize, vdl::FormatType _Format)override;

  void CreateUnorderedAccessTexture(ITexture** _ppUnorderedAccessTexture, const vdl::uint2& _TextureSize, vdl::FormatType _Format)override;

  void WriteMemory(IBuffer* _pDstBuffer, const void* _pSrcBuffer, vdl::uint _BufferSize)const override;

  void LoadShader(IShader** _ppShader, const char* _FilePath, const char* _EntryPoint, ShaderType _Type)override;

  void LoadShader(IShader** _ppShader, const char* _Source, vdl::uint _DataSize, const char* _EntryPoint, ShaderType _Type)override;

  void LoadShader(IVertexShader** _ppVertexShader, const char* _FilePath, const char* _EntryPoint, vdl::InputLayoutType _InputLayout)override;

  void LoadShader(IVertexShader** _ppVertexShader, const char* _Source, vdl::uint _DataSize, const char* _EntryPoint, vdl::InputLayoutType _InputLayout)override;
};
