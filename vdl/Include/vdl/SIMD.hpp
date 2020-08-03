#pragma once
#include "Types.hpp"
#include "Type2.hpp"
#include "Type4.hpp"

//  AVX,AVX2,FMAまで対応
#include <immintrin.h>

namespace vdl
{
  template<class Type>
  class alignas(16) SIMD { static_assert(std::is_fundamental<Type>::value, "対応していない型です。"); };

  //template<>
  //class alignas(16) SIMD<int32_t>
  //{
  //  __m128i Data_;
  //public:
  //  SIMD() = default;
  //
  //  SIMD(const SIMD&) = default;
  //
  //  SIMD(SIMD&&) = default;
  //
  //  ~SIMD() = default;
  //
  //  SIMD(const __m128i& _Data)noexcept
  //    : Data_(_Data) {}
  //
  //  SIMD(int32_t _x, int32_t _y, int32_t _z, int32_t _w)noexcept
  //  {
  //    Data_ = ::_mm_set_epi32(_w, _z, _y, _x);
  //  }
  //
  //  SIMD(int32_t _xyzw)noexcept
  //    : SIMD(_xyzw, _xyzw, _xyzw, _xyzw) {}
  //
  //  SIMD(const int4& _vec)noexcept
  //  {
  //    Data_ = ::_mm_load_ps(&_vec.x);
  //  }
  //public:
  //  [[nodiscard]] operator __m128()const noexcept { return Data_; }
  //public:
  //  SIMD& operator=(const SIMD&) = default;
  //
  //  SIMD& operator=(SIMD&&) = default;
  //
  //  [[nodiscard]] SIMD operator+()const noexcept { return *this; }
  //
  //  [[nodiscard]] SIMD operator-()const noexcept { return { -x(), -y(), -z(), -w() }; }
  //
  //  [[nodiscard]] SIMD operator+(const SIMD& _Simd)const noexcept { return ::_mm_add_ps(Data_, _Simd.Data_); }
  //
  //  [[nodiscard]] SIMD operator+(float _s)const noexcept { return *this + SIMD(_s); }
  //
  //  [[nodiscard]] SIMD operator-(const SIMD& _Simd)const noexcept { return ::_mm_sub_ps(Data_, _Simd.Data_); }
  //
  //  [[nodiscard]] SIMD operator-(float _s)const noexcept { return *this - SIMD(_s); }
  //
  //  [[nodiscard]] SIMD operator*(const SIMD& _Simd)const noexcept { return ::_mm_mul_ps(Data_, _Simd.Data_); }
  //
  //  [[nodiscard]] SIMD operator*(float _s)const noexcept { return *this * SIMD(_s); }
  //
  //  [[nodiscard]] SIMD operator/(const SIMD& _Simd)const noexcept { return ::_mm_div_ps(Data_, _Simd.Data_); }
  //
  //  [[nodiscard]] SIMD operator/(float _s)const noexcept { return *this / SIMD(_s); }
  //
  //  SIMD& operator+=(const SIMD& _Simd)noexcept { Data_ = ::_mm_add_ps(Data_, _Simd.Data_); return *this; }
  //
  //  SIMD& operator+=(float _s)noexcept { *this += SIMD(_s); return *this; }
  //
  //  SIMD& operator-=(const SIMD& _Simd)noexcept { Data_ = ::_mm_sub_ps(Data_, _Simd.Data_); return *this; }
  //
  //  SIMD& operator-=(float _s)noexcept { *this -= SIMD(_s); return *this; }
  //
  //  SIMD& operator*=(const SIMD& _Simd)noexcept { Data_ = ::_mm_mul_ps(Data_, _Simd.Data_); return *this; }
  //
  //  SIMD& operator*=(float _s)noexcept { *this *= SIMD(_s); return *this; }
  //
  //  SIMD& operator/=(const SIMD& _Simd)noexcept { Data_ = ::_mm_div_ps(Data_, _Simd.Data_); return *this; }
  //
  //  SIMD& operator/=(float _s)noexcept { *this /= SIMD(_s); return *this; }
  //  //public:
  //  //  template<uint _x, uint _y, uint _z, uint _w>
  //  //  [[nodiscard]] SIMD Shuffle()const
  //  //  {
  //  //    static_assert(_x < 4 && _y < 4 && _z < 4 && _w < 4, "範囲を指定しています。");
  //  //
  //  //    static constexpr uint kShuffleMask = _MM_SHUFFLE(_w, _z, _y, _x);
  //  //    return ::_mm_shuffle_ps(Data_, Data_, kShuffleMask);
  //  //  }
  //  //public:
  //  //  template<uint _Index>
  //  //  [[nodiscard]] int8_t get()const noexcept
  //  //  {
  //  //    static_assert(_Index < 16, "範囲を指定しています。");
  //  //
  //  //    return Shuffle<_Index, _Index, _Index, _Index>().x();
  //  //  }
  //public:
  //  [[nodiscard]] static SIMD Zero()noexcept { return ::_mm_setzero_si128(); }
  //};

