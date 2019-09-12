#pragma once
#include "../ISoundManager.hpp"

#include <vdl/ReferenceCount/ReferenceCount.hpp>

#include <Windows.h>
#include <wrl.h>
#include <mfplay.h>

class CSoundManager : public ISoundManager
{
  struct Data
  {
    Microsoft::WRL::ComPtr<IMFPMediaPlayer> pMediaPlayer;
    bool isLoop = false;
  };
private:
  HWND hWnd_;
  ReferenceCounts<Data> Datas_;
public:
  CSoundManager() = default;

  void Initialize()override;

  vdl::ID Load(const char* _FilePath)override;

  void AddRef(const vdl::ID& _ID)override { Datas_.Get(_ID).AddRef(); }

  void Release(const vdl::ID& _ID)override { Datas_.Get(_ID).Release(); }
};
