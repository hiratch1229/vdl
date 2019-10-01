#include <vdl.hpp>

void Main()
{
  vdl::Model Model("Data/danbo_atk.fbx");
  vdl::float3 Position = 0.0f;
  vdl::float3 Scale = 1.0f;
  vdl::Camera Camera(vdl::float3(0.0f, 0.0f, -10.0f));

  vdl::RenderTextures RenderTextures = { vdl::RenderTexture(vdl::Window::GetWindowSize(), vdl::Format::eR8G8B8A8_Unorm), vdl::RenderTexture(vdl::Window::GetWindowSize(), vdl::Format::eR8G8B8A8_Unorm) };

  //vdl::RenderTexture RenderTexture(vdl::Window::GetWindowSize(), vdl::Format::eR8G8B8A8_Unorm);

  vdl::RendererStaticMesh::SetPixelShader(vdl::PixelShader("Shader/StaticMesh/StaticMeshPS.hlsl"));

  constexpr vdl::uint kUsingRenderTextureNum = 2;

  while (vdl::System::Update())
  {
    vdl::FreeCamera(&Camera);
    vdl::Renderer3D::SetCamera(Camera);

    ImGui::Begin("Test");
    {
      ImGui::Text((std::string("FPS:") + std::to_string(vdl::System::GetFPS())).c_str());
      ImGui::Text((std::string("DeltaTime:") + std::to_string(vdl::System::GetDeltaTime())).c_str());
      ImGui::Text((std::string("MousePos:") + std::to_string(vdl::Input::Mouse::GetPos())).c_str());
      ImGui::Text((std::string("MouseDelta:") + std::to_string(vdl::Input::Mouse::GetDelta())).c_str());
    }
    ImGui::End();

    for (vdl::uint i = 0; i < kUsingRenderTextureNum; ++i)
    {
      vdl::Renderer::Clear(RenderTextures[i]);
    }
    //vdl::Renderer::SetRenderTexture(RenderTexture, vdl::DepthStencilTexture());

    vdl::Renderer::SetRenderTextures(RenderTextures, vdl::DepthStencilTexture());
    vdl::Renderer3D::Draw(Model, vdl::Matrix::Scale(Scale) * vdl::Matrix::Translate(Position));
    vdl::Renderer::SetRenderTexture(vdl::RenderTexture(), vdl::DepthStencilTexture());

    for (vdl::uint i = 0; i < kUsingRenderTextureNum; ++i)
    {
      vdl::Renderer2D::Draw(RenderTextures[i], vdl::float2((i % 2) * (1280.0f / kUsingRenderTextureNum), 0.0f), vdl::float2(640.0f, 720.0f));
    }
  }
}
