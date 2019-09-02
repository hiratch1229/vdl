#pragma once
#include <vdl/Fwd.hpp>
#include <vdl/Type2.hpp>

class IMouse
{
public:
  static IMouse* Create();

  IMouse() = default;

  virtual ~IMouse() = default;

  virtual void Initialize() = 0;

  virtual void Update() = 0;

  virtual bool Press(int _Number)const = 0;

  virtual bool Pressed(int _Number)const = 0;

  virtual bool Released(int _Number)const = 0;

  virtual bool Release(int _Number)const = 0;

  virtual vdl::int2 GetPos()const = 0;

  virtual vdl::int2 GetDelta()const = 0;

  virtual vdl::int2 GetWheel()const = 0;

  virtual void SetPos(vdl::int2 _Pos) = 0;

  virtual void Scroll(vdl::int2 _Scroll) = 0;
};
