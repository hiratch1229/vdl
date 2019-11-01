#include <vdl.hpp>

//#include "Scene/SceneSprite.hpp"
//#include "Scene/SceneModel.hpp"
//#include "Scene/SceneMultiRenderTexture.hpp"
#include "Scene/SceneTessellation.hpp"
#include "Scene/SceneGPUParticle.hpp"
#include "Scene/SceneDeferred.hpp"
#include "Scene/ScenePostEffect.hpp"
#include "Scene/SceneTBDR.hpp"

#include <memory>

namespace
{
  inline IScene* GetNextScene(vdl::uint i)
  {
    switch (i)
    {
    case 0:
      return new SceneTessellation;
    case 1:
      return new SceneGPUParticle;
    case 2:
      return new SceneDeferred;
    case 3:
      return new ScenePostEffect;
    case 4:
      return new SceneTBDR;
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
    "Tessellation",
    "GPUParticle",
    "Deferred",
    "PostEffect",
    "TBDR"
  };
  static constexpr vdl::uint kSceneTypeNum = static_cast<vdl::uint>(vdl::Macro::ArraySize(kSceneTypes));
  static constexpr vdl::uint kInitSceneType = 4;
  static_assert(kInitSceneType < kSceneTypeNum);

  vdl::uint SceneType = kInitSceneType;
  vdl::uint MaxFPS = vdl::Constants::kDefaultMaxFPS;

  std::unique_ptr<IScene> pCurrentScene(GetNextScene(SceneType));
  pCurrentScene->Initialize();

  while (vdl::System::Update())
  {
    ImGui::Begin("Information");
    ImGui::Text("FPS:%d(1 / %f)", vdl::System::GetFPS(), vdl::System::GetDeltaTime());
    ImGui::Text("CPU:%f%%", vdl::System::GetCPUUseRate());
    ImGui::Text("Memory:%f%%", vdl::System::GetMemoryUseRate());
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
        pCurrentScene->Initialize();
      }
    }

    ImGui::End();
  }
}
