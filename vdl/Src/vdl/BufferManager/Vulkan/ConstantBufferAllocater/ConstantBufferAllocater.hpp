#pragma once
#include <vdl/Buffer/Vulkan/CBuffer.hpp>

#include <memory>
#include <vector>


class ConstantBufferAllocater
{
  struct MemorySpace
  {
    vdl::uint Offset;
    vdl::uint BufferSize;
  public:
    MemorySpace() = default;

    MemorySpace(vdl::uint _Offset, vdl::uint _BufferSize)noexcept
      : Offset(_Offset), BufferSize(_BufferSize) {}

    [[nodiscard]] bool operator<(const MemorySpace& _MemorySpace)const noexcept { return Offset < _MemorySpace.Offset; }
  };
private:
  std::unique_ptr<CConstantBuffer> pConstantBuffer_;
  std::vector<MemorySpace> MemorySpaces_;
public:
  ConstantBufferAllocater() = default;

  void Initialize(vdl::uint _BufferSize);

  [[nodiscard]] void* GetBuffer(vdl::uint _Offset)const { return static_cast<vdl::uint8_t*>(pConstantBuffer_->BufferData.pData) + _Offset; }

  [[nodiscard]] const vk::Buffer& GetVkBuffer()const { return pConstantBuffer_->BufferData.Buffer.get(); }

  [[nodiscard]] vdl::uint Secure(vdl::uint _BufferSize);

  void Release(vdl::uint _Offset, vdl::uint _BufferSize);
};
