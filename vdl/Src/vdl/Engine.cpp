#include "Engine.hpp"

#include "Platform/IPlatform.hpp"
#include "System/ISystem.hpp"
#include "Window/IWindow.hpp"
#include "Device/IDevice.hpp"
#include "DeviceContext/IDeviceContext.hpp"
#include "SwapChain/ISwapChain.hpp"
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
#include <thread>

//#define USING_MULTI_THREAD

Engine::Engine()
{
  //  二度目以降作成時エラー
  assert(!pEngine);

  //  作成時にデータを取り出せるようにする
  pEngine = this;

#if defined USING_MULTI_THREAD
  std::thread DeviceThread = std::thread([&]()->void
  {
    pPlatform_->Initialize();
    pSystem_->Initialize();
    pDevice_->Initialize();
    pDeviceContext_->Initialize();
    pTextureManager_->Initialize();
    pBufferManager_->Initialize();
    pSoundManager_->Initialize();
  });

  pWindow_->Initialize();

  DeviceThread.join();

  std::thread SwapChainThread = std::thread([&]()->void
  {
    pSwapChain_->Initialize();
    pModelManager_->Initialize();
    pShaderManager_->Initialize();
    pCPUProfiler_->Initialize();
    pMemoryProfiler_->Initialize();
  });

  std::thread ProfilerThread = std::thread([&]()->void
  {
    pKeyboard_->Initialize();
    pMouse_->Initialize();
    pXInput_->Initialize();
    pGamepad_->Initialize();
    pRenderer_->Initialize();
    pComputer_->Initialize();
    pGUI_->Initialize();
  });

  SwapChainThread.join();
  ProfilerThread.join();
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
  pSwapChain_->Initialize();

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
  pSwapChain_.Release();
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
