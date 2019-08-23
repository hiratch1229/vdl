#include "Engine.hpp"

#include "System/ISystem.hpp"
#include "Window/IWindow.hpp"
#include "Input/Keyboard/IKeyboard.hpp"
#include "Input/Mouse/IMouse.hpp"
#include "Input/XInput/IXInput.hpp"
#include "Input/Gamepad/IGamepad.hpp"
#include "SoundManager/ISoundManager.hpp"
#include "TextureManager/ITextureManager.hpp"
#include "ModelManager/IModelManager.hpp"
#include "Renderer/IRenderer.hpp"
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
  pEngine = nullptr;
}
