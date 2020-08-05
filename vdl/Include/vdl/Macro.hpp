#pragma once
#include <cstddef>

namespace vdl::Macro
{
  //  �|�C���^���
  template<class T>
  inline void SafeDelete(T*& _p)
  {
    if (_p)
    {
      delete _p;
      _p = nullptr;
    }
  }

  //	�|�C���^�z����
  template<class T>
  inline void SafeDeleteArray(T*& _p)
  {
    if (_p)
    {
      delete[] _p;
      _p = nullptr;
    }
  }

  //  �f�[�^�̓���ւ�
  template<class T>
  inline void Swap(T& _Obj1, T& _Obj2)
  {
    if constexpr (std::is_move_assignable<T>::value)
    {
      T Temp = std::move(_Obj1);
      _Obj1 = std::move(_Obj2);
      _Obj2 = std::move(Temp);
    }
    else
    {
      T Temp = _Obj1;
      _Obj1 = _Obj2;
      _Obj2 = Temp;
    }
  }

  //  �z��T�C�Y���擾
  template<class T, std::size_t Size>
  [[nodiscard]] inline constexpr size_t ArraySize(const T(&)[Size])noexcept
  {
    return Size;
  }
}
