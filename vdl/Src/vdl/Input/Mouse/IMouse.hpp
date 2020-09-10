#pragma once
#include <vdl/Fwd.hpp>
#include <vdl/Type2.hpp>

#include <vdl/Platform/Platform.hpp>

enum class MouseCursorType : vdl::uint8_t
{
  eArrow,
  eTextInput,
  eResizeAll,
  eResizeEW,
  eResizeNS,
  eResizeNESW,
  eResizeNWSE,
  eHand,
  eNotArrow,
};

class IMouse
{
public:
  static IMouse* Create();

  IMouse() = default;

  virtual ~IMouse() = default;

  [[nodiscard]] virtual PlatformFlags GetPlatform()const = 0;

  virtual void Initialize() = 0;

  virtual void Update() = 0;

  [[nodiscard]] virtual bool Press(vdl::uint _Code)const = 0;

  [[nodiscard]] virtual bool Pressed(vdl::uint _Code)const = 0;

  [[nodiscard]] virtual bool Released(vdl::uint _Code)const = 0;

  [[nodiscard]] virtual bool Release(vdl::uint _Code)const = 0;

  [[nodiscard]] virtual const vdl::int2& GetPos()const = 0;

  [[nodiscard]] virtual const vdl::int2& GetDelta()const = 0;

  [[nodiscard]] virtual const vdl::int2& GetWheel()const = 0;

  virtual void SetPos(const vdl::int2& _Pos) = 0;

  virtual void Scroll(const vdl::int2& _Scroll) = 0;

  virtual void SetCursor(MouseCursorType _Type) = 0;
};
