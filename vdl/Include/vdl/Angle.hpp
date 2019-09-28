#pragma once
#include "Fwd.hpp"
#include "MathConstants.hpp"

#define MEMBER_COMPARISON_OPERATOR(ClassName, Operator)\
[[nodiscard]] constexpr bool operator##Operator(const ClassName& _##ClassName)const noexcept { return Value_ Operator _##ClassName.Value_; }\

#define MEMBER_ARITHMETIC_OPERATOR(ClassName, Operator)\
[[nodiscard]] constexpr ClassName operator##Operator(const ClassName& _##ClassName)const noexcept { return{ Value_ Operator static_cast<float>(_##ClassName.Value_) }; }\
template<class T>\
[[nodiscard]] constexpr ClassName operator##Operator(T _s)const noexcept { return{ Value_ Operator static_cast<float>(_s) }; }\

#define MEMBER_ASSIGNMENT_OPERATOR(ClassName, Operator)\
ClassName& operator##Operator(const ClassName& _##ClassName)noexcept { Value_ Operator static_cast<float>(_##ClassName.Value_); return *this; }\
template<class T>\
ClassName& operator##Operator(T _s)noexcept { Value_ Operator static_cast<float>(_s); return *this; }\

#define MEMBER_COMMON_OPERATOR(ClassName)\
ClassName& operator=(ClassName&&) = default;\
\
MEMBER_COMPARISON_OPERATOR(ClassName, ==)\
\
MEMBER_COMPARISON_OPERATOR(ClassName, !=)\
\
MEMBER_COMPARISON_OPERATOR(ClassName, <)\
\
MEMBER_COMPARISON_OPERATOR(ClassName, >)\
\
MEMBER_COMPARISON_OPERATOR(ClassName, <=)\
\
MEMBER_COMPARISON_OPERATOR(ClassName, >=)\
\
MEMBER_ARITHMETIC_OPERATOR(ClassName, +)\
\
MEMBER_ARITHMETIC_OPERATOR(ClassName, -)\
\
MEMBER_ARITHMETIC_OPERATOR(ClassName, *)\
\
MEMBER_ARITHMETIC_OPERATOR(ClassName, /)\
\
MEMBER_ASSIGNMENT_OPERATOR(ClassName, =)\
\
MEMBER_ASSIGNMENT_OPERATOR(ClassName, +=)\
\
MEMBER_ASSIGNMENT_OPERATOR(ClassName, -=)\
\
MEMBER_ASSIGNMENT_OPERATOR(ClassName, *=)\
\
MEMBER_ASSIGNMENT_OPERATOR(ClassName, /=)\


#define COMPARISON_OPERATOR(ClassName, Operator)\
template<class T>\
[[nodiscard]] inline constexpr bool operator##Operator(T _s, const ClassName& _##ClassName)noexcept { return ClassName(static_cast<float>(_s)) Operator _##ClassName; }\

#define ARITHMETIC_OPERATOR(ClassName, Operator)\
template<class T>\
[[nodiscard]] inline constexpr ClassName operator##Operator(T _s, const ClassName& _##ClassName)noexcept { return ClassName(static_cast<float>(_s)) Operator _##ClassName; }\

#define COMMON_OPERATOR(ClassName)\
COMPARISON_OPERATOR(ClassName, ==)\
\
COMPARISON_OPERATOR(ClassName, !=)\
\
COMPARISON_OPERATOR(ClassName, <)\
\
COMPARISON_OPERATOR(ClassName, >)\
\
COMPARISON_OPERATOR(ClassName, <=)\
\
COMPARISON_OPERATOR(ClassName, >=)\
\
ARITHMETIC_OPERATOR(ClassName, +)\
\
ARITHMETIC_OPERATOR(ClassName, -)\
\
ARITHMETIC_OPERATOR(ClassName, *)\
\
ARITHMETIC_OPERATOR(ClassName, /)\

namespace vdl
{
  //  �ʓx�@
  class Radian
  {
    float Value_;
  public:
    Radian() = default;

