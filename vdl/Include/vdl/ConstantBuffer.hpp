#pragma once
#include "ID.hpp"
#include "Types.hpp"

namespace vdl
{
  namespace detail
  {
    class ConstantBufferData
    {
      ID ID_;
    public:
      [[nodiscard]] ID GetID()const noexcept { return ID_; }
      [[nodiscard]] void* GetData();
    public:
      ConstantBufferData() = default;

      ConstantBufferData(uint _BufferSize);

      ConstantBufferData(const ConstantBufferData& _ConstantBufferData);

      ~ConstantBufferData();

      ConstantBufferData& operator=(const ConstantBufferData& _ConstantBufferData);
    };
  }

  template<class T>
  class ConstantBuffer
  {
    static constexpr uint kBufferSize = sizeof(T);
    static_assert(kBufferSize % 16 == 0);
  private:
    detail::ConstantBufferData Data_;
  public:
    [[nodiscard]] T& GetData() { return *reinterpret_cast<T*>(Data_.GetData()); }
    [[nodiscard]] const void* GetPtr()const { return Data_.GetData(); }
    [[nodiscard]] detail::ConstantBufferData GetDetail()const { return Data_; }
  public:
    ConstantBuffer()
      : Data_(kBufferSize) {}

    ConstantBuffer(const T& _Data)
      : ConstantBuffer()
    {
      GetData() = _Data;
    }
  };
}
