#pragma once

class InputState
{
public:
  enum class State
  {
    Release,  //  ������Ă��Ȃ�
    Press,    //  ������Ă���
    Released, //  �����ꂽ�u��
    Pressed,  //  �����ꂽ�u��
  };
private:
  State State_ = State::Release;
public:
  bool Press()const { return (static_cast<int>(State_) & 0x01) != 0; }
  bool Pressed()const { return State_ == State::Pressed; }
  bool Released()const { return State_ == State::Released; }
  bool Release()const { return !Press(); }
public:
  void Update(bool _isPress)
  {
    //  �O�񂪉�����Ă�����
    if (Press())
    {
      State_ = (_isPress) ? State::Press : State::Released;
    }
    //  �O�񂪗�����Ă�����
    else
    {
      State_ = (_isPress) ? State::Pressed : State::Release;
    }
  }
};
