#pragma once
#include "Fwd.hpp"
#include "Constants.hpp"
#include <type_traits>

namespace vdl
{
  namespace Detail
  {
    //  領域確保オブジェクト
    template<class...>
    struct VariantStorage;

    template<class T, class... Arg>
    struct VariantStorage<T, Arg...> : VariantStorage<Arg...>
    {
      using Base = VariantStorage<Arg...>;
      static constexpr uint kSize = sizeof(T) > Base::kSize ? sizeof(T) : Base::kSize;
      static constexpr uint kAlignment = alignof(T) > Base::kAlignment ? alignof(T) : Base::kAlignment;
    };

    template<class T>
    struct VariantStorage<T>
    {
      static constexpr uint kSize = sizeof(T);
      static constexpr uint kAlignment = alignof(T);
    };

    //  識別IDオブジェクト
    template<class, ID, class...>
    struct VariantTypeID;

    template<class Target, ID MaxID, class Head, class... Tail>
    struct VariantTypeID<Target, MaxID, Head, Tail...> : VariantTypeID<Target, MaxID, Tail...>
    {

    };

    template<class Target, ID MaxID, class... Arg>
    struct VariantTypeID<Target, MaxID, Target, Arg...>
    {
      static constexpr ID kID = MaxID - sizeof...(Arg);

      template<class T>
      static ID Assign(void* _Buffer, T&& _Obj)
      {
        ::new(_Buffer) Target(std::forward<T>(_Obj));

        return kID;
      }
    };

    //  暗黙変換可能識別IDオブジェクト
    template<class, ID, class...>
    struct VariantConstructibleTypeID;

    template<class Target, ID MaxID, class Head, class... Tail>
    struct VariantConstructibleTypeID<Target, MaxID, Head, Tail...>
    {
      static constexpr ID kID = std::is_constructible<Head, Target>::value
        ? MaxID - sizeof...(Tail) : VariantConstructibleTypeID<Target, MaxID, Tail...>::kID;
    };

    template<class Target, ID MaxID>
    struct VariantConstructibleTypeID<Target, MaxID>
    {
      static constexpr ID kID = Constants::kDisableID;
    };

    //  Indexから型を検索オブジェクト
    template<ID, class...>
    struct VariantIndexType;

    template<ID Index, class T, class... Arg>
    struct VariantIndexType<Index, T, Arg...> : VariantIndexType<Index - 1, Arg...>
    {

    };

    template<class T, class... Arg>
    struct VariantIndexType<0, T, Arg...>
    {
      using Type = T;
    };

    //  Indexから適切な関数呼び出しオブジェクト
    //  TODO:コンパイル時展開
    template<class...>
    struct VariantVisiter;

    template<class Head, class... Tail>
    struct VariantVisiter<Head, Tail...>
    {
      template<class Visitor, class... Arg>
      static auto Visit(ID _Index, void* _Buffer, Visitor&& _Visiter, Arg&&... _Arg)
      {
        if (_Index == 0)
        {
          return std::invoke(_Visiter, *reinterpret_cast<Head*>(_Buffer), _Arg...);
        }
        else
        {
          return VariantVisiter<Tail...>::Visit(_Index - 1, _Buffer, _Visiter);
        }
      }

      static void Copy(ID _Index, void* _Buffer0, const void* _Buffer1)
      {
        if (_Index == 0)
        {
          ::new(_Buffer0) Head(*reinterpret_cast<const Head*>(_Buffer1));
        }
        else
        {
          VariantVisiter<Tail...>::Copy(_Index - 1, _Buffer0, _Buffer1);
        }
      }

      static void Move(ID _Index, void* _Buffer0, void* _Buffer1)
      {
        if (_Index == 0)
        {
          ::new(_Buffer0) Head(std::move(*reinterpret_cast<Head*>(_Buffer1)));
        }
        else
        {
          VariantVisiter<Tail...>::Move(_Index - 1, _Buffer0, _Buffer1);
        }
      }

