#include "CSystem.hpp"

#include <vdl/Engine.hpp>
#include <vdl/Window/IWindow.hpp>
#include <vdl/Device/IDevice.hpp>
#include <vdl/DeviceContext/IDeviceContext.hpp>
#include <vdl/SwapChain/ISwapChain.hpp>
#include <vdl/CPUProfiler/ICPUProfiler.hpp>
#include <vdl/MemoryProfiler/IMemoryProfiler.hpp>
#include <vdl/Input/Keyboard/IKeyboard.hpp>
#include <vdl/Input/Mouse/IMouse.hpp>
#include <vdl/Input/XInput/IXInput.hpp>
#include <vdl/Input/Gamepad/IGamepad.hpp>
#include <vdl/SoundManager/ISoundManager.hpp>
#include <vdl/TextureManager/ITextureManager.hpp>
#include <vdl/ModelManager/IModelManager.hpp>
#include <vdl/BufferManager/IBufferManager.hpp>
#include <Vdl/ShaderManager/IShaderManager.hpp>
#include <vdl/Renderer/IRenderer.hpp>
#include <vdl/Computer/IComputer.hpp>
#include <vdl/GUI/IGUI.hpp>
#include <vdl/Constants/Constants.hpp>

#include <vdl/System.hpp>

#include <vdl/Viewport.hpp>
#include <vdl/Scissor.hpp>

#include <thread>
#include <Windows.h>

void CSystem::Initialize()
{
  pSwapChain_ = Engine::Get<ISwapChain>();
  pCPUProfiler_ = Engine::Get<ICPUProfiler>();
  pMemoryProfiler_ = Engine::Get<IMemoryProfiler>();
  pKeyboard_ = Engine::Get<IKeyboard>();
  pMouse_ = Engine::Get<IMouse>();
  pXInput_ = Engine::Get<IXInput>();
  pGamepad_ = Engine::Get<IGamepad>();
  pRenderer_ = Engine::Get<IRenderer>();
  pGUI_ = Engine::Get<IGUI>();

  Engine::Get<IWindow>()->Initialize();
  Engine::Get<IDevice>()->Initialize();
  Engine::Get<IDeviceContext>()->Initialize();
  pSwapChain_->Initialize();

  pCPUProfiler_->Initialize();

  pKeyboard_->Initialize();
  pMouse_->Initialize();
  pXInput_->Initialize();
  pGamepad_->Initialize();

  Engine::Get<ISoundManager>()->Initialize();
  Engine::Get<ITextureManager>()->Initialize();
  Engine::Get<IModelManager>()->Initialize();
  Engine::Get<IBufferManager>()->Initialize();
  Engine::Get<IShaderManager>()->Initialize();

  pRenderer_->Initialize();
  Engine::Get<IComputer>()->Initialize();
  pGUI_->Initialize();

  //  フレームレートを設定
  SetMaxFPS(Constants::kDefaultMaxFPS);

  SystemState_ = SystemState::eInitialized;
}

bool CSystem::Update()
{
  if (SystemState_ == SystemState::eInitialized)
  {
    IWindow* pWindow = Engine::Get<IWindow>();
    pWindow->Show(true);

    const vdl::uint2& WindowSize = pWindow->GetWindowSize();
    const vdl::Viewport Viewport(0.0f, WindowSize);
    const vdl::Scissor Scissor(0, WindowSize);

    pRenderer_->SetViewport(Viewport, vdl::InputLayoutType::eNone);
    pRenderer_->SetScissor(Scissor, vdl::InputLayoutType::eNone);
    pRenderer_->SetViewport(Viewport, vdl::InputLayoutType::eTexture);
    pRenderer_->SetScissor(Scissor, vdl::InputLayoutType::eTexture);
    pRenderer_->SetViewport(Viewport, vdl::InputLayoutType::eMesh);
    pRenderer_->SetScissor(Scissor, vdl::InputLayoutType::eMesh);

    SystemState_ = SystemState::eRunning;
    LastTime_ = std::chrono::high_resolution_clock::now();
  }
  else
  {
    pRenderer_->Flush();

    pGUI_->Draw();

    pSwapChain_->Present();
  }

  //--------------------------------------------------
  //  ここから次のフレーム
  //--------------------------------------------------

  //  デフォルトの機能の処理
  {
    if (DefaultActionFlags_ & vdl::System::DefalutAction::eExit)
    {
      return false;
    }
    if (DefaultActionFlags_ & vdl::System::DefalutAction::eChangeWindowMode)
    {
      pSwapChain_->ChangeWindowMode();
    }
    if (DefaultActionFlags_ & vdl::System::DefalutAction::eScreenShot)
    {
      pSwapChain_->ScreenShot();
    }

    //  状態のクリア
    DefaultActionFlags_ = 0;
  }

  pSwapChain_->ScreenClear();

  //  FPS制御
  {
    ++Frames_;

    std::chrono::high_resolution_clock::time_point CurrentTime;
    Duration DeltaTime;
    Duration SleepTime;

    while (true)
    {
      CurrentTime = std::chrono::high_resolution_clock::now();
      DeltaTime = std::chrono::duration(CurrentTime - LastTime_);
      SleepTime = FrameInterval_ - DeltaTime;

      if (SleepTime.count() > 0.0 || SystemState_ == SystemState::ePause)
      {
        std::this_thread::yield();
        continue;
      }

      break;
    }

    LastTime_ = CurrentTime;
    DeltaTime_ = DeltaTime;

    if ((TimeStamp_ += DeltaTime).count() >= 1.0)
    {
      CurrentFPS_ = Frames_;
      Frames_ = 0;
      TimeStamp_ = Duration(0.0);

      CurrentCPUUseRate_ = pCPUProfiler_->GetUseRate();
      CurrentMemoryUseRate_ = pMemoryProfiler_->GetUseRate();
    }
  }

  //  入力系の更新
  {
    pKeyboard_->Update();/*
    */pMouse_->Update();
    pXInput_->Update();/*
    */pGamepad_->Update();
  }

  pGUI_->Update();

  return true;
}
