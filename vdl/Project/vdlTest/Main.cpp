#include <vdl.hpp>

#include "Scene/SceneMultiRenderTexture.hpp"
#define INIT_SCENE SceneMultiRenderTexture

#include <memory>

void Main()
{
  static constexpr const char* kSceneTypes[] = { "MultiRenderTexture" };
  static constexpr vdl::uint kSceneTypeNum = static_cast<vdl::uint>(SceneType::eNum);
  static_assert(kSceneTypeNum == vdl::Macro::ArraySize(kSceneTypes));

  std::unique_ptr<IScene> pCurrentScene(new INIT_SCENE);
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
          case SceneType::eMultiRenderTexture:
            pNextScene = new SceneMultiRenderTexture;
            break;
          default: assert(false);
          }
        }
      }

      if (pNextScene)
      {
        pCurrentScene.reset(pNextScene);
      }
    }

    ImGui::End();
  }
}
