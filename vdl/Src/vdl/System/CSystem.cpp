#include "CSystem.hpp"

#include <vdl/Engine.hpp>
#include <vdl/Window/IWindow.hpp>
#include <vdl/Device/IDevice.hpp>
#include <vdl/DeviceContext/IDeviceContext.hpp>
#include <vdl/SwapChain/ISwapChain.hpp>
#include <vdl/Input/Keyboard/IKeyboard.hpp>
#include <vdl/Input/Mouse/IMouse.hpp>
#include <vdl/Input/XInput/IXInput.hpp>
#include <vdl/Input/Gamepad/IGamepad.hpp>
#include <vdl/SoundManager/ISoundManager.hpp>
#include <vdl/TextureManager/ITextureManager.hpp>
#include <vdl/ModelManager/IModelManager.hpp>
#include <vdl/BufferManager/IBufferManager.hpp>
#include <vdl/Renderer/IRenderer.hpp>
#include <vdl/GUI/IGUI.hpp>

#include <vdl/System.hpp>

void CSystem::Initialize()
{
  pSwapChain_ = Engine::Get<ISwapChain>();
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

  pKeyboard_->Initialize();
  pMouse_->Initialize();
  pXInput_->Initialize();
  pGamepad_->Initialize();

  Engine::Get<ISoundManager>()->Initialize();
  Engine::Get<ITextureManager>()->Initialize();
  Engine::Get<IModelManager>()->Initialize();
  Engine::Get<IBufferManager>()->Initialize();

  pRenderer_->Initialize();
  pGUI_->Initialize();

  //  周波数を取得
  {
    LARGE_INTEGER Frequency;
    ::QueryPerformanceFrequency(&Frequency);
    assert(Frequency.QuadPart != 0);

    //  周波数ごとの時間を算出(ms)
    MilliSecondsPerFrequency_ = 1000.0 / static_cast<double>(Frequency.QuadPart);
  }

  SystemState_ = SystemState::eInitialized;
}

bool CSystem::Update()
{
  if (SystemState_ == SystemState::eInitialized)
  {
    Engine::Get<IWindow>()->Show(true);

    SystemState_ = SystemState::eRunning;
  }

  pRenderer_->Flush();

  pGUI_->Draw();

  pSwapChain_->Present();

  //--------------------------------------------------
  //  ここから次のフレーム
  //--------------------------------------------------

  //  デフォルトの機能の処理
  {
    //  有効なもの以外を削除
    DefaultActionFlags_ &= ValidDefaultActions_;

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
    double CurrentTime;
    double DeltaTime;
    double SleepTime;

    do
    {
      CurrentTime = GetMicroSecond();
      DeltaTime = CurrentTime - LastTime_;
      SleepTime = FrameInterval_ - DeltaTime;

      if (SleepTime > 0.0)
      {
        //  精度を1msにする
        ::timeBeginPeriod(1);
        //  停止
        ::Sleep(static_cast<DWORD>(SleepTime));
        //  精度を元に戻す
        ::timeEndPeriod(1);
      }
    } while (SleepTime > 0.5 || SystemState_ == SystemState::ePause);

    LastTime_ = CurrentTime;
    DeltaTime_ = DeltaTime;
  }

  //  入力系の更新
  {
    pKeyboard_->Update();
    pMouse_->Update();
    pXInput_->Update();
    pGamepad_->Update();
  }

  return true;
}

//--------------------------------------------------

double CSystem::GetMicroSecond()const
{
  LARGE_INTEGER Counter;
  ::QueryPerformanceCounter(&Counter);

  return Counter.QuadPart * MilliSecondsPerFrequency_;
}
