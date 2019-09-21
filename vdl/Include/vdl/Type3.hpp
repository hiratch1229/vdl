#pragma once
#include "Fwd.hpp"

#include <cmath>
#include <string>
#include <fstream>

#define ARITHMETIC_OPERATOR(Operator)\
template<class T>\
[[nodiscard]] constexpr auto operator##Operator(const Type3<T>& _v)const noexcept->Type3<decltype(x Operator _v.x)>\
{\
  return{ x Operator _v.x, y Operator _v.y, z Operator _v.z };\
}\
template<class T>\
[[nodiscard]] constexpr auto operator##Operator(T _s)const noexcept->Type3<decltype(x Operator _s)>\
{\
  return{ x Operator _s, y Operator _s, z Operator _s };\
}\

#define ASSIGNMENT_OPERATOR(Operator)\
template<class T>\
Type3& operator##Operator(const Type3<T>& _v)noexcept\
{\
  x Operator static_cast<Type>(_v.x);\
  y Operator static_cast<Type>(_v.y);\
  z Operator static_cast<Type>(_v.z);\
  \
  return *this;\
}\
template<class T>\
Type3& operator##Operator(T _s)noexcept\
{\
  x Operator static_cast<Type>(_s);\
  y Operator static_cast<Type>(_s);\
  z Operator static_cast<Type>(_s);\
  \
  return *this;\
}\

namespace vdl
{
  template<class Type>
  struct Type3
  {
    Type x, y, z;
  public:
    Type3() = default;

    constexpr Type3(Type _x, Type _y, Type _z)noexcept
      : x(_x)
      , y(_y)
      , z(_z) {}

    constexpr Type3(Type _xyz)noexcept
      : x(_xyz)
      , y(_xyz)
      , z(_xyz) {}

    template<class X, class Y, class Z>
    constexpr Type3(X _x, Y _y, Z _z)noexcept
      : x(static_cast<Type>(_x))
      , y(static_cast<Type>(_y))
      , z(static_cast<Type>(_z)) {}

    template<class T>
    constexpr Type3(const Type3<T>& _v)noexcept
      : x(static_cast<Type>(_v.x))
      , y(static_cast<Type>(_v.y))
      , z(static_cast<Type>(_v.z)) {}

    template<class XY, class Z>
    constexpr Type3(const Type2<XY>& _xy, Z _z)noexcept
      : x(static_cast<Type>(_xy.x))
      , y(static_cast<Type>(_xy.y))
      , z(static_cast<Type>(_z)) {}

    template<class X, class YZ>
    constexpr Type3(X _x, const Type2<YZ>& _yz)noexcept
      : x(static_cast<Type>(_x))
      , y(static_cast<Type>(_yz.x))
      , z(static_cast<Type>(_yz.y)) {}
  public:
    [[nodiscard]] constexpr bool operator==(const Type3& _v)const noexcept { return x == _v.x && y == _v.y && z == _v.z; }

    [[nodiscard]] constexpr bool operator!=(const Type3& _v)const noexcept { return x != _v.x || y != _v.y || z != _v.z; }

    [[nodiscard]] constexpr Type3 operator+()const noexcept { return *this; }

    [[nodiscard]] constexpr Type3 operator-()const noexcept { return { -x, -y, -z }; }

    ARITHMETIC_OPERATOR(+);

    ARITHMETIC_OPERATOR(-);

    ARITHMETIC_OPERATOR(*);

    ARITHMETIC_OPERATOR(/ );

    ASSIGNMENT_OPERATOR(= );

    ASSIGNMENT_OPERATOR(+= );

    ASSIGNMENT_OPERATOR(-= );

    ASSIGNMENT_OPERATOR(*= );

    ASSIGNMENT_OPERATOR(/= );
  public:
    [[nodiscard]] constexpr Type Length()const noexcept { return std::sqrt(LengthSq()); }

    [[nodiscard]] constexpr Type LengthSq()const noexcept { return x * x + y * y + z * z; }

    [[nodiscard]] constexpr Type3 Normalize()const noexcept { return *this / Length(); }

    template<class T>
    [[nodiscard]] constexpr auto Dot(const Type3<T>& _v)const noexcept->decltype(x * _v.x) { return x * _v.x + y * _v.y + z * _v.z; }

    template<class T>
    [[nodiscard]] constexpr auto Cross(const Type3<T>& _v)const noexcept->Type3<decltype(x * _v.x)> { return{ y * _v.z - z * _v.y, z * _v.x - x * _v.z, x * _v.y - y * _v.x }; }
  public:
    [[nodiscard]] constexpr Type2<Type> xy()const noexcept { return { x, y }; }

    [[nodiscard]] constexpr Type2<Type> xz()const noexcept { return { x, z }; }

    [[nodiscard]] constexpr Type2<Type> yz()const noexcept { return { y, z }; }
  public:
    [[nodiscard]] static constexpr Type3 Right()noexcept { return { 1, 0, 0 }; }

    [[nodiscard]] static constexpr Type3 Left()noexcept { return { -1, 0, 0 }; }

    [[nodiscard]] static constexpr Type3 Up()noexcept { return { 0, 1, 0 }; }

    [[nodiscard]] static constexpr Type3 Down()noexcept { return { 0, -1, 0 }; }

    [[nodiscard]] static constexpr Type3 Forward()noexcept { return { 0, 0, 1 }; }

    [[nodiscard]] static constexpr Type3 Back()noexcept { return { 0, 0, -1 }; }
  };

  template<class T, class Type>
  [[nodiscard]] inline constexpr auto operator==(T _s, const Type3<Type>& _v)noexcept { return _v == _s; }

  template<class T, class Type>
  [[nodiscard]] inline constexpr auto operator!=(T _s, const Type3<Type>& _v)noexcept { return _v != _s; }

  template<class T, class Type>
  [[nodiscard]] inline constexpr auto operator+(T _s, const Type3<Type>& _v)noexcept { return _v + _s; }

  template<class T, class Type>
  [[nodiscard]] inline constexpr auto operator*(T _s, const Type3<Type>& _v)noexcept { return _v * _s; }
}

namespace std
{
  template <class Type>
  [[nodiscard]] inline string to_string(const vdl::Type3<Type>& _v)
  {
    return to_string(_v.x) + ',' + to_string(_v.y) + ',' + to_string(_v.z);
  }

  template <class Type>
  [[nodiscard]] inline wstring to_wstring(const vdl::Type3<Type>& _v)
  {
    return to_wstring(_v.x) + L',' + to_wstring(_v.y) + L',' + to_wstring(_v.z);
  }

  template <class CharType, class Type>
  inline basic_ostream<CharType>& operator<<(basic_ostream<CharType>& _OStream, const vdl::Type3<Type>& _v)
  {
    return _OStream << _v.x << CharType(',') << _v.y << CharType(',') << _v.z;
  }

  template <class CharType, class Type>
  inline basic_istream<CharType>& operator>>(basic_istream<CharType>& _IStream, vdl::Type3<Type>& _v)
  {
    CharType Temp;

    return _IStream >> _v.x >> Temp >> _v.y >> Temp >> _v.z;
  }
}

#undef ARITHMETIC_OPERATOR
#undef ASSIGNMENT_OPERATOR
