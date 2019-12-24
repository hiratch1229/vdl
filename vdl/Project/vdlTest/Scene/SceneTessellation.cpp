#include "SceneTessellation.hpp"

using namespace vdl;

void SceneTessellation::Initialize()
{
  StaticModelData ModelData;
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

  Renderer3D::SetStaticMeshShaders(VertexShader("Shader/Tessellation/TessellationVS.hlsl", InputLayoutType::eStaticMesh),
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
  GUI::Begin("SceneTessellation");
  {
    ConstantBufferData& Data = ConstantBuffer_.GetData();
    GUI::Slider("TessFactor", &Data.TessFactor, kTessellationMinValue, kTessellationMaxValue);
    GUI::Slider("InsideFactor", &Data.InsideFactor, kTessellationMinValue, kTessellationMaxValue);
  }
  GUI::End();

  FreeCamera(&Camera_);
  Renderer3D::SetCamera(Camera_);

  Renderer3D::Draw(Rectangle_, Matrix::Scale(2.5f));
}
