#pragma once
#include "../IMouse.hpp"

#include <vdl/Input/InputState.hpp>

#include <vdl/pch/Linux/pch.hpp>

class CMouse : public IMouse
{
  //  �g�p�{�^����
  static constexpr vdl::uint kButtonNum = 5;
private:
  //  ���͏��
  InputState InputStatus_[kButtonNum];
  //  ���W
  vdl::int2 Pos_;
  //  �ړ���
  vdl::int2 Delta_;
  //  ���݂̃z�C�[���̈ړ���
  vdl::int2 Scroll_;
private:
  bool isWithinRange(vdl::uint _Code)const { return (0 <= _Code && _Code < kButtonNum); }
public:
  CMouse() = default;

  PlatformFlags GetPlatform()const final { return PlatformFlag::eLinux; }

  void Initialize()override;

  void Update()override;

  bool Press(vdl::uint _Code)const override { return isWithinRange(_Code) ? InputStatus_[_Code].Press() : false; }

  bool Pressed(vdl::uint _Code)const override { return isWithinRange(_Code) ? InputStatus_[_Code].Pressed() : false; }

  bool Released(vdl::uint _Code)const override { return isWithinRange(_Code) ? InputStatus_[_Code].Released() : false; }

  bool Release(vdl::uint _Code)const override { return isWithinRange(_Code) ? InputStatus_[_Code].Release() : false; }

  const vdl::int2& GetPos()const override { return Pos_; }

  const vdl::int2& GetDelta()const override { return Delta_; }

  const vdl::int2& GetWheel()const override { return Scroll_; }

  void SetPos(const vdl::int2& _Pos)override;

  void Scroll(const vdl::int2& _Scroll)override;
};
