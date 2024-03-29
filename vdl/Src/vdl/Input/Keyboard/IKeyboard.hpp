#pragma once
#include <vdl/Types.hpp>

#include <vdl/Platform/Platform.hpp>

class IKeyboard
{
public:
  static IKeyboard* Create();

  IKeyboard() = default;

  virtual ~IKeyboard() = default;

  [[nodiscard]] virtual PlatformFlags GetPlatform()const = 0;

  virtual void Initialize() = 0;

  virtual void Update() = 0;

  [[nodiscard]] virtual bool Press(vdl::uint _Code)const = 0;

  [[nodiscard]] virtual bool Pressed(vdl::uint _Code)const = 0;

  [[nodiscard]] virtual bool Released(vdl::uint _Code)const = 0;

  [[nodiscard]] virtual bool Release(vdl::uint _Code)const = 0;
};
