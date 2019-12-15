#include "CSoundManager.hpp"

#include <vdl/Engine.hpp>
#include <vdl/Window/IWindow.hpp>

#include <vdl/Constants/Constants.hpp>
#include <vdl/Misc/Windows/Misc.hpp>

#include <vdl/DetectMemoryLeak.hpp>

void CSoundManager::Initialize()
{
  hWnd_ = static_cast<HWND>(Engine::Get<IWindow>()->GetHandle());
}

vdl::ID CSoundManager::Load(const char* _FilePath)
{
  //  エラーチェック用
  HRESULT hr = S_OK;

  wchar_t wFilePath[Constants::kMaxCharacterNum]{};
  ::mbstowcs_s(nullptr, wFilePath, _FilePath, Constants::kMaxCharacterNum);

  Data* pData = new Data;
  {
    hr = ::MFPCreateMediaPlayer(wFilePath, false, MFP_OPTION_NONE, nullptr, hWnd_, pData->pMediaPlayer.GetAddressOf());
    ERROR_CHECK(hr);
  }

  return Datas_.Add(pData);
}
