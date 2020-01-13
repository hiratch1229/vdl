#pragma once
#include "IPlatform.hpp"

class CPlatform : public IPlatform
{
  PlatformFlags Flags_;
private:
  //  OSのソースコードで実装
  PlatformFlags GetOperationSystem()const;
  //  描画APIのソースコードで実装
  PlatformFlags GetExecute()const;
public:
  CPlatform() = default;

  void Initialize()override { Flags_ = GetOperationSystem() | GetExecute(); }

  const PlatformFlags& GetFlags()const { return Flags_; }
};
