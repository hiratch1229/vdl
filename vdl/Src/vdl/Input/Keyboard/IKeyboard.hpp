#pragma once

class IKeyboard
{
public:
  static IKeyboard* Create();

  IKeyboard() = default;

  virtual ~IKeyboard() = default;

  virtual void Update() = 0;

  virtual bool Press(int _Number)const = 0;

  virtual bool Pressed(int _Number)const = 0;

  virtual bool Released(int _Number)const = 0;

  virtual bool Release(int _Number)const = 0;

  virtual bool AnyKeyPress()const = 0;

  virtual bool AnyKeyPressed()const = 0;

  virtual bool AnyKeyReleased()const = 0;
};