      static bool Equal(ID _Index, const void* _Buffer0, const void* _Buffer1)
      {
        if (_Index == 0)
        {
          return *reinterpret_cast<const Head*>(_Buffer0) == *reinterpret_cast<const Head*>(_Buffer1);
        }
        else
        {
          return VariantVisiter<Tail...>::Equal(_Index - 1, _Buffer0, _Buffer1);
        }
      }

      static bool NotEqual(ID _Index, const void* _Buffer0, const void* _Buffer1)
      {
        if (_Index == 0)
        {
          return *reinterpret_cast<const Head*>(_Buffer0) != *reinterpret_cast<const Head*>(_Buffer1);
        }
        else
        {
          return VariantVisiter<Tail...>::NotEqual(_Index - 1, _Buffer0, _Buffer1);
        }
      }

      static bool Less(ID _Index, const void* _Buffer0, const void* _Buffer1)
      {
        if (_Index == 0)
        {
          return *reinterpret_cast<const Head*>(_Buffer0) < *reinterpret_cast<const Head*>(_Buffer1);
        }
        else
        {
          return VariantVisiter<Tail...>::Less(_Index - 1, _Buffer0, _Buffer1);
        }
      }

      static bool LessEqual(ID _Index, const void* _Buffer0, const void* _Buffer1)
      {
        if (_Index == 0)
        {
          return *reinterpret_cast<const Head*>(_Buffer0) < *reinterpret_cast<const Head*>(_Buffer1);
        }
        else
        {
          return VariantVisiter<Tail...>::LessEqual(_Index - 1, _Buffer0, _Buffer1);
        }
      }

      static bool Greater(ID _Index, const void* _Buffer0, const void* _Buffer1)
      {
        if (_Index == 0)
        {
          return *reinterpret_cast<const Head*>(_Buffer0) > *reinterpret_cast<const Head*>(_Buffer1);
        }
        else
        {
          return VariantVisiter<Tail...>::Greater(_Index - 1, _Buffer0, _Buffer1);
        }
      }

      static bool GreaterEqual(ID _Index, const void* _Buffer0, const void* _Buffer1)
      {
        if (_Index == 0)
        {
          return *reinterpret_cast<const Head*>(_Buffer0) > *reinterpret_cast<const Head*>(_Buffer1);
        }
        else
        {
          return VariantVisiter<Tail...>::GreaterEqual(_Index - 1, _Buffer0, _Buffer1);
        }
      }
    };

    template<class T>
    struct VariantVisiter<T>
    {
      template<class Visitor, class... Args>
      static auto Visit(ID _Index, void* _Buffer, Visitor&& _Visiter, Args&&... _Args)
      {
        if (_Index == 0)
        {
          return std::invoke(_Visiter, *reinterpret_cast<T*>(_Buffer), _Args...);
        }

        throw std::runtime_error("bad variant access\n");
      }

      static void Copy(ID _Index, void* _Buffer0, const void* _Buffer1)
      {
        if (_Index == 0)
        {
          ::new(_Buffer0) T(*reinterpret_cast<const T*>(_Buffer1));
        }
        else
        {
          throw std::runtime_error("bad variant access\n");
        }
      }

      static void Move(ID _Index, void* _Buffer0, void* _Buffer1)
      {
        if (_Index == 0)
        {
          ::new(_Buffer0) T(std::move(*reinterpret_cast<T*>(_Buffer1)));
        }
        else
        {
          throw std::runtime_error("bad variant access\n");
        }
      }

      static bool Equal(ID _Index, const void* _Buffer0, const void* _Buffer1)
      {
        if (_Index == 0)
        {
          return *reinterpret_cast<const T*>(_Buffer0) == *reinterpret_cast<const T*>(_Buffer1);
        }

        throw std::runtime_error("bad variant access\n");
      }

      static bool NotEqual(ID _Index, const void* _Buffer0, const void* _Buffer1)
      {
        if (_Index == 0)
        {
          return *reinterpret_cast<const T*>(_Buffer0) != *reinterpret_cast<const T*>(_Buffer1);
        }

        throw std::runtime_error("bad variant access\n");
      }

