#pragma once
#include "Fwd.hpp"

namespace vdl
{
  namespace Detail
  {
    class UnorderedAccessBufferData
    {
      ID ID_;
    public:
      UnorderedAccessBufferData() = default;

      UnorderedAccessBufferData(uint _Stride, uint _BufferSize, void* _Buffer);

      UnorderedAccessBufferData(const UnorderedAccessBufferData& _UnorderedAccessBufferData);

      UnorderedAccessBufferData(UnorderedAccessBufferData&& _UnorderedAccessBufferData)noexcept;

      ~UnorderedAccessBufferData();
    public:
      UnorderedAccessBufferData& operator=(const UnorderedAccessBufferData& _UnorderedAccessBufferData);

      UnorderedAccessBufferData& operator=(UnorderedAccessBufferData&& _UnorderedAccessBufferData)noexcept;

      [[nodiscard]] bool operator==(const UnorderedAccessBufferData& _UnorderedAccessBufferData)const noexcept { return ID_ == _UnorderedAccessBufferData.ID_; }

      [[nodiscard]] bool operator!=(const UnorderedAccessBufferData& _UnorderedAccessBufferData)const noexcept { return ID_ != _UnorderedAccessBufferData.ID_; }
    public:
      [[nodiscard]] ID GetID()const noexcept { return ID_; }

      [[nodiscard]] bool isEmpty()const noexcept { return ID_ == std::nullopt; }
    };
  }

  template<class T>
  class UnorderedAccessBuffer
  {
    static constexpr uint kSize = sizeof(T);
    static_assert(kSize % 16 == 0);
  private:
    Detail::UnorderedAccessBufferData Data_;
  public:
    UnorderedAccessBuffer() = default;

    UnorderedAccessBuffer(uint _Size)
      : Data_(kSize, kSize*_Size, nullptr) {}

    UnorderedAccessBuffer(uint _Size, T* _InitData)
      : Data_(kSize, kSize*_Size, _InitData) {}
  public:
    [[nodiscard]] bool operator==(const UnorderedAccessBuffer& _UnorderedAccessBuffer)const noexcept { return Data_ == _UnorderedAccessBuffer.Data_; }

    [[nodiscard]] bool operator!=(const UnorderedAccessBuffer& _UnorderedAccessBuffer)const noexcept { return Data_ != _UnorderedAccessBuffer.Data_; }
  public:
    [[nodiscard]] const Detail::UnorderedAccessBufferData& GetDetail()const { return Data_; }
  };
}
