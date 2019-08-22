#pragma once

namespace vdl
{
  //  �f�U�C���p�^�[��Singleton
  //  �p�����Ďg�p
  template <class T>
  class Singleton
  {
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
  protected:
    Singleton() = default;
    virtual ~Singleton() = default;
  public:
    static T* Get()
    {
      static T Instance;
      return &Instance;
    }
  };
}