      static bool Less(ID _Index, const void* _Buffer0, const void* _Buffer1)
      {
        if (_Index == 0)
        {
          return *reinterpret_cast<const T*>(_Buffer0) < *reinterpret_cast<const T*>(_Buffer1);
        }

        throw std::runtime_error("bad variant access\n");
      }

      static bool LessEqual(ID _Index, const void* _Buffer0, const void* _Buffer1)
      {
        if (_Index == 0)
        {
          return *reinterpret_cast<const T*>(_Buffer0) < *reinterpret_cast<const T*>(_Buffer1);
        }

        throw std::runtime_error("bad variant access\n");
      }

      static bool Greater(ID _Index, const void* _Buffer0, const void* _Buffer1)
      {
        if (_Index == 0)
        {
          return *reinterpret_cast<const T*>(_Buffer0) > *reinterpret_cast<const T*>(_Buffer1);
        }

        throw std::runtime_error("bad variant access\n");
      }

      static bool GreaterEqual(ID _Index, const void* _Buffer0, const void* _Buffer1)
      {
        if (_Index == 0)
        {
          return *reinterpret_cast<const T*>(_Buffer0) > *reinterpret_cast<const T*>(_Buffer1);
        }

        throw std::runtime_error("bad variant access\n");
      }
    };
  }

  //  std::variantの代用(g++,clangで使えない為)
  template<class... Arg>
  class Variant
  {
    static_assert(sizeof...(Arg) > 0, "可変長引数が空です。");

    static constexpr ID kMaxID = sizeof...(Arg) - 1;
    using Storage = std::aligned_storage_t<Detail::VariantStorage<Arg...>::kSize, Detail::VariantStorage<Arg...>::kAlignment>;
    template<ID Index> using VariantIndexType = Detail::VariantIndexType<Index, Arg...>;
    template<class Target> using VariantConstructibleTypeID = Detail::VariantConstructibleTypeID<Target, kMaxID, Arg...>;
    template<class Target> using VariantTypeID = Detail::VariantTypeID<typename VariantIndexType<VariantConstructibleTypeID<Target>::kID>::Type, kMaxID, Arg...>;
    template<class T> using OriginalType = typename VariantIndexType<VariantTypeID<T>::kID>::Type;

    using VariantVisiter = Detail::VariantVisiter<Arg...>;
  private:
    struct Deleter
    {
      template<class T> void operator()(T& _Obj) { _Obj.~T(); }
    };
  private:
    ID ID_;
    Storage Storage_;
  public:
    Variant()
      : ID_(VariantTypeID<typename VariantIndexType<0>::Type>::Assign(&Storage_, typename VariantIndexType<0>::Type())) {}

    Variant(const Variant& _Variant)
      : ID_(_Variant.ID_)
    {
      VariantVisiter::Copy(ID_, &Storage_, &_Variant.Storage_);
    }

    Variant(Variant&& _Variant)
      : ID_(_Variant.ID_)
    {
      VariantVisiter::Move(ID_, &Storage_, &_Variant.Storage_);
      _Variant.ID_ = VariantTypeID<typename VariantIndexType<0>::Type>::Assign(&_Variant.Storage_, typename VariantIndexType<0>::Type());
    }

    template<class T, std::enable_if_t<VariantConstructibleTypeID<T>::kID != Constants::kDisableID, std::nullptr_t> = nullptr>
    Variant(T&& _Obj)
      : ID_(VariantTypeID<T>::Assign(&Storage_, std::forward<T>(_Obj))) {}

    ~Variant()
    {
      Visit(Deleter());
    }

    Variant& operator=(const Variant& _Variant)
    {
      Visit(Deleter());

      ID_ = _Variant.ID_;
      VariantVisiter::Copy(ID_, &Storage_, &_Variant.Storage_);

      return *this;
    }

