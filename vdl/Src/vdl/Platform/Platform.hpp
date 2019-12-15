#pragma once
#include <assert.h>
#include <type_traits>

enum class PlatformType
{
  eCommon,

  eDirectX11,
  eDirectX12,
  eVulkan,

  eWindows,
};

template<class Derived, class Base>
inline Derived* Cast(Base* _pInterface)
{
  static_assert(std::is_base_of<Base, Derived>::value);

#if defined VDL_USING_DIRECTX11
  constexpr PlatformType kExecuteType = PlatformType::eDirectX11;
#elif defined VDL_USING_DIRECTX12
  constexpr PlatformType kExecuteType = PlatformType::eDirectX12;
#elif defined VDL_USING_VULKAN
  constexpr PlatformType kExecuteType = PlatformType::eVulkan;
#else
  constexpr PlatformType kExecuteType = PlatformType::eCommon;
#endif

#if defined VDL_USING_WINDOWS
  constexpr PlatformType kOperationSystemType = PlatformType::eWindows;
#else
  constexpr PlatformType kOperationSystemType = PlatformType::eCommon;
#endif

  const PlatformType Type = _pInterface->GetPlatform();
  assert(Type == kExecuteType || Type == kOperationSystemType);

  return static_cast<Derived*>(_pInterface);
}
