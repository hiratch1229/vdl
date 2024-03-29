#include <vdl.hpp>

//#include "Scene/SceneSprite.hpp"
//#include "Scene/SceneModel.hpp"
//#include "Scene/SceneMultiRenderTexture.hpp"
//#include "Scene/SceneTessellation.hpp"
//#include "Scene/SceneGPUParticle.hpp"
//#include "Scene/SceneDeferred.hpp"
//#include "Scene/ScenePostEffect.hpp"
#include "Scene/SceneTBDR.hpp"
#include "Scene/SceneOcean.hpp"
#include "Scene/ScenePBR.hpp"

#include <memory>

//#define ENABLE_BENCHMARK

#ifdef ENABLE_BENCHMARK
#pragma comment (lib, "Shlwapi.lib")
#include <ThirdParty/benchmark/benchmark.h>

#define ITERATION_NUM 10
#define UPDATE_NUM 120
#endif

using namespace vdl;

namespace
{
  inline IScene* GetNextScene(uint i)
  {
    switch (i)
    {
    case 0:
      return new SceneTBDR;
    case 1:
      return new SceneOcean;
    case 2:
      return new ScenePBR;
    default: assert(false);
    }

    return nullptr;
  }

#ifdef ENABLE_BENCHMARK
  void SceneUpdate(benchmark::State& _State)
  {
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

    Renderer::SetTopology(TopologyType::eDefaultNone);
    Renderer2D::SetTopology(TopologyType::eDefaultTexture);
    Renderer2D::SetGraphicsStates(DefaultBlendState2D, DefaultDepthStencilState2D, DefaultRasterizerState2D);
    Renderer2D::SetShaders(DefaultVertexShader2D, DefaultPixelShader2D);
    Renderer3D::SetTopology(TopologyType::eDefaultMesh);
    Renderer3D::SetGraphicsStates(DefaultBlendState3D, DefaultDepthStencilState3D, DefaultRasterizerState3D);
    Renderer3D::SetStaticMeshShaders(DefaultVertexShader3D, DefaultPixelShader3D);

    std::unique_ptr<IScene> pCurrentScene(GetNextScene(static_cast<uint>(_State.range_x())));
    pCurrentScene->Initialize();

    for (auto _ : _State)
    {
      for (vdl::uint i = 0; i < UPDATE_NUM; ++i)
      {
        [[maybe_unused]] bool Result = System::Update();

        constexpr GUI::WindowFlags kBeginFlag = GUI::WindowFlags(GUI::WindowFlag::eNoMove) | GUI::WindowFlag::eNoResize;
        GUI::Begin("Information", nullptr, kBeginFlag);
        GUI::SetWindowPos(GUIHelper::kInformationWindowPos);
        GUI::SetWindowSize(GUIHelper::kInformationWindowSize);
        GUI::Text("FPS:%d(1 / %f)", System::GetFPS(), System::GetDeltaTime());
        GUI::Text("CPU:%f%%", System::GetCPUUseRate());
        GUI::Text("Memory:%f%%", System::GetMemoryUseRate());
        GUI::End();

        pCurrentScene->Update();
      }
    }
  }
  BENCHMARK(SceneUpdate)->Unit(benchmark::kMillisecond)->Iterations(ITERATION_NUM)->DenseRange(0, 2);
#endif // ENABLE_BENCHMARK
}

void Main()
{
#ifdef ENABLE_BENCHMARK
  benchmark::Initialize(nullptr, nullptr);

  [[maybe_unused]] bool Result = System::Update();

  System::SetMaxFPS(0);

  if (::benchmark::ReportUnrecognizedArguments(0, nullptr))
  {
    return;
  }

  benchmark::RunSpecifiedBenchmarks();
#else
  static constexpr const char* kSceneTypes[] = {
    //"Sprite",
    //"Model" ,
    //"MultiRenderTexture",
    //"Tessellation",
    //"GPUParticle",
    //"Deferred",
    //"PostEffect",
    "TBDR",
    "Ocean",
    "PBR"
  };
  static constexpr uint kSceneTypeNum = static_cast<uint>(Macro::ArraySize(kSceneTypes));
  static constexpr uint kInitSceneType = 1;
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

  uint SceneType = kInitSceneType;
  uint MaxFPS = Constants::kDefaultMaxFPS;

  std::unique_ptr<IScene> pCurrentScene(GetNextScene(SceneType));
  pCurrentScene->Initialize();

  constexpr GUI::WindowFlags kBeginFlag = GUI::WindowFlags(GUI::WindowFlag::eNoMove) | GUI::WindowFlag::eNoResize;

  while (System::Update())
  {
    GUI::Begin("Information", nullptr, kBeginFlag);
    GUI::SetWindowPos(GUIHelper::kInformationWindowPos);
    GUI::SetWindowSize(GUIHelper::kInformationWindowSize);
    GUI::Text("FPS:%d(1 / %f)", System::GetFPS(), System::GetDeltaTime());
    GUI::Text("CPU:%f%%", System::GetCPUUseRate());
    GUI::Text("Memory:%f%%", System::GetMemoryUseRate());
    if (GUI::Input("MaxFPS", &MaxFPS))
    {
      System::SetMaxFPS(MaxFPS);
    }

    //  シーンの更新
    {
      pCurrentScene->Update();

      IScene* pNextScene = nullptr;
      {
        if (GUI::Combo("Scene", reinterpret_cast<int*>(&SceneType), kSceneTypes, kSceneTypeNum))
        {
          pNextScene = GetNextScene(SceneType);
        }
      }

      if (pNextScene)
      {
        pCurrentScene.reset(pNextScene);

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

    GUI::End();
  }
#endif
}
