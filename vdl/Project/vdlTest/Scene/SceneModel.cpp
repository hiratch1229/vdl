#include "SceneModel.hpp"

using namespace vdl;

void SceneModel::Initialize()
{
  Model_ = Model("Data/DamagedHelmet/glTF/DamagedHelmet.gltf");
  Camera_ = Camera(vdl::float3(0.0f, 0.0f, -15.0f));
}

void SceneModel::Update()
{
  FreeCamera(&Camera_);

  ImGui::Begin("Model");
  {
    const int DrawNum = DrawNum_;
    if (ImGui::InputInt("DrawNum", reinterpret_cast<int*>(&DrawNum_)) && DrawNum_ < 0)
    {
      DrawNum_ = DrawNum;
    }
    ImGui::SetNextTreeNodeOpen(false, ImGuiCond_Once);
    if (ImGui::TreeNode("Camera"))
    {
      ImGui::InputFloat3("Position", &Camera_.Position.x);
      ImGui::InputFloat3("Target", &Camera_.Target.x);
      ImGui::InputFloat3("Up", &Camera_.Up.x);
      ImGui::InputFloat("Near", &Camera_.Near);
      ImGui::InputFloat("Far", &Camera_.Far);
      ImGui::InputFloat("Fov", reinterpret_cast<float*>(&Camera_.Fov));
      ImGui::Checkbox("isPerspective", &Camera_.isPerspective);
      ImGui::TreePop();
    }
  }
  ImGui::End();

  Renderer3D::SetCamera(Camera_);

  constexpr Matrix Identity = Matrix::Identity();
  //  •`‰æ
  for (uint i = 0; i < DrawNum_; ++i)
  {
    Renderer3D::Draw(Model_, Identity);
  }
}
