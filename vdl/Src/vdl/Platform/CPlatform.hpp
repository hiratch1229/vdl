#pragma once
#include "IPlatform.hpp"

class CPlatform : public IPlatform
{
  PlatformFlags Flags_;
private:
  //  OS�̃\�[�X�R�[�h�Ŏ���
  PlatformFlags GetOperationSystem()const;
  //  �`��API�̃\�[�X�R�[�h�Ŏ���
  PlatformFlags GetExecute()const;
public:
  CPlatform() = default;

  void Initialize()override { Flags_ = GetOperationSystem() | GetExecute(); }

  const PlatformFlags& GetFlags()const { return Flags_; }
};
