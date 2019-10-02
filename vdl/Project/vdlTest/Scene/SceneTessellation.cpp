#include "SceneTessellation.hpp"

using namespace vdl;

void SceneTessellation::Initialize()
{
  Rectangle_ = StaticMeshData::Rectangle();

  RendererStaticMesh::SetShaders(VertexShader("Shader/Tessellation/TessellationVS.hlsl", InputLayout::eStaticMesh),
    HullShader("Shader/Tessellation/TessellationHS.hlsl"),
    DomainShader("Shader/Tessellation/TessellationDS.hlsl"),
    PixelShader("Shader/Tessellation/TessellationPS.hlsl"));

  RendererStaticMesh::SetTopology(Topology::ePatchList3ControlPoint);

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
  RendererStaticMesh::SetShaders(VertexShader(Constants::kDefaultStaticMeshVertexShaderCode, static_cast<uint>(Macro::ArraySize(Constants::kDefaultStaticMeshVertexShaderCode)), InputLayout::eStaticMesh),
    PixelShader(Constants::kDefaultStaticMeshPixelShaderCode, static_cast<uint>(Macro::ArraySize(Constants::kDefaultStaticMeshPixelShaderCode))));

  RendererStaticMesh::SetTopology(Topology::eDefaultStaticMesh);

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

  Renderer3D::Draw(Rectangle_, Matrix::Scale(2.5f));
}
