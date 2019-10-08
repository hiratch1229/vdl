#pragma once
#include "../IDevice.hpp"

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.hpp>

#include <vdl/Buffer/Vulkan/CBuffer.hpp>

class CDevice : public IDevice
{
  static constexpr vdl::uint kInstanceBufferSizeMultiple = 5;
private:
  vk::UniqueInstance Instance_;
  vk::UniqueDevice VkDevice_;
  vk::PhysicalDevice PhysicalDevice_;
  vk::UniqueCommandPool CommandPool_;
  vk::UniqueCommandBuffer CommandBuffer_;
  vk::Queue GraphicsQueue_;
  vk::Queue ComputeQueue_;
private:
  vdl::uint GraphicsQueueIndex_;
  vdl::uint ComputeQueueIndex_;
private:
#if defined(DEBUG) | defined(_DEBUG)
  PFN_vkDestroyDebugReportCallbackEXT	DestroyReportFunction_;
  vk::DebugReportCallbackEXT DebugReportCallBack_;
#endif
public:
  [[nodiscard]] const vk::Instance& GetInstance()const { return Instance_.get(); }
  [[nodiscard]] const vk::Device& GetDevice()const { return VkDevice_.get(); }
  [[nodiscard]] const vk::PhysicalDevice& GetPhysicalDevice()const { return PhysicalDevice_; }
  [[nodiscard]] const vk::Queue& GetGraphicsQueue()const { return GraphicsQueue_; }
  [[nodiscard]] const vk::Queue& GetComputeQueue()const { return ComputeQueue_; }
  [[nodiscard]] const vk::CommandBuffer& GetCommandBuffer()const { return CommandBuffer_.get(); }
  [[nodiscard]] vdl::uint GetGraphicsQueueIndex()const { return GraphicsQueueIndex_; }
  [[nodiscard]] vdl::uint GetComputeQueueIndex()const { return ComputeQueueIndex_; }
public:
  [[nodiscard]] vdl::uint FindQueue(vk::QueueFlags _QueueFlag, const vk::SurfaceKHR& _Surface = vk::SurfaceKHR())const { return FindQueue(_QueueFlag, vk::QueueFlags(), _Surface); }
  [[nodiscard]] vdl::uint GetMemoryTypeIndex(vdl::uint _MemoryTypeBits, const vk::MemoryPropertyFlags& _MemoryPropertys)const;
  void SubmitAndWait(vk::SubmitInfo _SubmitInfo)const;
private:
  [[nodiscard]] vdl::uint FindQueue(vk::QueueFlags _QueueFlag, vk::QueueFlags _NotFlag, const vk::SurfaceKHR& _Surface = vk::SurfaceKHR())const;
  void CreateBuffer(BufferData* _pBuffer, vk::DeviceSize _BufferSize, vk::BufferUsageFlags _Usage, vk::MemoryPropertyFlags _Properties)const;
  void CreateStagingBuffer(BufferData* _pStagingBuffer, const void* _Buffer, vdl::uint _BufferSize)const;
  void CopyBuffer(vk::Buffer _SrcBuffer, vk::Buffer _DstBuffer, vdl::uint _BufferSize)const;
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

  void WaitIdle()override;
};
