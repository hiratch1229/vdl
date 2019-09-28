#pragma once
#include "ID.hpp"
#include "Types.hpp"

namespace vdl
{
  namespace Detail
  {
    class ConstantBufferData
    {
      ID ID_;
    public:
      ConstantBufferData() = default;

      ConstantBufferData(uint _BufferSize);

      ConstantBufferData(const ConstantBufferData& _ConstantBufferData);

      ConstantBufferData(ConstantBufferData&& _ConstantBufferData)noexcept;

      ~ConstantBufferData();
    public:
      ConstantBufferData& operator=(const ConstantBufferData& _ConstantBufferData);
      
      ConstantBufferData& operator=(ConstantBufferData&& _ConstantBufferData)noexcept;

      [[nodiscard]] constexpr bool operator==(const ConstantBufferData& _ConstantBufferData)const noexcept { return ID_ == _ConstantBufferData.ID_; }

      [[nodiscard]] constexpr bool operator!=(const ConstantBufferData& _ConstantBufferData)const noexcept { return ID_ != _ConstantBufferData.ID_; }
    public:
      [[nodiscard]] ID GetID()const noexcept { return ID_; }

      [[nodiscard]] void* GetData()const;

      [[nodiscard]] uint GetSize()const;
    };
  }

  template<class T>
  class ConstantBuffer
  {
    static constexpr uint kBufferSize = sizeof(T);
    static_assert(kBufferSize % 16 == 0);
  private:
    Detail::ConstantBufferData Data_;
  public:
    ConstantBuffer()
      : Data_(kBufferSize) {}

    ConstantBuffer(const T& _Data)
      : ConstantBuffer()
    {
      GetData() = _Data;
    }
  public:
    [[nodiscard]] bool operator==(const ConstantBuffer& _ConstantBuffer)const noexcept { return Data_ == _ConstantBuffer.Data_; }

    [[nodiscard]] bool operator!=(const ConstantBuffer& _ConstantBuffer)const noexcept { return Data_ != _ConstantBuffer.Data_; }
  public:
    [[nodiscard]] T& GetData() { return *reinterpret_cast<T*>(Data_.GetData()); }

    [[nodiscard]] const void* GetPtr()const { return Data_.GetData(); }

    [[nodiscard]] Detail::ConstantBufferData GetDetail()const { return Data_; }
  };
}
