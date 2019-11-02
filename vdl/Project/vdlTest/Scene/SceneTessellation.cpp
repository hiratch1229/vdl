#include "SceneTessellation.hpp"

using namespace vdl;

void SceneTessellation::Initialize()
{
  Rectangle_ = ModelData::Rectangle();
  Camera_ = Camera(vdl::float3(0.0f, 0.0f, -15.0f));

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

void SceneTessellation::Update()
{
  ImGui::Begin("SceneTessellation");
  {
    ConstantBufferData& Data = ConstantBuffer_.GetData();
    ImGui::SliderFloat3("TessFactor", &Data.TessFactor, kTessellationMinValue, kTessellationMaxValue);
    ImGui::SliderFloat("InsideFactor", &Data.InsideFactor, kTessellationMinValue, kTessellationMaxValue);
  }
  ImGui::End();

  FreeCamera(&Camera_);
  Renderer3D::SetCamera(Camera_);

  Renderer3D::Draw(Rectangle_, Matrix::Scale(2.5f));
}
