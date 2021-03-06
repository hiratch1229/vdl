#pragma once

class InputState
{
public:
  enum class State
  {
    eRelease,  //  押されていない
    ePress,    //  押されている
    eReleased, //  離された瞬間
    ePressed,  //  押された瞬間
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
    //  前回が押されていた時
    if (Press())
    {
      State_ = (_isPress) ? State::ePress : State::eReleased;
    }
    //  前回が離されていた時
    else
    {
      State_ = (_isPress) ? State::ePressed : State::eRelease;
    }
  }
};