  //----------------------------------------------------------------------------------------------------

  template<>
  class alignas(16) SIMD<float>
  {
    __m128 Data_;
  public:
    SIMD() = default;

    SIMD(const SIMD&) = default;

    SIMD(SIMD&&) = default;

    ~SIMD() = default;

    SIMD(const __m128& _Data)noexcept
      : Data_(_Data) {}

    SIMD(float _x, float _y, float _z, float _w)noexcept
    {
      Data_ = ::_mm_set_ps(_w, _z, _y, _x);
    }

    SIMD(float _xyzw)noexcept
      : SIMD(_xyzw, _xyzw, _xyzw, _xyzw) {}

    SIMD(const float4& _vec)noexcept
    {
      Data_ = ::_mm_load_ps(&_vec.x);
    }
  public:
    [[nodiscard]] operator __m128()const noexcept { return Data_; }
  public:
    SIMD& operator=(const SIMD&) = default;

    SIMD& operator=(SIMD&&) = default;

    [[nodiscard]] SIMD operator+()const noexcept { return *this; }

    [[nodiscard]] SIMD operator-()const noexcept { return *this * SIMD(-1.0f); }

    [[nodiscard]] SIMD operator+(const SIMD& _Simd)const noexcept { return ::_mm_add_ps(Data_, _Simd.Data_); }

    [[nodiscard]] SIMD operator+(float _s)const noexcept { return *this + SIMD(_s); }

    [[nodiscard]] SIMD operator-(const SIMD& _Simd)const noexcept { return ::_mm_sub_ps(Data_, _Simd.Data_); }

    [[nodiscard]] SIMD operator-(float _s)const noexcept { return *this - SIMD(_s); }

    [[nodiscard]] SIMD operator*(const SIMD& _Simd)const noexcept { return ::_mm_mul_ps(Data_, _Simd.Data_); }

    [[nodiscard]] SIMD operator*(float _s)const noexcept { return *this * SIMD(_s); }

    [[nodiscard]] SIMD operator/(const SIMD& _Simd)const noexcept { return ::_mm_div_ps(Data_, _Simd.Data_); }

    [[nodiscard]] SIMD operator/(float _s)const noexcept { return *this / SIMD(_s); }

    SIMD& operator+=(const SIMD& _Simd)noexcept { Data_ = ::_mm_add_ps(Data_, _Simd.Data_); return *this; }

    SIMD& operator+=(float _s)noexcept { *this += SIMD(_s); return *this; }

    SIMD& operator-=(const SIMD& _Simd)noexcept { Data_ = ::_mm_sub_ps(Data_, _Simd.Data_); return *this; }

    SIMD& operator-=(float _s)noexcept { *this -= SIMD(_s); return *this; }

    SIMD& operator*=(const SIMD& _Simd)noexcept { Data_ = ::_mm_mul_ps(Data_, _Simd.Data_); return *this; }

    SIMD& operator*=(float _s)noexcept { *this *= SIMD(_s); return *this; }

    SIMD& operator/=(const SIMD& _Simd)noexcept { Data_ = ::_mm_div_ps(Data_, _Simd.Data_); return *this; }

    SIMD& operator/=(float _s)noexcept { *this /= SIMD(_s); return *this; }
  public:
    template<uint _x, uint _y, uint _z, uint _w,
      std::enable_if_t < _x < 4 && _y < 4 && _z < 4 && _w < 4, std::nullptr_t> = nullptr>
      [[nodiscard]] SIMD Swizzle()const noexcept
    {
      static constexpr int kShuffleMask = _MM_SHUFFLE(_w, _z, _y, _x);
      return ::_mm_permute_ps(Data_, kShuffleMask);
    }

    template<>
    [[nodiscard]] SIMD Swizzle<0, 0, 2, 2>()const noexcept
    {
      return ::_mm_moveldup_ps(Data_);
    }

    template<>
    [[nodiscard]] SIMD Swizzle<1, 1, 3, 3>()const noexcept
    {
      return ::_mm_movehdup_ps(Data_);
    }

    template<uint _x, uint _y, uint _z, uint _w,
      std::enable_if_t < _x < 4 && _y < 4 && _z < 4 && _w < 4, std::nullptr_t> = nullptr>
      [[nodiscard]] SIMD Shuffle(const SIMD & _Data)const noexcept
    {
      static constexpr uint kShuffleMask = _MM_SHUFFLE(_w, _z, _y, _x);
      return ::_mm_shuffle_ps(Data_, _Data, kShuffleMask);
    }

