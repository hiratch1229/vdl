#include "ScenePBR.hpp"

using namespace vdl;

void ScenePBR::Initialize()
{
  Camera_ = Camera(float3(0.0f, 10.0f, -50.0f), float3(0.0f, 5.0f, 0.0f));
  PBRVertexShader_ = VertexShader("Shader/PBR/PBRVS.hlsl", InputLayoutType::eStaticMesh);
  PBRPixelShader_ = PixelShader("Shader/PBR/PBRPS.hlsl");

  DirectinalLight& DirectinalLight = LightConstantData_.GetData();
  {
    DirectinalLight.Direction = float3(0.0f, -0.5f, 1.0f).Normalize();
    DirectinalLight.Itensity = 3.5f;
    DirectinalLight.Color = Palette::White;
  }
  Renderer3D::SetPixelStageConstantBuffers(0, 1, &LightConstantData_);
  MaterialData& MaterialData = MaterialDataConstantData_.GetData();
  {
    MaterialData.Reflectance = 1.0f;
    MaterialData.CameraPosition = Camera_.Position;
    MaterialData.Ambient = Palette::DimGray;
  }
  Renderer3D::SetPixelStageConstantBuffers(1, 1, &MaterialDataConstantData_);

  //  PBRモデルのロード
  {
    Texture DefaultAmbientOcclussion = Texture(Palette::White);
    Texture DefaultEmissive = Texture(Palette::Black);

    //AnimatedCube_ = Model("Data/PBR/AnimatedCube/AnimatedCube.gltf");
    //AnimatedMaterials_.MetallicRoughness = Texture("Data/PBR/AnimatedCube/AnimatedCube_MetallicRoughness.png");
    //AnimatedMaterials_.AmbientOcclusion = DefaultAmbientOcclussion;
    //AnimatedMaterials_.Emissive = DefaultEmissive;
    //AnimatedCubeWorld_ = Matrix::Scale(kAnimationCubeScale) * Matrix::Translate(kAnimationCubePosition);
    BoomBox_ = Model("Data/PBR/BoomBox/BoomBox.gltf");
    BoomBoxMaterials_.MetallicRoughness = Texture("Data/PBR/BoomBox/BoomBox_occlusionRoughnessMetallic.png");
    BoomBoxMaterials_.AmbientOcclusion = BoomBoxMaterials_.MetallicRoughness;
    BoomBoxMaterials_.Emissive = Texture("Data/PBR/BoomBox/BoomBox_emissive.png");
    BoomBoxWorld_ = Matrix::Scale(kBoomBoxScale) * Matrix::Translate(kBoomBoxPosition);
    DamagedHelmet_ = Model("Data/PBR/DamagedHelmet/DamagedHelmet.gltf");
    DamagedHelmetMaterials_.MetallicRoughness = Texture("Data/PBR/DamagedHelmet/Default_metalRoughness.jpg");
    DamagedHelmetMaterials_.AmbientOcclusion = Texture("Data/PBR/DamagedHelmet/Default_AO.jpg");
    DamagedHelmetMaterials_.Emissive = Texture("Data/PBR/DamagedHelmet/Default_emissive.jpg");
    DamagedHelmetWorld_ = Matrix::Scale(kDamagedHelmetScale) * Matrix::Translate(kDamagedHelmetPosition);
    SciFiHelmet_ = Model("Data/PBR/SciFiHelmet/SciFiHelmet.gltf");
    SciFiHelmetMaterials_.MetallicRoughness = Texture("Data/PBR/SciFiHelmet/SciFiHelmet_MetallicRoughness.png");
    SciFiHelmetMaterials_.AmbientOcclusion = Texture("Data/PBR/SciFiHelmet/SciFiHelmet_AmbientOcclusion.png");
    SciFiHelmetMaterials_.Emissive = DefaultEmissive;
    SciFiHelmetWorld_ = Matrix::Scale(kSciFiHelmetScale) * Matrix::Translate(kSciFiHelmetPosition);
    Suzanne_ = Model("Data/PBR/Suzanne/Suzanne.gltf");
    SuzanneMaterials_.MetallicRoughness = Texture("Data/PBR/Suzanne/Suzanne_MetallicRoughness.png");
    SuzanneMaterials_.AmbientOcclusion = DefaultAmbientOcclussion;
    SuzanneMaterials_.Emissive = DefaultEmissive;
    SuzanneWorld_ = Matrix::Scale(kSuzanneScale) * Matrix::Translate(kSuzannePosition);
    TwoSidedPlane_ = Model("Data/PBR/TwoSidedPlane/TwoSidedPlane.gltf");
    TwoSidedPlaneMaterials_.MetallicRoughness = Texture("Data/PBR/TwoSidedPlane/TwoSidedPlane_MetallicRoughness.png");
    TwoSidedPlaneMaterials_.AmbientOcclusion = DefaultAmbientOcclussion;
    TwoSidedPlaneMaterials_.Emissive = DefaultEmissive;
    TwoSidedPlaneWorld_ = Matrix::Scale(kTwoSidedPlaneScale) * Matrix::Translate(kTwoSidedPlanePosition);
  }

  //  空のデータをロード
  {
    Sphere_ = StaticModelData::Sphere(20, 20);
    SkyboxTexture_ = CubeTexture("Data/Skybox/darkskies.png");
    SkyboxVertexShader_ = VertexShader("Shader/Skybox/SkyboxVS.hlsl", InputLayoutType::eStaticMesh);
    SkyboxPixelShader_ = PixelShader("Shader/Skybox/SkyboxPS.hlsl");
  }
}

