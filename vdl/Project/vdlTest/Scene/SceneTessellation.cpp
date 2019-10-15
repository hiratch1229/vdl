#include "SceneTessellation.hpp"

using namespace vdl;

void SceneTessellation::Initialize()
{
  Rectangle_ = MeshData::Rectangle();
  Camera_ = { vdl::float3(0.0f, 0.0f, -15.0f) };

  Renderer3D::SetShaders(VertexShader("Shader/Tessellation/TessellationVS.hlsl", InputLayoutType::eMesh),
    HullShader("Shader/Tessellation/TessellationHS.hlsl"),
    DomainShader("Shader/Tessellation/TessellationDS.hlsl"),
    PixelShader("Shader/Tessellation/TessellationPS.hlsl"));

  Renderer3D::SetTopology(TopologyType::ePatchList3ControlPoint);

  Renderer3D::SetRasterizerState(RasterizerState::kWireframeCullBack);

  Renderer3D::SetHullStageConstantBuffers(0, 1, &ConstantBuffer_);

  ConstantBufferData& Data = ConstantBuffer_.GetData();
  {
    Data.TessFactor = kTessellationMinValue;
    Data.InsideFactor = kTessellationMinValue;
  }
}

SceneTessellation::~SceneTessellation()
{
  Renderer3D::SetShaders(VertexShader("Shader/Mesh/MeshVS.hlsl", InputLayoutType::eMesh),
    PixelShader("Shader/Mesh/MeshPS.hlsl"));

  Renderer3D::SetTopology(TopologyType::eDefaultMesh);

  Renderer3D::SetRasterizerState(RasterizerState::kDefault3D);
}

void SceneTessellation::Update()
{
  ImGui::Begin("SceneTessellation");
  {
    ConstantBufferData& Data = ConstantBuffer_.GetData();
    ImGui::SliderFloat3("TessFactor", &Data.TessFactor.x, kTessellationMinValue, kTessellationMaxValue);
    ImGui::SliderFloat("InsideFactor", &Data.InsideFactor, kTessellationMinValue, kTessellationMaxValue);
  }
  ImGui::End();

  FreeCamera(&Camera_);
  Renderer3D::SetCamera(Camera_);

  Renderer3D::Draw(Rectangle_, Matrix::Scale(2.5f));
}
