#pragma once

class InputState
{
public:
  enum class State
  {
    Release,  //  ‰Ÿ‚³‚ê‚Ä‚¢‚È‚¢
    Press,    //  ‰Ÿ‚³‚ê‚Ä‚¢‚é
    Released, //  —£‚³‚ê‚½uŠÔ
    Pressed,  //  ‰Ÿ‚³‚ê‚½uŠÔ
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
    //  ‘O‰ñ‚ª‰Ÿ‚³‚ê‚Ä‚¢‚½
    if (Press())
    {
      State_ = (_isPress) ? State::Press : State::Released;
    }
    //  ‘O‰ñ‚ª—£‚³‚ê‚Ä‚¢‚½
    else
    {
      State_ = (_isPress) ? State::Pressed : State::Release;
    }
  }
};