    Radian(const Radian&) = default;

    Radian(Radian&&) = default;

   constexpr Radian(float _Radian)noexcept
      : Value_(_Radian) {}

    constexpr Radian(const Degree& _Degree)noexcept;
  public:
    constexpr operator float()const noexcept { return Value_; }
  public:
    MEMBER_COMMON_OPERATOR(Radian)
  public:
    [[nodiscard]] constexpr Radian Normalize()const noexcept
    {
      float Value = Value_;

      while (Value > Math::kHalfPI) Value -= Math::kPI;
      while (Value < -Math::kHalfPI) Value += Math::kPI;

      return Value;
    }
  };

  //  �x���@
  class Degree
  {
    float Value_;
  public:
    Degree() = default;

    Degree(const Degree&) = default;

    Degree(Degree&&) = default;

    constexpr Degree(float _Degree)noexcept
      : Value_(_Degree) {}

    constexpr Degree(const Radian& _Radian)noexcept;
  public:
    constexpr operator float()const noexcept { return Value_; }
  public:
    MEMBER_COMMON_OPERATOR(Degree)
  public:
    Degree& operator++()noexcept { ++Value_; return *this; }

    Degree operator++(int)noexcept { Degree Temp = *this; ++Value_; return Temp; }

    Degree& operator--()noexcept { --Value_; return *this; }

    Degree operator--(int)noexcept { Degree Temp = *this; --Value_; return Temp; }
  public:
    [[nodiscard]] constexpr Degree Normalize()const noexcept
    {
      float Value = Value_;

      while (Value > 180.0f) Value -= 360.0f;
      while (Value < -180.0f) Value += 360.0f;

      return Value;
    }
  };

  inline constexpr Radian::Radian(const Degree& _Degree)noexcept
    : Value_(Math::ToRadian(_Degree)) {}

  inline constexpr Degree::Degree(const Radian& _Radian)noexcept
    : Value_(Math::ToDegree(_Radian)) {}

  COMMON_OPERATOR(Radian);

  COMMON_OPERATOR(Degree);
}

namespace std
{
  [[nodiscard]] inline string to_string(const vdl::Radian& _Angle)
  {
    return to_string(static_cast<float>(_Angle));
  }

  [[nodiscard]] inline wstring to_wstring(const vdl::Radian& _Angle)
  {
    return to_wstring(static_cast<float>(_Angle));
  }

  template <class CharType>
  inline basic_ostream<CharType>& operator<<(basic_ostream<CharType>& _OStream, const vdl::Radian& _Angle)
  {
    return _OStream << static_cast<float>(_Angle);
  }

  template <class CharType>
  inline basic_istream<CharType>& operator>>(basic_istream<CharType>& _IStream, vdl::Radian& _Angle)
  {
    return _IStream >> reinterpret_cast<float&>(_Angle);
  }

  [[nodiscard]] inline string to_string(const vdl::Degree& _Angle)
  {
    return to_string(static_cast<float>(_Angle));
  }

  [[nodiscard]] inline wstring to_wstring(const vdl::Degree& _Angle)
  {
    return to_wstring(static_cast<float>(_Angle));
  }

  template <class CharType>
  inline basic_ostream<CharType>& operator<<(basic_ostream<CharType>& _OStream, const vdl::Degree& _Angle)
  {
    return _OStream << static_cast<float>(_Angle);
  }

  template <class CharType>
  inline basic_istream<CharType>& operator>>(basic_istream<CharType>& _IStream, vdl::Degree& _Angle)
  {
    return _IStream >> reinterpret_cast<float&>(_Angle);
  }
}

#undef MEMBER_COMPARISON_OPERATOR
#undef MEMBER_ARITHMETIC_OPERATOR
#undef MEMBER_ASSIGNMENT_OPERATOR
#undef MEMBER_COMMON_OPERATOR
#undef COMPARISON_OPERATOR
#undef ARITHMETIC_OPERATOR
#undef COMMON_OPERATOR
