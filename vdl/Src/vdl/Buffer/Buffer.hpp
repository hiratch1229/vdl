#pragma once
#include <vdl/Fwd.hpp>

enum class IndexType : vdl::uint8_t
{
  eUint16,
  eUint32,
};

class VertexBuffer
{
  vdl::ID ID_;
public:
  VertexBuffer() = default;

  VertexBuffer(vdl::uint _BufferSize);

  VertexBuffer(const void* _Vertices, vdl::uint _BufferSize);

  VertexBuffer(const VertexBuffer& _VertexBuffer);

  VertexBuffer(VertexBuffer&& _VertexBuffer)noexcept;

  ~VertexBuffer();
public:
  VertexBuffer& operator=(const VertexBuffer& _VertexBuffer);

  VertexBuffer& operator=(VertexBuffer&& _VertexBuffer)noexcept;

  [[nodiscard]] constexpr bool operator==(const VertexBuffer& _VertexBuffer)const noexcept { return ID_ == _VertexBuffer.ID_; }

  [[nodiscard]] constexpr bool operator!=(const VertexBuffer& _VertexBuffer)const noexcept { return ID_ != _VertexBuffer.ID_; }
public:
  [[nodiscard]] vdl::ID GetID()const noexcept { return ID_; }

  [[nodiscard]] bool isEmpty()const noexcept { return ID_ == std::nullopt; }
};

class InstanceBuffer
{
  vdl::ID ID_;
public:
  InstanceBuffer() = default;

  InstanceBuffer(vdl::uint _BufferSize);

  InstanceBuffer(const InstanceBuffer& _InstanceBuffer);

  InstanceBuffer(InstanceBuffer&& _InstanceBuffer)noexcept;

  ~InstanceBuffer();
public:
  InstanceBuffer& operator=(const InstanceBuffer& _InstanceBuffer);

  InstanceBuffer& operator=(InstanceBuffer&& _InstanceBuffer)noexcept;

  [[nodiscard]] constexpr bool operator==(const InstanceBuffer& _InstanceBuffer)const noexcept { return ID_ == _InstanceBuffer.ID_; }

  [[nodiscard]] constexpr bool operator!=(const InstanceBuffer& _InstanceBuffer)const noexcept { return ID_ != _InstanceBuffer.ID_; }
public:
  [[nodiscard]] vdl::ID GetID()const noexcept { return ID_; }

  [[nodiscard]] bool isEmpty()const noexcept { return ID_ == std::nullopt; }
};

class IndexBuffer
{
  vdl::ID ID_;
public:
  IndexBuffer() = default;

  IndexBuffer(vdl::uint _BufferSize, IndexType _IndexType);

  IndexBuffer(const void* _Indices, vdl::uint _BufferSize, IndexType _IndexType);

  IndexBuffer(const IndexBuffer& _IndexBuffer);

  IndexBuffer(IndexBuffer&& _IndexBuffer)noexcept;

  ~IndexBuffer();
public:
  IndexBuffer& operator=(const IndexBuffer& _IndexBuffer);

  IndexBuffer& operator=(IndexBuffer&& _IndexBuffer)noexcept;

  [[nodiscard]] constexpr bool operator==(const IndexBuffer& _IndexBuffer)const noexcept { return ID_ == _IndexBuffer.ID_; }

  [[nodiscard]] constexpr bool operator!=(const IndexBuffer& _IndexBuffer)const noexcept { return ID_ != _IndexBuffer.ID_; }
public:
  [[nodiscard]] vdl::ID GetID()const noexcept { return ID_; }

  [[nodiscard]] bool isEmpty()const noexcept { return ID_ == std::nullopt; }
};
