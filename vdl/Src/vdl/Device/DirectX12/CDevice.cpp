#include "CDevice.hpp"

void CDevice::Initialize()
{

}

void CDevice::CreateVertexBuffer(IBuffer** _ppVertexBuffer, vdl::uint _Stride, vdl::uint _BufferSize)
{

}

void CDevice::CreateVertexBuffer(IBuffer** _ppVertexBuffer, void* _Vertices, vdl::uint _Stride, vdl::uint _BufferSize)
{

}

void CDevice::CreateIndexBuffer(IBuffer** _ppIndexBuffer, vdl::uint _Stride, vdl::uint _BufferSize)
{

}

void CDevice::CreateIndexBuffer(IBuffer** _ppIndexBuffer, void* _Indices, vdl::uint _Stride, vdl::uint _BufferSize)
{

}

void CDevice::CreateConstantBuffer(IBuffer** _ppConstantBuffer, vdl::uint _BufferSize)
{

}

void CDevice::CreateTexture(ITexture** _ppTexture, const vdl::Image& _Image)
{

}

void CDevice::CreateRenderTexture(ITexture** _ppRenderTexture, const vdl::uint2& _TextureSize, vdl::Format _Format)
{

}

void CDevice::CreateDepthStecilTexture(ITexture** _ppDepthStecilTexture, const vdl::uint2& _TextureSize, vdl::Format _Format)
{

}

void CDevice::LoadShader(IShader** _ppShader, const char* _Source, vdl::uint _DataSize, const char* _EntryPoint, ShaderType _Type)
{

}

void CDevice::LoadShader(IVertexShader** _ppVertexShader, const char* _Source, vdl::uint _DataSize, const char* _EntryPoint, vdl::RenderType _Type)
{

}

void CDevice::WriteMemory(IBuffer* _pSrcBuffer, void* _pDstBuffer, vdl::uint _BufferSize)const
{

}