    Variant& operator=(Variant&& _Variant)noexcept
    {
      Visit(Deleter());

      ID_ = _Variant.ID_;
      VariantVisiter::Move(ID_, &Storage_, &_Variant.Storage_);
      _Variant.ID_ = VariantTypeID<typename VariantIndexType<0>::Type>::Assign(&_Variant.Storage_, typename VariantIndexType<0>::Type());

      return *this;
    }

    template<class T, std::enable_if_t<VariantConstructibleTypeID<T>::kID != Constants::kDisableID, std::nullptr_t> = nullptr>
    Variant& operator=(T&& _Obj)
    {
      if (ID_ == VariantTypeID<T>::kID)
      {
        *reinterpret_cast<OriginalType<T>*>(&Storage_) = _Obj;
      }
      else
      {
        Visit(Deleter());
        ID_ = VariantTypeID<T>::Assign(&Storage_, std::forward<T>(_Obj));
      }

      return *this;
    }

    [[nodiscard]] bool operator==(const Variant& _Obj)const { return ID_ == _Obj.ID_ && VariantVisiter::Equal(ID_, &Storage_, &_Obj.Storage_); }

    [[nodiscard]] bool operator!=(const Variant& _Obj)const { return ID_ != _Obj.ID_ || VariantVisiter::NotEqual(ID_, &Storage_, &_Obj.Storage_); }

    [[nodiscard]] bool operator<(const Variant& _Obj)const { return (ID_ != _Obj.ID_ ? ID_ < _Obj.ID_ : VariantVisiter::Less(ID_, &Storage_, &_Obj.Storage_)); }

    [[nodiscard]] bool operator>(const Variant& _Obj)const { return (ID_ != _Obj.ID_ ? ID_ > _Obj.ID_ : VariantVisiter::Greater(ID_, &Storage_, &_Obj.Storage_)); }

    [[nodiscard]] bool operator<=(const Variant& _Obj)const { return (ID_ == _Obj.ID_ ? VariantVisiter::LessEqual(ID_, &Storage_, &_Obj.Storage_) : ID_ < _Obj.ID_); }

    [[nodiscard]] bool operator>=(const Variant& _Obj)const { return (ID_ == _Obj.ID_ ? VariantVisiter::GreaterEqual(ID_, &Storage_, &_Obj.Storage_) : ID_ > _Obj.ID_); }
  public:
    template<class Visitor, class... Arg>
    auto Visit(Visitor&& _Visiter, Arg&&... _Arg)
    {
      return VariantVisiter::Visit(ID_, &Storage_, _Visiter, _Arg...);
    }

    template<ID Index>
    decltype(auto) Get()
    {
      if (Index == ID_)
      {
        return *reinterpret_cast<typename VariantIndexType<Index>::Type*>(&Storage_);
      }

      throw std::runtime_error("bad variant access\n");
    }

    template<ID Index>
    decltype(auto) Get()const
    {
      if (Index == ID_)
      {
        return *reinterpret_cast<typename VariantIndexType<Index>::Type*>(&Storage_);
      }

      throw std::runtime_error("bad variant access\n");
    }

    template<class T>
    decltype(auto) Get()
    {
      return Get<VariantTypeID<T>::kID>();
    }

    template<class T>
    decltype(auto) Get()const
    {
      return Get<VariantTypeID<T>::kID>();
    }

    template<ID Index>
    decltype(auto) GetIf()
    {
      return (Index == ID_ ? reinterpret_cast<typename VariantIndexType<Index>::Type*>(&Storage_) : nullptr);
    }

    template<ID Index>
    decltype(auto) GetIf()const
    {
      return (Index == ID_ ? reinterpret_cast<const typename VariantIndexType<Index>::Type*>(&Storage_) : nullptr);
    }

    template<class T>
    decltype(auto) GetIf()
    {
      return GetIf<VariantTypeID<T>::kID>();
    }

    template<class T>
    decltype(auto) GetIf()const
    {
      return GetIf<VariantTypeID<T>::kID>();
    }
  };
}
