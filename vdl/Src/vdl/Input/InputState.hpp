#pragma once

class InputState
{
public:
  enum class State
  {
    eRelease,  //  ������Ă��Ȃ�
    ePress,    //  ������Ă���
    eReleased, //  �����ꂽ�u��
    ePressed,  //  �����ꂽ�u��
  };
private:
  State State_ = State::eRelease;
public:
  [[nodiscard]] bool Press()const { return (static_cast<int>(State_) & 0x01) != 0; }
  [[nodiscard]] bool Pressed()const { return State_ == State::ePressed; }
  [[nodiscard]] bool Released()const { return State_ == State::eReleased; }
  [[nodiscard]] bool Release()const { return !Press(); }
public:
  void Update(bool _isPress)
  {
    //  �O�񂪉�����Ă�����
    if (Press())
    {
      State_ = (_isPress) ? State::ePress : State::eReleased;
    }
    //  �O�񂪗�����Ă�����
    else
    {
      State_ = (_isPress) ? State::ePressed : State::eRelease;
    }
  }
};
