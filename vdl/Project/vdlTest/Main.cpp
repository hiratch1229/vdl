#include <vdl.hpp>

void Main()
{
  //vdl::Texture Texture("Data/SAOîwåi.png");
  vdl::float2 LeftTopPos = 0.0f;
  vdl::float2 DisplaySize = vdl::Window::GetWindowSize();
  vdl::Degree Angle = 0.0f;

  vdl::StaticMesh Model = vdl::StaticMeshData::Box();
  //vdl::Model Model("Data/danbo_atk.fbx");
  vdl::float3 Position = 0.0f;
  vdl::float3 Scale = 1.0f;
  int ModelNum = 1;

  vdl::Camera Camera(vdl::float3(0.0f, 0.0f, -10.0f));
  vdl::Renderer3D::SetRasterizerState(vdl::RasterizerState::kSolidCullFront);

  while (vdl::System::Update())
  {
    ImGui::Begin("Test");
    {
      ImGui::Text((std::string("FPS:") + std::to_string(vdl::System::GetFPS())).c_str());
      ImGui::Text((std::string("DeltaTime:") + std::to_string(vdl::System::GetDeltaTime())).c_str());
      ImGui::Text((std::string("MousePos:") + std::to_string(vdl::Input::Mouse::GetPos())).c_str());
      //ImGui::InputFloat2("LeftTopPos", &LeftTopPos.x);
      //ImGui::InputFloat2("DisplaySize", &DisplaySize.x);
      //ImGui::InputFloat("Angle", reinterpret_cast<float*>(&Angle));
      ImGui::InputFloat3("Position", &Position.x);
      ImGui::InputFloat3("Scale", &Scale.x);
      ImGui::InputInt("ModelNum", &ModelNum);
    }
    ImGui::End();

    vdl::FreeCamera(&Camera);
    vdl::Renderer3D::SetCamera(Camera);

    //vdl::RendererTexture::Draw(Texture, LeftTopPos, DisplaySize, Angle);
    for (int i = 0; i < ModelNum; ++i)
    {
      vdl::Renderer3D::Draw(Model, vdl::Matrix::Scale(Scale) * vdl::Matrix::Translate(Position));
    }
  }
}
