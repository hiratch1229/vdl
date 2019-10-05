#include "SceneGPUParticle.hpp"

void SceneGPUParticle::Initialize()
{
  UnorderedAccessBuffer_ = vdl::UnorderedAccessBuffer<vdl::int4>(kBufferNum);
  vdl::UnorderedAccessTexture A(1,vdl::FormatType::eR8G8B8A8_Uint);

  vdl::Computer::SetShader(vdl::ComputeShader("Shader/GPUParticle/GPUParticleInitCS.hlsl"));
  vdl::Computer::SetUnorderedAccessObjects(0, 1, &UnorderedAccessBuffer_);

  vdl::Computer::Dispatch(1, 1, 1);
}

SceneGPUParticle::~SceneGPUParticle()
{

}

void SceneGPUParticle::Update()
{

}
