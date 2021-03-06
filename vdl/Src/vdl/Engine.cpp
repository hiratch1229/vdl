#include "Engine.hpp"

#include "Platform/IPlatform.hpp"
#include "System/ISystem.hpp"
#include "Window/IWindow.hpp"
#include "Device/IDevice.hpp"
#include "DeviceContext/IDeviceContext.hpp"
#include "CPUProfiler/ICPUProfiler.hpp"
#include "MemoryProfiler/IMemoryProfiler.hpp"
#include "Input/Keyboard/IKeyboard.hpp"
#include "Input/Mouse/IMouse.hpp"
#include "Input/XInput/IXInput.hpp"
#include "Input/Gamepad/IGamepad.hpp"
#include "SoundManager/ISoundManager.hpp"
#include "TextureManager/ITextureManager.hpp"
#include "ModelManager/IModelManager.hpp"
#include "BufferManager/IBufferManager.hpp"
#include "ShaderManager/IShaderManager.hpp"
#include "Renderer/IRenderer.hpp"
#include "Computer/IComputer.hpp"
#include "GUI/IGUI.hpp"

#include <vdl/DetectMemoryLeak.hpp>

#include <assert.h>

#define USING_MULTI_THREAD

Engine::Engine()
{
  //  二度目以降作成時エラー
  assert(!pEngine);

  //  作成時にデータを取り出せるようにする
  pEngine = this;

#if defined USING_MULTI_THREAD
  pPlatform_->Initialize();
  pSystem_->Initialize();
  pWindow_->Initialize();

  ThreadPool* ThreadPool = pSystem_->GetThreadPool();

  std::future<void> Device = ThreadPool->EnqueueWithAcquireResult([&]
  {
    pDevice_->Initialize();
    pTextureManager_->Initialize();
    pBufferManager_->Initialize();
    pShaderManager_->Initialize();
    pDeviceContext_->Initialize();
    pRenderer_->Initialize();
    pComputer_->Initialize();
    pGUI_->Initialize();
  });

  std::future<void> InputsAndGraphics = ThreadPool->EnqueueWithAcquireResult([&]
  {
    pKeyboard_->Initialize();
    pMouse_->Initialize();
    pXInput_->Initialize();
    pGamepad_->Initialize();
  });
  std::future<void> ManagersAndProfilers = ThreadPool->EnqueueWithAcquireResult([&]
  {
    pSoundManager_->Initialize();
    pModelManager_->Initialize();
    pCPUProfiler_->Initialize();
    pMemoryProfiler_->Initialize();
  });
  Device.get();
  InputsAndGraphics.get();
  ManagersAndProfilers.get();
#else
  pPlatform_->Initialize();
  pSystem_->Initialize();

  pWindow_->Initialize();
  pDevice_->Initialize();

  pSoundManager_->Initialize();
  pTextureManager_->Initialize();
  pModelManager_->Initialize();
  pBufferManager_->Initialize();
  pShaderManager_->Initialize();

  pDeviceContext_->Initialize();

  pCPUProfiler_->Initialize();
  pMemoryProfiler_->Initialize();

  pKeyboard_->Initialize();
  pMouse_->Initialize();
  pXInput_->Initialize();
  pGamepad_->Initialize();

  pRenderer_->Initialize();
  pComputer_->Initialize();
  pGUI_->Initialize();
#endif
}

Engine::~Engine()
{
  pDevice_->WaitIdle();

  pGUI_.Release();
  pComputer_.Release();
  pRenderer_.Release();
  pMemoryProfiler_.Release();
  pCPUProfiler_.Release();
  pDeviceContext_.Release();
  pShaderManager_.Release();
  pBufferManager_.Release();
  pModelManager_.Release();
  pTextureManager_.Release();
  pSoundManager_.Release();
  pGamepad_.Release();
  pXInput_.Release();
  pMouse_.Release();
  pKeyboard_.Release();
  pDevice_.Release();
  pWindow_.Release();
  pSystem_.Release();
  pPlatform_.Release();

  pEngine = nullptr;
}
