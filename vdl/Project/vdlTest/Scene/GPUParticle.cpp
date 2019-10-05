#include "SceneGPUParticle.hpp"

void SceneGPUParticle::Initialize()
{
  UnorderedAccessBuffer_ = vdl::UnorderedAccessBuffer<int>(kBufferNum);

  vdl::Computer::SetShader(vdl::ComputeShader("Shader/GPUParticle/GPUParticleInitCS.hlsl"));
  vdl::Computer::SetUnorderedAccessObjects(0, 1, &UnorderedAccessBuffer_);
}

SceneGPUParticle::~SceneGPUParticle()
{

}

void SceneGPUParticle::Update()
{

}
