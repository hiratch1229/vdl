#include "Engine.hpp"

#include "System/ISystem.hpp"
#include "Window/IWindow.hpp"
#include "Device/IDevice.hpp"
#include "DeviceContext/IDeviceContext.hpp"
#include "SwapChain/ISwapChain.hpp"
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

#include <assert.h>

Engine::Engine()
{
  //  ��x�ڈȍ~�쐬���G���[
  assert(!pEngine);

  //  �쐬���Ƀf�[�^�����o����悤�ɂ���
  pEngine = this;

  pSystem_->Initialize();
}

Engine::~Engine()
{
  pGUI_.Release();
  pComputer_.Release();
  pRenderer_.Release();
  pShaderManager_.Release();
  pBufferManager_.Release();
  pModelManager_.Release();
  pTextureManager_.Release();
  pSoundManager_.Release();
  pGamepad_.Release();
  pXInput_.Release();
  pMouse_.Release();
  pKeyboard_.Release();
  pSwapChain_.Release();
  pDeviceContext_.Release();
  pDevice_.Release();
  pWindow_.Release();
  pSystem_.Release();

  pEngine = nullptr;
}
