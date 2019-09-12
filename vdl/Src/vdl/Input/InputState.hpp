#pragma once

class InputState
{
public:
  enum class State
  {
    eRelease,  //  ‰Ÿ‚³‚ê‚Ä‚¢‚È‚¢
    ePress,    //  ‰Ÿ‚³‚ê‚Ä‚¢‚é
    eReleased, //  —£‚³‚ê‚½uŠÔ
    ePressed,  //  ‰Ÿ‚³‚ê‚½uŠÔ
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
    //  ‘O‰ñ‚ª‰Ÿ‚³‚ê‚Ä‚¢‚½
    if (Press())
    {
      State_ = (_isPress) ? State::ePress : State::eReleased;
    }
    //  ‘O‰ñ‚ª—£‚³‚ê‚Ä‚¢‚½
    else
    {
      State_ = (_isPress) ? State::ePressed : State::eRelease;
    }
  }
};
