#pragma once
#include <type_traits>
#include <limits>

#undef max
#undef min

namespace vdl::Math
{
  //  å^ÇÃç≈ëÂíl
  template<class T, std::enable_if_t<std::is_fundamental<T>::value, std::nullptr_t> = nullptr>
  inline constexpr T kMax = std::numeric_limits<T>::max();

  //  å^ÇÃç≈í·íl
  template<class T, std::enable_if_t<std::is_fundamental<T>::value, std::nullptr_t> = nullptr>
  inline constexpr T kMin = std::numeric_limits<T>::min();

  //  ÉŒ
  template<class T>
  inline constexpr T kPI = static_cast<T>(3.14159265358979323846);
  inline constexpr float kPIf = kPI<float>;

  //  2ÉŒ
  template<class T>
  inline constexpr T kTwoPI = kPI<T> * static_cast<T>(2);
  inline constexpr float kTwoPIf = kTwoPI<float>;

  //  1/2ÉŒ
  template<class T>
  inline constexpr T kHalfPI = kPI<T> / static_cast<T>(2);
  inline constexpr float kHalfPIf = kHalfPI<float>;
}
