#pragma once
#include "../IDevice.hpp"

class CDevice : public IDevice
{
public:
  void Initialize()override;

  void CreateVertexBuffer(IBuffer** _ppVertexBuffer, vdl::uint _Stride, vdl::uint _BufferSize)override;

  void CreateVertexBuffer(IBuffer** _ppVertexBuffer, void* _Vertices, vdl::uint _Stride, vdl::uint _BufferSize)override;

  void CreateIndexBuffer(IBuffer** _ppIndexBuffer, vdl::uint _Stride, vdl::uint _BufferSize)override;

  void CreateIndexBuffer(IBuffer** _ppIndexBuffer, void* _Indices, vdl::uint _Stride, vdl::uint _BufferSize)override;

  void CreateConstantBuffer(IBuffer** _ppConstantBuffer, vdl::uint _BufferSize)override;

  void CreateTexture(ITexture** _ppTexture, const vdl::Image& _Image)override;

  void CreateRenderTexture(ITexture** _ppRenderTexture, const vdl::uint2& _TextureSize, vdl::Format _Format)override;

  void CreateDepthStecilTexture(ITexture** _ppDepthStecilTexture, const vdl::uint2& _TextureSize, vdl::Format _Format)override;

  void LoadShader(IShader** _ppShader, const char* _Source, vdl::uint _DataSize, const char* _EntryPoint, ShaderType _Type)override;

  void LoadShader(IVertexShader** _ppVertexShader, const char* _Source, vdl::uint _DataSize, const char* _EntryPoint, vdl::RenderType _Type)override;

  void WriteMemory(IBuffer* _pSrcBuffer, void* _pDstBuffer, vdl::uint _BufferSize)const override;
};
