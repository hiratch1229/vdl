#pragma once
#include "../IMouse.hpp"

#include <vdl/Input/InputState.hpp>

#include <vdl/pch/Windows/pch.hpp>

#include <mutex>

class IKeyboard;

class CMouse : public IMouse
{
  //  �g�p�{�^����
  static constexpr vdl::uint kButtonNum = 5;
private:
  IKeyboard* pKeyboard_;
  HWND hWnd_;
private:
  //  ���͏��
  InputState InputStatus_[kButtonNum];
  //  ���W
  vdl::int2 Pos_;
  //  �ړ���
  vdl::int2 Delta_;
  //  ���݂̃z�C�[���̈ړ���
  vdl::int2 Scroll_;
  //  �z�C�[���̈ړ��ʂ̒~��
  vdl::int2 AccumulationScroll_;
  //  ��ʉ𑜓x
  vdl::float2 DpiScale_;
  //  �X�N���[���̔r������
  std::mutex ScrollMutex_;
private:
  bool isWithinRange(vdl::uint _Code)const { return (0 <= _Code && _Code < kButtonNum); }
public:
  CMouse() = default;

  PlatformFlags GetPlatform()const final { return PlatformFlag::eWindows; }

  void Initialize()override;

  void Update()override;

  bool Press(vdl::uint _Code)const override { return isWithinRange(_Code) ? InputStatus_[_Code].Press() : false; }

  bool Pressed(vdl::uint _Code)const override { return isWithinRange(_Code) ? InputStatus_[_Code].Pressed() : false; }

  bool Released(vdl::uint _Code)const override { return isWithinRange(_Code) ? InputStatus_[_Code].Released() : false; }

  bool Release(vdl::uint _Code)const override { return isWithinRange(_Code) ? InputStatus_[_Code].Release() : false; }

  vdl::int2 GetPos()const override { return Pos_; }

  vdl::int2 GetDelta()const override { return Delta_; }

  vdl::int2 GetWheel()const override { return Scroll_; }

  void SetPos(const vdl::int2& _Pos)override;

  void Scroll(const vdl::int2& _Scroll)override;
};
