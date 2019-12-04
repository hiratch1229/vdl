#include <vdl.hpp>

//#include "Scene/SceneSprite.hpp"
//#include "Scene/SceneModel.hpp"
//#include "Scene/SceneMultiRenderTexture.hpp"
//#include "Scene/SceneTessellation.hpp"
//#include "Scene/SceneGPUParticle.hpp"
#include "Scene/SceneDeferred.hpp"
#include "Scene/ScenePostEffect.hpp"
#include "Scene/SceneTBDR.hpp"
#include "Scene/SceneOcean.hpp"
#include "Scene/ScenePBR.hpp"

#include <memory>

using namespace vdl;

namespace
{
  inline IScene* GetNextScene(uint i)
  {
    switch (i)
    {
    case 0:
      return new SceneDeferred;
    case 1:
      return new ScenePostEffect;
    case 2:
      return new SceneTBDR;
    case 3:
      return new SceneOcean;
    case 4:
      return new ScenePBR;
    default: assert(false);
    }

    return nullptr;
  }
}

void Main()
{
  static constexpr const char* kSceneTypes[] = {
    //"Sprite",
    //"Model" ,
    //"MultiRenderTexture",
    //"Tessellation",
    //"GPUParticle",
    "Deferred",
    "PostEffect",
    "TBDR",
    "Ocean",
    "PBR"
  };
  static constexpr uint kSceneTypeNum = static_cast<uint>(Macro::ArraySize(kSceneTypes));
  static constexpr uint kInitSceneType = 4;
  static_assert(kInitSceneType < kSceneTypeNum);

  const VertexShader DefaultVertexShader2D = VertexShader("Shader/Texture/TextureVS.hlsl", InputLayoutType::eTexture);
  const PixelShader DefaultPixelShader2D = PixelShader("Shader/Texture/TexturePS.hlsl");
  const BlendState DefaultBlendState2D = BlendState::kDefault;
  const DepthStencilState DefaultDepthStencilState2D = DepthStencilState::kDefault2D;
  const RasterizerState DefaultRasterizerState2D = RasterizerState::kDefault2D;
  const VertexShader DefaultVertexShader3D = VertexShader("Shader/Mesh/StaticMesh/StaticMeshVS.hlsl", InputLayoutType::eStaticMesh);
  const PixelShader DefaultPixelShader3D = PixelShader("Shader/Mesh/MeshPS.hlsl");
  const BlendState DefaultBlendState3D = BlendState::kDefault;
  const DepthStencilState DefaultDepthStencilState3D = DepthStencilState::kDefault3D;
  const RasterizerState DefaultRasterizerState3D = RasterizerState::kDefault3D;
  const RenderTexture SwapChainRenderTexture = Window::GetRenderTexture();
  const DepthStencilTexture SwapChainDepthStencilTexture = Window::GetDepthStencilTexture();

  uint SceneType = kInitSceneType;
  uint MaxFPS = vdl::Constants::kDefaultMaxFPS;

  std::unique_ptr<IScene> pCurrentScene(GetNextScene(SceneType));
  pCurrentScene->Initialize();

  while (vdl::System::Update())
  {
    ImGui::Begin("Information", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
    ImGui::SetWindowPos(ImGuiHelper::kInformationWindowPos);
    ImGui::SetWindowSize(ImGuiHelper::kInformationWindowSize);
    ImGui::Text("FPS:%d(1 / %f)", System::GetFPS(), System::GetDeltaTime());
    ImGui::Text("CPU:%f%%", System::GetCPUUseRate());
    ImGui::Text("Memory:%f%%", System::GetMemoryUseRate());
    if (ImGui::InputInt("MaxFPS", reinterpret_cast<int*>(&MaxFPS)))
    {
      vdl::System::SetMaxFPS(MaxFPS);
    }

    //  シーンの更新
    {
      pCurrentScene->Update();

      IScene* pNextScene = nullptr;
      {
        if (ImGui::Combo("Scene", reinterpret_cast<int*>(&SceneType), kSceneTypes, kSceneTypeNum))
        {
          pNextScene = GetNextScene(SceneType);
        }
      }

      if (pNextScene)
      {
        pCurrentScene.reset(pNextScene);

        Renderer::SetRenderTexture(SwapChainRenderTexture, SwapChainDepthStencilTexture);
        Renderer::SetTopology(TopologyType::eDefaultNone);
        Renderer2D::SetTopology(TopologyType::eDefaultTexture);
        Renderer2D::SetGraphicsStates(DefaultBlendState2D, DefaultDepthStencilState2D, DefaultRasterizerState2D);
        Renderer2D::SetShaders(DefaultVertexShader2D, DefaultPixelShader2D);
        Renderer3D::SetTopology(TopologyType::eDefaultMesh);
        Renderer3D::SetGraphicsStates(DefaultBlendState3D, DefaultDepthStencilState3D, DefaultRasterizerState3D);
        Renderer3D::SetStaticMeshShaders(DefaultVertexShader3D, DefaultPixelShader3D);

        pCurrentScene->Initialize();
      }
    }

    ImGui::End();
  }
}
