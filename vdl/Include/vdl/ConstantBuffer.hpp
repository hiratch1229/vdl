#pragma once
#include "Fwd.hpp"
#include "Resource.hpp"

namespace vdl
{
  namespace Detail
  {
    class ConstantBufferData : public Resource
    {
    public:
      ConstantBufferData() = default;

      ConstantBufferData(uint _BufferSize);

      ConstantBufferData(const ConstantBufferData& _ConstantBufferData);

      ConstantBufferData(ConstantBufferData&& _ConstantBufferData)noexcept;

      ~ConstantBufferData();
    public:
      ConstantBufferData& operator=(const ConstantBufferData& _ConstantBufferData);

      ConstantBufferData& operator=(ConstantBufferData&& _ConstantBufferData)noexcept;

      [[nodiscard]] bool operator==(const ConstantBufferData& _ConstantBufferData)const noexcept { return ID_ == _ConstantBufferData.ID_; }

      [[nodiscard]] bool operator!=(const ConstantBufferData& _ConstantBufferData)const noexcept { return ID_ != _ConstantBufferData.ID_; }
    public:
      [[nodiscard]] void* GetData()const;

      [[nodiscard]] uint GetBufferSize()const;
    };
  }

  template<class T, std::enable_if_t<sizeof(T) % 16 == 0, std::nullptr_t>>
  class ConstantBuffer
  {
    static constexpr uint kBufferSize = sizeof(T);
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
    [[nodiscard]] T& GetData()const { return *reinterpret_cast<T*>(Data_.GetData()); }

    [[nodiscard]] const Detail::ConstantBufferData& GetDetail()const { return Data_; }
  };
}
