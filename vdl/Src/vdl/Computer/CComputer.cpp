#include "CComputer.hpp"

#include <vdl/Engine.hpp>
#include <vdl/DeviceContext/IDeviceContext.hpp>
#include <vdl/Renderer/IRenderer.hpp>

void CComputer::Initialize()
{
  pDeviceContext_ = Engine::Get<IDeviceContext>();
  pRenderer_ = Engine::Get<IRenderer>();

  ComputeCommandList_.Initialize();
}

void CComputer::SetShader(const vdl::ComputeShader& _ComputeShader)
{
  ComputeCommandList_.SetShader(_ComputeShader);
}

void CComputer::SetShaderResource(vdl::uint _StartSlot, vdl::uint _ShaderResourceNum, const vdl::ShaderResource _ShaderResources[])
{
  ComputeCommandList_.SetShaderResources(_StartSlot, _ShaderResourceNum, _ShaderResources);
}

void CComputer::SetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[])
{
  ComputeCommandList_.SetSamplers(_StartSlot, _SamplerNum, _Samplers);
}

void CComputer::SetConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[])
{
  ComputeCommandList_.SetConstantBuffers(_StartSlot, _ConstantBufferNum, _ConstantBuffers);
}

void CComputer::SetUnorderedAccessObjects(vdl::uint _StartSlot, vdl::uint _UnorderedAccessObjectNum, const vdl::UnorderedAccessObject _UnorderedAccessObjects[])
{
  ComputeCommandList_.SetUnorderedAccessObjects(_StartSlot, _UnorderedAccessObjectNum, _UnorderedAccessObjects);
}

void CComputer::Dispatch(vdl::uint _ThreadGroupX, vdl::uint _ThreadGroupY, vdl::uint _ThreadGroupZ)
{
  pRenderer_->Flush();

  pDeviceContext_->SetRenderTextures(vdl::RenderTextures(), vdl::DepthStencilTexture());

  ComputeCommandList_.Dispatch(_ThreadGroupX, _ThreadGroupY, _ThreadGroupZ);

  pDeviceContext_->Execute(ComputeCommandList_);

  ComputeCommandList_.Reset();
}
