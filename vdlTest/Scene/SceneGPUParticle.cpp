#include "SceneGPUParticle.hpp"

using namespace vdl;

void SceneGPUParticle::Initialize()
{
  //  バッファの初期化
  {
    UnorderedAccessBuffer_ = UnorderedAccessBuffer(kBufferNum);

    Computer::SetShader(ComputeShader("Shader/GPUParticle/GPUParticleInitCS.hlsl"));
    Computer::SetUnorderedAccessObjects(0, 1, &UnorderedAccessBuffer_);

    Computer::Dispatch(kThreadGroupNum, 1, 1);
  }

  Computer::SetShader(ComputeShader("Shader/GPUParticle/GPUParticleCS.hlsl"));

  Computer::SetConstantBuffers(0, 1, &CSConstantBuffer_);

  Renderer::SetShaders(VertexShader("Shader/GPUParticle/GPUParticleVS.hlsl", InputLayoutType::eNone),
    GeometryShader("Shader/GPUParticle/GPUParticleGS.hlsl"),
    PixelShader("Shader/GPUParticle/GPUParticlePS.hlsl"));

  Renderer::SetVertexStageShaderResources(0, 1, &UnorderedAccessBuffer_);

  GSConstantBuffer_.GetData().PointHalfSize = 1.0f;
  Renderer::SetGeometryStageConstantBuffers(0, 1, &GSConstantBuffer_);

  Renderer::SetDepthStencilState(DepthStencilState::kDefault2D);

  Renderer::SetTopology(TopologyType::ePointList);
}

void SceneGPUParticle::Update()
{
  CSConstantBufferData& CSData = CSConstantBuffer_.GetData();
  {
    CSData.MousePosition = Input::Mouse::GetPos();
    CSData.DeltaTime = System::GetDeltaTime();
  }

  GUI::Begin("SceneGPUParticle");
  {
    GUI::Text("ParticleNum:%d", kBufferNum);
    GUI::Text((std::string("Target:") + std::to_string(CSData.MousePosition)).c_str());
    GUI::Input("PointHalfSize", &GSConstantBuffer_.GetData().PointHalfSize);
  }
  GUI::End();

  Computer::Dispatch(kThreadGroupNum, 1, 1);

  Renderer::Draw(kBufferNum);
}
