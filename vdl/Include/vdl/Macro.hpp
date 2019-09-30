#pragma once
#include <cstddef>

namespace vdl::Macro
{
  //  ポインタ解放
  template<class T>
  inline void SafeDelete(T*& _p)
  {
    if (_p)
    {
      delete _p;
      _p = nullptr;
    }
  }

  //	ポインタ配列解放
  template<class T>
  inline void SafeDeleteArray(T*& _p)
  {
    if (_p)
    {
      delete[] _p;
      _p = nullptr;
    }
  }

  //  配列サイズを取得
  template<class T, std::size_t Size>
  inline constexpr size_t ArraySize(const T(&)[Size])
  {
    return Size;
  }

  //  データの入れ替え
  template<class T>
  inline void Swap(T& _Obj1, T& _Obj2)
  {
    T Temp = _Obj1;
    _Obj1 = _Obj2;
    _Obj2 = Temp;
  }
}
