#include "CDevice.hpp"

void CDevice::Initialize()
{

}

void CDevice::CreateVertexBuffer(IBuffer** _ppVertexBuffer, vdl::uint _Stride, vdl::uint _BufferSize)
{

}

void CDevice::CreateVertexBuffer(IBuffer** _ppVertexBuffer, const void* _Vertices, vdl::uint _Stride, vdl::uint _BufferSize)
{

}

void CDevice::CreateInstanceBuffer(IBuffer** _ppInstanceBuffer, vdl::uint _Stride, vdl::uint _BufferSize)
{

}

void CDevice::CreateIndexBuffer(IBuffer** _ppIndexBuffer, vdl::uint _BufferSize, IndexType _IndexType)
{

}

void CDevice::CreateIndexBuffer(IBuffer** _ppIndexBuffer, const void* _Indices, vdl::uint _BufferSize, IndexType _IndexType)
{

}

void CDevice::CreateConstantBuffer(IBuffer** _ppConstantBuffer, vdl::uint _BufferSize)
{

}

void CDevice::CreateUnorderedAccessBuffer(IBuffer** _ppUnorderedAccessBuffer, vdl::uint _Stride, vdl::uint _BufferSize, void* _Buffer)
{

}

void CDevice::CreateTexture(ITexture** _ppTexture, const vdl::Image& _Image)
{

}

void CDevice::CreateRenderTexture(ITexture** _ppRenderTexture, const vdl::uint2& _TextureSize, vdl::FormatType _Format)
{

}

void CDevice::CreateDepthStecilTexture(ITexture** _ppDepthStecilTexture, const vdl::uint2& _TextureSize, vdl::FormatType _Format)
{

}

void CDevice::CreateUnorderedAccessTexture(ITexture** _ppUnorderedAccessTexture, const vdl::uint2& _TextureSize, vdl::FormatType _Format)
{

}

void CDevice::WriteMemory(IBuffer* _pDstBuffer, const void* _pSrcBuffer, vdl::uint _BufferSize)const
{

}

void CDevice::LoadShader(IShader** _ppShader, const char* _FilePath, const char* _EntryPoint, ShaderType _Type)
{

}

void CDevice::LoadShader(IShader** _ppShader, const char* _Source, vdl::uint _DataSize, const char* _EntryPoint, ShaderType _Type)
{

}

void CDevice::LoadShader(IVertexShader** _ppVertexShader, const char* _FilePath, const char* _EntryPoint, vdl::InputLayoutType _InputLayout)
{

}

void CDevice::LoadShader(IVertexShader** _ppVertexShader, const char* _Source, vdl::uint _DataSize, const char* _EntryPoint, vdl::InputLayoutType _InputLayout)
{

}
