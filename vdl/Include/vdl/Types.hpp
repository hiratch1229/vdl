#pragma once

namespace vdl
{
  using int8_t = signed char;
  using int16_t = short;
  using int32_t = int;
  using int64_t = long long;

  using uint8_t = unsigned char;
  using uint16_t = unsigned short;
  using uint32_t = unsigned int;
  using uint64_t = unsigned long long;

  using uint = unsigned int;

  using ID = uint;

  //-------------------------
  //  Type2.hpp
  //-------------------------
  template<class Type> struct Type2;
  using int2 = Type2<int>;
  using uint2 = Type2<unsigned int>;
  using float2 = Type2<float>;

  //-------------------------
  //  Type3.hpp
  //-------------------------
  template<class Type> struct Type3;
  using int3 = Type3<int>;
  using uint3 = Type3<unsigned int>;
  using float3 = Type3<float>;

  //-------------------------
  //  Type4.hpp
  //-------------------------
  template<class Type> struct Type4;
  using int4 = Type4<int>;
  using uint4 = Type4<unsigned int>;
  using float4 = Type4<float>;
}
