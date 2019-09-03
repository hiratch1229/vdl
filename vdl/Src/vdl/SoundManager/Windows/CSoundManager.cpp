#include "CSoundManager.hpp"

#include <vdl/Engine.hpp>
#include <vdl/Window/IWindow.hpp>

#include <vdl/Constants/Constants.hpp>
#include <vdl/Misc/Windows/Misc.hpp>

void CSoundManager::Initialize()
{
  hWnd_ = static_cast<HWND>(Engine::Get<IWindow>()->GetHandle());
}

ID CSoundManager::Load(const char* _FilePath)
{
  //  �G���[�`�F�b�N�p
  HRESULT hr = S_OK;

  wchar_t wFilePath[Constants::kMaxCharacterNum]{};
  ::mbstowcs_s(nullptr, wFilePath, _FilePath, Constants::kMaxCharacterNum);

  Data* pData = new Data;
  {
    hr = ::MFPCreateMediaPlayer(wFilePath, false, MFP_OPTION_NONE, nullptr, hWnd_, pData->pMediaPlayer.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
  }

  return Datas_.Add(pData);
}