#pragma once
#if defined _DEBUG
#include <vdl/Engine.hpp>
#endif
#include "Platform.hpp"
#include <assert.h>

class IPlatform
{
public:
  static IPlatform* Create();

  IPlatform() = default;

  virtual ~IPlatform() = default;

  virtual void Initialize() = 0;

  [[nodiscard]] virtual const PlatformFlags& GetFlags()const = 0;
};

template<class Derived, class Base>
inline Derived* Cast(Base* _pInterface)
{
  static_assert(std::is_base_of<Base, Derived>::value);

  assert(_pInterface->GetPlatform() & Engine::Get<IPlatform>()->GetFlags());

  return static_cast<Derived*>(_pInterface);
}
