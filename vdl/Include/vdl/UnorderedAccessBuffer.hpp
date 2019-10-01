#pragma once
#include "Types.hpp"
#include "ID.hpp"

//namespace vdl
//{
//  namespace detail
//  {
//    class UnorderedAccessBufferData
//    {
//      ID ID_;
//    public:
//      [[nodiscard]] ID GetID()const noexcept { return ID_; }
//      [[nodiscard]] void* GetDatas();
//      [[nodiscard]] uint GetSize()const;
//    public:
//      void Resize(uint _Size);
//    public:
//      UnorderedAccessBufferData() = default;
//
//      UnorderedAccessBufferData(uint _Size);
//
//      ~UnorderedAccessBufferData();
//    };
//  }
//
//  template<class T>
//  class UnorderedAccessBuffer
//  {
//    static constexpr uint kSize = sizeof(T);
//    static_assert(kSize % 16 == 0);
//  private:
//    detail::UnorderedAccessBufferData Data_;
//  public:
//    [[nodiscard]] ID GetID()const noexcept { return Base_.GetID(); }
//    [[nodiscard]] T* GetDatas() { return reinterpret_cast<T*>(Data_.GetDatas()); }
//    [[nodiscard]] const void* GetPtr()const { return Data_.GetData(); }
//    [[nodiscard]] uint GetSize()const { return Data_.GetSize(); }
//  public:
//    UnorderedAccessBuffer() = default;
//
//    UnorderedAccessBuffer(uint _Size)
//      : Data_(kSize*_Size) {}
//
//    UnorderedAccessBuffer(T* _Datas, uint _Size)
//      : Data_(kSize*_Size) 
//    {
//      ::memcpy(GetDatas(), _Datas, kSize * _Size);
//    }
//  };
//}
