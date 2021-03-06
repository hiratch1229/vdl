#pragma once
#include <vdl/Types.hpp>
#include <vdl/Macro.hpp>

#include <shared_mutex>
#include <vector>

template<class T>
class ReferenceCount
{
  T* Ptr_;
  vdl::uint Count_ = 0;
  std::shared_mutex Mutex_;
private:
  void Swap(ReferenceCount& _Other)
  {
    vdl::Macro::Swap(Ptr_, _Other.Ptr_);
    vdl::Macro::Swap(Count_, _Other.Count_);
  }
public:
  ReferenceCount()noexcept
    : Ptr_(nullptr), Count_(0) {}

  ReferenceCount(decltype(nullptr))noexcept
    : Ptr_(nullptr), Count_(0) {}

  ReferenceCount(T* _Other)noexcept
    : Ptr_(_Other)
  {
    AddRef();
  }

  ReferenceCount(const ReferenceCount& _Other)noexcept
    : Ptr_(_Other.Ptr_)
  {
    AddRef();
  }

  ReferenceCount(ReferenceCount&& _Other)noexcept
    : Ptr_(nullptr)
  {
    if (this != reinterpret_cast<ReferenceCount*>(&_Other))
    {
      Swap(_Other);
    }
  }

  ~ReferenceCount()
  {
    if (Count_ > 0 && --Count_ == 0)
    {
      vdl::Macro::SafeDelete(Ptr_);
    }
  }
public:
  operator bool()const { return Count_; }

  bool operator!()const { return !Count_; }

  T* operator->()const { return Ptr_; }
public:
  ReferenceCount& operator=(decltype(nullptr))
  {
    Release();

    return *this;
  }

  ReferenceCount& operator=(T* _Other)
  {
    if (Ptr_ != _Other)
    {
      ReferenceCount(_Other).Swap(*this);
    }

    return *this;
  }

  ReferenceCount& operator=(const ReferenceCount& _Other)
  {
    if (Ptr_ != _Other.Ptr_)
    {
      ReferenceCount(_Other).Swap(*this);
    }

    return *this;
  }

  ReferenceCount& operator=(ReferenceCount&& _Other)noexcept
  {
    ReferenceCount(std::move(_Other)).Swap(*this);

    return *this;
  }
public:
  T* Get()
  {
    std::shared_lock Lock(Mutex_);
    return Ptr_;
  }

  void AddRef()
  {
    std::lock_guard Lock(Mutex_);

    if (Ptr_)
    {
      ++Count_;
    }
  }

  void Release()
  {
    std::lock_guard Lock(Mutex_);

    if (Count_ > 0 && --Count_ == 0)
    {
      vdl::Macro::SafeDelete(Ptr_);
    }
  }
};

template<class T>
class ReferenceCounts
{
  using Type = ReferenceCount<T>;
private:
  std::vector<Type> ReferenceCounts_;
  std::shared_mutex Mutex_;
public:
  Type& Get(vdl::ID _ID)
  {
    std::shared_lock Lock(Mutex_);
    return ReferenceCounts_[_ID];
  }
public:
  ReferenceCounts() = default;

  ~ReferenceCounts() = default;

  vdl::ID Add(Type _Obj)
  {
    vdl::uint Index = 0;
    {
      std::lock_guard Lock(Mutex_);

      const size_t Size = ReferenceCounts_.size();
      for (; Index < Size; ++Index)
      {
        if (!ReferenceCounts_[Index])
        {
          break;
        }
      }

      if (Size == Index)
      {
        ReferenceCounts_.resize(Size + 1);
      }
    }

    ReferenceCounts_[Index] = std::move(_Obj);

    return Index;
  }
};
