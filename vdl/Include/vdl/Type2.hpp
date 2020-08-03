#pragma once
#include "Fwd.hpp"
#include "Math.hpp"

#include <string>
#include <fstream>

#define ARITHMETIC_OPERATOR(Operator)\
template<class T>\
[[nodiscard]] constexpr auto operator Operator(const Type2<T>& _v)const noexcept->Type2<decltype(x Operator _v.x)>\
{\
  return{ x Operator _v.x, y Operator _v.y };\
}\
template<class T>\
[[nodiscard]] constexpr auto operator Operator(T _s)const noexcept->Type2<decltype(x Operator _s)>\
{\
  return{ x Operator _s, y Operator _s };\
}\

#define ASSIGNMENT_OPERATOR(Operator)\
template<class T>\
Type2& operator Operator(const Type2<T>& _v)noexcept\
{\
  x Operator static_cast<Type>(_v.x);\
  y Operator static_cast<Type>(_v.y);\
  \
  return *this;\
}\
template<class T>\
Type2& operator Operator(T _s)noexcept\
{\
  x Operator static_cast<Type>(_s);\
  y Operator static_cast<Type>(_s);\
  \
  return *this;\
}\

namespace vdl
{
  template<class Type>
  struct Type2
  {
    static_assert(std::is_fundamental<Type>::value, "ëŒâûÇµÇƒÇ¢Ç»Ç¢å^Ç≈Ç∑ÅB");

    Type x, y;
  public:
    Type2() = default;

    Type2(const Type2&) = default;

    Type2(Type2&&) = default;

    constexpr Type2(Type _x, Type _y)noexcept
      : x(_x)
      , y(_y) {}

    constexpr Type2(Type _xy)noexcept
      : x(_xy)
      , y(_xy) {}

    template<class X, class Y>
    constexpr Type2(X _x, Y _y)noexcept
      : x(static_cast<Type>(_x))
      , y(static_cast<Type>(_y)) {}

    template<class T>
    constexpr Type2(const Type2<T>& _v)noexcept
      : x(static_cast<Type>(_v.x))
      , y(static_cast<Type>(_v.y)) {}
  public:
    Type2& operator=(const Type2&) = default;

    Type2& operator=(Type2&&) = default;

    [[nodiscard]] constexpr Type& operator[](uint _Index) { return (&x)[_Index]; }

    [[nodiscard]] constexpr const Type& operator[](uint _Index)const { return (&x)[_Index]; }

    [[nodiscard]] constexpr bool operator==(const Type2& _v)const noexcept { return x == _v.x && y == _v.y; }

    [[nodiscard]] constexpr bool operator!=(const Type2& _v)const noexcept { return x != _v.x || y != _v.y; }

    [[nodiscard]] constexpr Type2 operator+()const noexcept { return *this; }

    [[nodiscard]] constexpr Type2 operator-()const noexcept { return { -x, -y }; }

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
    [[nodiscard]] constexpr Type Length()const noexcept { return Math::Sqrt(LengthSq()); }

    [[nodiscard]] constexpr Type LengthSq()const noexcept { return x * x + y * y; }

    [[nodiscard]] constexpr Type2 Normalize()const noexcept { return *this / Length(); }

    template<class T>
    [[nodiscard]] constexpr auto Dot(const Type2<T>& _v)const noexcept->decltype(x* _v.x) { return x * _v.x + y * _v.y; }

    template<class T>
    [[nodiscard]] constexpr auto Cross(const Type2<T>& _v)const noexcept->decltype(x* _v.x) { return x * _v.y - y * _v.x; }
  public:
    [[nodiscard]] static constexpr Type2 Right()noexcept { return { 1, 0 }; }

    [[nodiscard]] static constexpr Type2 Left()noexcept { return { -1, 0 }; }

    [[nodiscard]] static constexpr Type2 Up()noexcept { return { 0, 1 }; }

    [[nodiscard]] static constexpr Type2 Down()noexcept { return { 0, -1 }; }
  };

  template<class T, class Type>
  [[nodiscard]] inline constexpr auto operator==(T _s, const Type2<Type>& _v)noexcept { return _v == _s; }

  template<class T, class Type>
  [[nodiscard]] inline constexpr auto operator!=(T _s, const Type2<Type>& _v)noexcept { return _v != _s; }

  template<class T, class Type>
  [[nodiscard]] inline constexpr auto operator+(T _s, const Type2<Type>& _v)noexcept { return _v + _s; }

  template<class T, class Type>
  [[nodiscard]] inline constexpr auto operator*(T _s, const Type2<Type>& _v)noexcept { return _v * _s; }
}

namespace std
{
  template <class Type>
  [[nodiscard]] inline string to_string(const vdl::Type2<Type>& _v)
  {
    return to_string(_v.x) + ',' + to_string(_v.y);
  }

  template <class Type>
  [[nodiscard]] inline wstring to_wstring(const vdl::Type2<Type>& _v)
  {
    return to_wstring(_v.x) + L',' + to_wstring(_v.y);
  }

  template <class CharType, class Type>
  inline basic_ostream<CharType>& operator<<(basic_ostream<CharType>& _OStream, const vdl::Type2<Type>& _v)
  {
    return _OStream << _v.x << CharType(',') << _v.y;
  }

  template <class CharType, class Type>
  inline basic_istream<CharType>& operator>>(basic_istream<CharType>& _IStream, vdl::Type2<Type>& _v)
  {
    CharType Temp;

    return _IStream >> _v.x >> Temp >> _v.y;
  }
}

#undef ARITHMETIC_OPERATOR
#undef ASSIGNMENT_OPERATOR
