#include <vdl.hpp>

#include "Scene/SceneSprite.hpp"
#include "Scene/SceneModel.hpp"
#include "Scene/SceneMultiRenderTexture.hpp"
#include "Scene/SceneTessellation.hpp"
#include "Scene/SceneGPUParticle.hpp"
#define INIT_SCENE SceneSprite

#include <memory>

void Main()
{
  static constexpr const char* kSceneTypes[] = { "Sprite", "Model" ,"MultiRenderTexture", "Tessellation", "GPUParticle" };
  static constexpr vdl::uint kSceneTypeNum = static_cast<vdl::uint>(SceneType::eNum);
  static_assert(kSceneTypeNum == vdl::Macro::ArraySize(kSceneTypes));

  std::unique_ptr<IScene> pCurrentScene(new INIT_SCENE);
  pCurrentScene->Initialize();

  SceneType SceneType = pCurrentScene->GetType();
  vdl::uint MaxFPS = vdl::Constants::kDefaultMaxFPS;

  while (vdl::System::Update())
  {
    ImGui::Begin("Information");
    ImGui::Text("FPS:%d(1 / %f)", vdl::System::GetFPS(), vdl::System::GetDeltaTime());
    if (ImGui::InputInt("MaxFPS", reinterpret_cast<int*>(&MaxFPS)))
    {
      vdl::System::SetMaxFPS(MaxFPS);
    }

    //  �V�[���̍X�V
    {
      pCurrentScene->Update();

      IScene* pNextScene = nullptr;
      {
        if (ImGui::Combo("Scene", reinterpret_cast<int*>(&SceneType), kSceneTypes, kSceneTypeNum))
        {
          switch (SceneType)
          {
          case SceneType::eSprite:
            pNextScene = new SceneSprite;
            break;
          case SceneType::eModel:
            pNextScene = new SceneModel;
            break;
          case SceneType::eMultiRenderTexture:
            pNextScene = new SceneMultiRenderTexture;
            break;
          case SceneType::eTessellation:
            pNextScene = new SceneTessellation;
            break;
          case SceneType::eGPUParticle:
            pNextScene = new SceneGPUParticle;
            break;
          default: assert(false);
          }
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
