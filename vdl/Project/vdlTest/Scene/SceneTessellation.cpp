#include "SceneTessellation.hpp"

using namespace vdl;

void SceneTessellation::Initialize()
{
  ModelData ModelData;
  ModelData.Vertices.resize(4);
  {
    ModelData.Vertices[0] = { { +0.5f, +0.5f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { +1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } };
    ModelData.Vertices[1] = { { -0.5f, +0.5f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { +1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } };
    ModelData.Vertices[2] = { { +0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { +1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } };
    ModelData.Vertices[3] = { { -0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { +1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } };
  }
  ModelData.Indices = { 0, 1, 2, 3 };
  ModelData.MeshDatas.resize(1);
  {
    ModelData.MeshDatas[0].IndexCount = 4;
    ModelData.MeshDatas[0].Material.Diffuse = Palette::White;
  }

  Rectangle_ = ModelData;
  Camera_ = Camera(float3(0.0f, 0.0f, -5.0f));

  Renderer3D::SetShaders(VertexShader("Shader/Tessellation/TessellationVS.hlsl", InputLayoutType::eMesh),
    HullShader("Shader/Tessellation/TessellationHS.hlsl"),
    DomainShader("Shader/Tessellation/TessellationDS.hlsl"),
    PixelShader("Shader/Tessellation/TessellationPS.hlsl"));

  Renderer3D::SetTopology(TopologyType::ePatchList4ControlPoint);

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