    template<>
    [[nodiscard]] SIMD Shuffle<0, 1, 0, 1>(const SIMD& _Data)const noexcept
    {
      return ::_mm_movelh_ps(Data_, _Data);
    }

    template<>
    [[nodiscard]] SIMD Shuffle<2, 3, 2, 3>(const SIMD& _Data)const noexcept
    {
      return ::_mm_movehl_ps(Data_, _Data);
    }
  public:
    [[nodiscard]] float GetX()const noexcept
    {
      return ::_mm_cvtss_f32(Data_);
    }

    [[nodiscard]] float GetY()const noexcept
    {
      static constexpr int kShuffleMask = _MM_SHUFFLE(1, 1, 1, 1);
      return ::_mm_cvtss_f32(::_mm_permute_ps(Data_, kShuffleMask));
    }

    [[nodiscard]] float GetZ()const noexcept
    {
      static constexpr int kShuffleMask = _MM_SHUFFLE(2, 2, 2, 2);
      return ::_mm_cvtss_f32(::_mm_permute_ps(Data_, kShuffleMask));
    }

    [[nodiscard]] float GetW()const noexcept
    {
      static constexpr int kShuffleMask = _MM_SHUFFLE(3, 3, 3, 3);
      return ::_mm_cvtss_f32(::_mm_permute_ps(Data_, kShuffleMask));
    }

    void SetX(float _x)noexcept
    {
      Data_ = ::_mm_move_ss(Data_, ::_mm_set_ss(_x));
    }

    void SetY(float _y)noexcept
    {
      static constexpr int kShuffleMask = _MM_SHUFFLE(3, 2, 0, 1);
      Data_ = ::_mm_permute_ps(::_mm_move_ss(::_mm_permute_ps(Data_, kShuffleMask), ::_mm_set_ss(_y)), kShuffleMask);
    }

    void SetZ(float _z)noexcept
    {
      static constexpr int kShuffleMask = _MM_SHUFFLE(3, 1, 0, 2);
      Data_ = ::_mm_permute_ps(::_mm_move_ss(::_mm_permute_ps(Data_, kShuffleMask), ::_mm_set_ss(_z)), kShuffleMask);
    }

    void SetW(float _w)noexcept
    {
      static constexpr int kShuffleMask = _MM_SHUFFLE(2, 1, 0, 3);
      Data_ = ::_mm_permute_ps(::_mm_move_ss(::_mm_permute_ps(Data_, kShuffleMask), ::_mm_set_ss(_w)), kShuffleMask);
    }
  public:
    [[nodiscard]] SIMD Length()const noexcept
    {
      return ::_mm_sqrt_ps(LengthSq());
    }

    [[nodiscard]] SIMD LengthSq()const noexcept
    {
      __m128 Temp = ::_mm_mul_ps(Data_, Data_);
      Temp = ::_mm_hadd_ps(Temp, Temp);
      Temp = ::_mm_hadd_ps(Temp, Temp);

      static constexpr int kShuffleMask = _MM_SHUFFLE(0, 0, 0, 0);
      return ::_mm_permute_ps(Temp, kShuffleMask);
    }

    [[nodiscard]] SIMD Normalize()const noexcept
    {
      return *this / Length();
    }

    [[nodiscard]] SIMD Dot(const SIMD& _Data)const noexcept
    {
      __m128 Temp = ::_mm_mul_ps(Data_, _Data.Data_);
      Temp = ::_mm_hadd_ps(Temp, Temp);
      Temp = ::_mm_hadd_ps(Temp, Temp);

      static constexpr int kShuffleMask = _MM_SHUFFLE(0, 0, 0, 0);
      return ::_mm_permute_ps(Temp, kShuffleMask);
    }
  public:
    [[nodiscard]] static SIMD Zero()noexcept { return ::_mm_setzero_ps(); }
  };

  template<>
  template<>
#pragma warning(disable:26495)
  inline Type4<float>::Type4(const SIMD<float>& _v)noexcept
#pragma warning(default:26495)
  {
    ::_mm_store_ps(&x, _v);
  }

  template<>
  template<>
  inline Type4<float>& Type4<float>::operator=(const SIMD<float>& _v)noexcept
  {
    ::_mm_store_ps(&x, _v);

    return *this;
  }

  //----------------------------------------------------------------------------------------------------

  template<>
  class alignas(16) SIMD<double>
  {
    __m128d Data_;
  public:
    SIMD() = default;

    SIMD(const SIMD&) = default;

    SIMD(SIMD&&) = default;

    ~SIMD() = default;

    SIMD(const __m128d& _Data)noexcept
      : Data_(_Data) {}

