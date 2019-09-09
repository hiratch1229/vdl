#pragma once
#include <vdl/Fwd.hpp>
#include <vdl/Types.hpp>

#include <vdl/Shader/IShader.hpp>
#include <vdl/Buffer/IBuffer.hpp>
#include <vdl/Texture/ITexture.hpp>

class IDevice
{
public:
  static IDevice* Create();

  IDevice() = default;

  virtual ~IDevice() = default;

  virtual void Initialize() = 0;

  virtual void CreateVertexBuffer(IBuffer** _ppVertexBuffer, vdl::uint _Stride, vdl::uint _BufferSize) = 0;

  virtual void CreateVertexBuffer(IBuffer** _ppVertexBuffer, void* _Vertices, vdl::uint _Stride, vdl::uint _BufferSize) = 0;

  virtual void CreateIndexBuffer(IBuffer** _ppIndexBuffer, vdl::uint _Stride, vdl::uint _BufferSize) = 0;

  virtual void CreateIndexBuffer(IBuffer** _ppIndexBuffer, void* _Indices, vdl::uint _Stride, vdl::uint _BufferSize) = 0;

  virtual void CreateInstanceBuffer(IBuffer** _ppInstanceBuffer, vdl::uint _Stride, vdl::uint _BufferSize) = 0;

  virtual void CreateConstantBuffer(IBuffer** _ppConstantBuffer, vdl::uint _BufferSize) = 0;

  virtual void CreateTexture(ITexture** _ppTexture, const vdl::Image& _Image) = 0;

  virtual void CreateRenderTexture(ITexture** _ppRenderTexture, const vdl::uint2& _TextureSize, vdl::Format _Format) = 0;

  virtual void CreateDepthStecilTexture(ITexture** _ppDepthStecilTexture, const vdl::uint2& _TextureSize, vdl::Format _Format) = 0;

  virtual void LoadShader(IShader** _ppShader, const char* _Source, vdl::uint _DataSize, const char* _EntryPoint, ShaderType _Type) = 0;

  virtual void LoadShader(IVertexShader** _ppVertexShader, const char* _Source, vdl::uint _DataSize, const char* _EntryPoint, vdl::InputLayout _Type) = 0;

  virtual void WriteMemory(IBuffer* _pSrcBuffer, void* _pDstBuffer, vdl::uint _BufferSize)const = 0;
};
