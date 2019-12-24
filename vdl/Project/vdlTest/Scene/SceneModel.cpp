#include "SceneModel.hpp"

using namespace vdl;

void SceneModel::Initialize()
{
  Model_ = Model("Data/DamagedHelmet/DamagedHelmet.gltf");
  Camera_ = Camera(vdl::float3(0.0f, 0.0f, -15.0f));
}

void SceneModel::Update()
{
  FreeCamera(&Camera_);

  GUI::Begin("Model");
  {
    const int DrawNum = DrawNum_;
    if (GUI::Input("DrawNum", reinterpret_cast<int*>(&DrawNum_)) && DrawNum_ < 0)
    {
      DrawNum_ = DrawNum;
    }

    if (GUI::TreeNode("Camera"))
    {
      GUI::Input("Position", &Camera_.Position);
      GUI::Input("Target", &Camera_.Target);
      GUI::Input("Up", &Camera_.Up);
      GUI::Input("Near", &Camera_.Near);
      GUI::Input("Far", &Camera_.Far);
      GUI::Input("Fov", reinterpret_cast<float*>(&Camera_.Fov));
      GUI::Checkbox("isPerspective", &Camera_.isPerspective);
      GUI::TreePop();
    }
  }
  GUI::End();

  Renderer3D::SetCamera(Camera_);

  constexpr Matrix Identity = Matrix::Identity();
  //  •`‰æ
  for (uint i = 0; i < DrawNum_; ++i)
  {
    Renderer3D::Draw(Model_, Identity);
  }
}