    SIMD(double _x, double _y)noexcept
    {
      Data_ = ::_mm_set_pd(_y, _x);
    }

    SIMD(double _xy)noexcept
      : SIMD(_xy, _xy) {}

    SIMD(const double2& _vec)noexcept
    {
      Data_ = ::_mm_load_pd(&_vec.x);
    }
  public:
    [[nodiscard]] operator __m128d()const noexcept { return Data_; }
  public:
    SIMD& operator=(const SIMD&) = default;

    SIMD& operator=(SIMD&&) = default;

    [[nodiscard]] SIMD operator+()const noexcept { return *this; }

    [[nodiscard]] SIMD operator-()const noexcept { return *this * SIMD(-1.0); }

    [[nodiscard]] SIMD operator+(const SIMD& _Simd)const noexcept { return ::_mm_add_pd(Data_, _Simd.Data_); }

    [[nodiscard]] SIMD operator+(double _s)const noexcept { return *this + SIMD(_s); }

    [[nodiscard]] SIMD operator-(const SIMD& _Simd)const noexcept { return ::_mm_sub_pd(Data_, _Simd.Data_); }

    [[nodiscard]] SIMD operator-(double _s)const noexcept { return *this - SIMD(_s); }

    [[nodiscard]] SIMD operator*(const SIMD& _Simd)const noexcept { return ::_mm_mul_pd(Data_, _Simd.Data_); }

    [[nodiscard]] SIMD operator*(double _s)const noexcept { return *this * SIMD(_s); }

    [[nodiscard]] SIMD operator/(const SIMD& _Simd)const noexcept { return ::_mm_div_pd(Data_, _Simd.Data_); }

    [[nodiscard]] SIMD operator/(double _s)const noexcept { return *this / SIMD(_s); }

    SIMD& operator+=(const SIMD& _Simd)noexcept { Data_ = ::_mm_add_pd(Data_, _Simd.Data_); return *this; }

    SIMD& operator+=(double _s)noexcept { *this += SIMD(_s); return *this; }

    SIMD& operator-=(const SIMD& _Simd)noexcept { Data_ = ::_mm_sub_pd(Data_, _Simd.Data_); return *this; }

    SIMD& operator-=(double _s)noexcept { *this -= SIMD(_s); return *this; }

    SIMD& operator*=(const SIMD& _Simd)noexcept { Data_ = ::_mm_mul_pd(Data_, _Simd.Data_); return *this; }

    SIMD& operator*=(double _s)noexcept { *this *= SIMD(_s); return *this; }

    SIMD& operator/=(const SIMD& _Simd)noexcept { Data_ = ::_mm_div_pd(Data_, _Simd.Data_); return *this; }

    SIMD& operator/=(double _s)noexcept { *this /= SIMD(_s); return *this; }
  public:
    template<uint _x, uint _y,
      std::enable_if_t < _x < 2 && _y < 2, std::nullptr_t> = nullptr>
      [[nodiscard]] SIMD Swizzle()const noexcept
    {
      static constexpr int kShuffleMask = _MM_SHUFFLE2(_y, _x);
      return ::_mm_permute_pd(Data_, kShuffleMask);
    }

    template<>
    [[nodiscard]] SIMD Swizzle<0, 0>()const noexcept
    {
      return ::_mm_movedup_pd(Data_);
    }

    template<uint _x, uint _y,
      std::enable_if_t < _x < 2 && _y < 2, std::nullptr_t> = nullptr>
      [[nodiscard]] SIMD Shuffle(const SIMD& _Data)const noexcept
    {
      static constexpr int kShuffleMask = _MM_SHUFFLE2(_y, _x);
      return ::_mm_shuffle_pd(Data_, _Data, kShuffleMask);
    }
  public:
    [[nodiscard]] double GetX()const noexcept
    {
      return ::_mm_cvtsd_f64(Data_);
    }

    [[nodiscard]] double GetY()const noexcept
    {
      static constexpr int kShuffleMask = _MM_SHUFFLE2(1, 1);
      return ::_mm_cvtsd_f64(::_mm_permute_pd(Data_, kShuffleMask));
    }

    void SetX(double _x)noexcept
    {
      Data_ = ::_mm_move_sd(Data_, ::_mm_set_sd(_x));
    }

    void SetY(double _y) noexcept
    {
      static constexpr int kShuffleMask = _MM_SHUFFLE2(0, 1);
      Data_ = ::_mm_permute_pd(::_mm_move_sd(::_mm_permute_pd(Data_, kShuffleMask), ::_mm_set_sd(_y)), kShuffleMask);
    }
  public:
    [[nodiscard]] static SIMD Zero()noexcept { return ::_mm_setzero_pd(); }
  };
}