void ScenePBR::Update()
{
  FreeCamera(&Camera_);
  Renderer3D::SetCamera(Camera_);
  MaterialData& MaterialData = MaterialDataConstantData_.GetData();
  MaterialData.CameraPosition = Camera_.Position;

  ImGui::Begin("ScenePBR", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
  {
    ImGui::SetWindowPos(ImGuiHelper::kSceneWindowPos);
    ImGui::SetWindowSize(kSceneWindowSize);

    if (ImGui::TreeNode("DirectinalLight"))
    {
      DirectinalLight& DirectionalLight = LightConstantData_.GetData();
      ImGui::SliderFloat3("Direction", &DirectionalLight.Direction, -1.0f, 1.0f);
      ImGui::DragFloat("Itensity", &DirectionalLight.Itensity, 0.01f);
      ImGui::ColorEdit3("Color", &DirectionalLight.Color);

      ImGui::TreePop();
    }
    if (ImGui::TreeNode("MaterialData"))
    {
      ImGui::SliderFloat("Reflectance", &MaterialData.Reflectance, 0.0f, 1.0f);
      ImGui::ColorEdit3("Ambient", &MaterialData.Ambient);

      ImGui::TreePop();
    }
  }
  ImGui::End();

  //  PBRの描画
  {
    Renderer3D::SetRasterizerState(RasterizerState::kSolidCullBack);

    auto SetMaterials = [](const Materials& _Materials)->void
    {
      Renderer3D::SetPixelStageShaderResources(2, 1, &_Materials.MetallicRoughness);
      Renderer3D::SetPixelStageShaderResources(3, 1, &_Materials.AmbientOcclusion);
      Renderer3D::SetPixelStageShaderResources(4, 1, &_Materials.Emissive);
    };

    Renderer3D::SetStaticMeshShaders(PBRVertexShader_, PBRPixelShader_);

    SetMaterials(BoomBoxMaterials_);
    Renderer3D::Draw(BoomBox_, BoomBoxWorld_);

    //SetMaterials(AnimatedMaterials_);
    //Renderer3D::Draw(AnimatedCube_, AnimatedCubeWorld_);

    SetMaterials(DamagedHelmetMaterials_);
    Renderer3D::Draw(DamagedHelmet_, DamagedHelmetWorld_);

    SetMaterials(SciFiHelmetMaterials_);
    Renderer3D::Draw(SciFiHelmet_, SciFiHelmetWorld_);

    SetMaterials(SuzanneMaterials_);
    Renderer3D::Draw(Suzanne_, SuzanneWorld_);

    SetMaterials(TwoSidedPlaneMaterials_);
    Renderer3D::Draw(TwoSidedPlane_, TwoSidedPlaneWorld_);
  }

  //  空の描画
  {
    Renderer3D::SetPixelStageShaderResources(2, 1, &SkyboxTexture_);
    Renderer3D::SetStaticMeshShaders(SkyboxVertexShader_, SkyboxPixelShader_);
    Renderer3D::SetRasterizerState(RasterizerState::kSolidCullFront);

    Renderer3D::Draw(Sphere_, Matrix::Scale(kSkyboxScale));
  }
}
