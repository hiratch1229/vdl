#pragma once
#include <vdl/Fwd.hpp>
#include <vdl/Types.hpp>
#include <vdl/ConstantBuffer.hpp>

#include <vdl/Platform/Platform.hpp>
#include <vdl/Buffer/Buffer.hpp>

#include <vdl/Shader/IShader.hpp>
#include <vdl/Buffer/IBuffer.hpp>
#include <vdl/Texture/ITexture.hpp>

#include <array>

class IDevice
{
public:
  static IDevice* Create();

  IDevice() = default;

  virtual ~IDevice() = default;

  [[nodiscard]] virtual PlatformFlags GetPlatform()const = 0;

  virtual void Initialize() = 0;

  virtual void CreateVertexBuffer(IBuffer** _ppVertexBuffer, vdl::uint _BufferSize) = 0;

  virtual void CreateVertexBuffer(IBuffer** _ppVertexBuffer, const void* _Vertices, vdl::uint _BufferSize) = 0;

  virtual void CreateInstanceBuffer(IBuffer** _ppInstanceBuffer, vdl::uint _BufferSize) = 0;

  virtual void CreateIndexBuffer(IBuffer** _ppIndexBuffer, vdl::uint _BufferSize, IndexType _IndexType) = 0;

  virtual void CreateIndexBuffer(IBuffer** _ppIndexBuffer, const void* _Indices, vdl::uint _BufferSize, IndexType _IndexType) = 0;

  virtual void CreateConstantBuffer(IBuffer** _ppConstantBuffer, vdl::uint _BufferSize) = 0;

  virtual void CreateUnorderedAccessBuffer(IBuffer** _ppUnorderedAccessBuffer, vdl::uint _Stride, vdl::uint _BufferSize, const void* _Buffer) = 0;

  virtual vdl::Detail::ConstantBufferData CloneConstantBuffer(const vdl::Detail::ConstantBufferData& _ConstantBuffer) = 0;

  virtual void CreateTexture(ITexture** _ppTexture, const vdl::Image& _Image) = 0;

  virtual void CreateCubeTexture(ITexture** _ppCubeTexture, const std::array<vdl::Image, 6>& _Images) = 0;

  virtual void CreateRenderTexture(ITexture** _ppRenderTexture, const vdl::uint2& _TextureSize, vdl::FormatType _Format) = 0;

  virtual void CreateDepthStencilTexture(ITexture** _ppDepthStencilTexture, const vdl::uint2& _TextureSize, vdl::FormatType _Format) = 0;

  virtual void CreateDepthTexture(ITexture** _ppDepthTexture, IDepthStencilTexture* _pDepthStencilTexture) = 0;

  virtual void CreateStencilTexture(ITexture** _ppStencilTexture, IDepthStencilTexture* _pDepthStencilTexture) = 0;

  virtual void CreateUnorderedAccessTexture(ITexture** _ppUnorderedAccessTexture, const vdl::uint2& _TextureSize, vdl::FormatType _Format) = 0;

  virtual void WriteMemory(IBuffer* _pDstBuffer, const void* _pSrcBuffer, vdl::uint _BufferSize)const = 0;

  virtual void LoadShader(IShader** _ppShader, const char* _FilePath, const char* _EntryPoint, ShaderType _Type) = 0;

  virtual void LoadShader(IShader** _ppShader, const char* _Source, vdl::uint _DataSize, const char* _EntryPoint, ShaderType _Type) = 0;

  virtual void LoadShader(IVertexShader** _ppVertexShader, const char* _FilePath, const char* _EntryPoint, vdl::InputLayoutType _InputLayout) = 0;

  virtual void LoadShader(IVertexShader** _ppVertexShader, const char* _Source, vdl::uint _DataSize, const char* _EntryPoint, vdl::InputLayoutType _InputLayout) = 0;

  virtual void WaitIdle() = 0;
};
