#include "Buffer.hpp"

#include <vdl/Engine.hpp>
#include <vdl/BufferManager/IBufferManager.hpp>
#include <vdl/Constants/Constants.hpp>

VertexBuffer::VertexBuffer(vdl::uint _BufferSize)
{
  ID_ = Engine::Get<IBufferManager>()->CreateVertexBuffer(_BufferSize);
}

VertexBuffer::VertexBuffer(const void* _Vertices, vdl::uint _BufferSize)
{
  ID_ = Engine::Get<IBufferManager>()->CreateVertexBuffer(_Vertices, _BufferSize);
}

VertexBuffer::VertexBuffer(const VertexBuffer& _VertexBuffer)
{
  ID_ = _VertexBuffer.ID_;
  if (!isEmpty())
  {
    Engine::Get<IBufferManager>()->AddRef(ID_);
  }
}

VertexBuffer::VertexBuffer(VertexBuffer&& _VertexBuffer)noexcept
{
  ID_ = _VertexBuffer.ID_;
  _VertexBuffer.ID_ = Constants::kDisableID;
}

VertexBuffer& VertexBuffer::operator=(const VertexBuffer& _VertexBuffer)
{
  if (ID_ != _VertexBuffer.ID_)
  {
    if (!isEmpty())
    {
      Engine::Get<IBufferManager>()->Release(ID_);
    }
    ID_ = _VertexBuffer.ID_;
    if (!isEmpty())
    {
      Engine::Get<IBufferManager>()->AddRef(ID_);
    }
  }

  return *this;
}

VertexBuffer& VertexBuffer::operator=(VertexBuffer&& _VertexBuffer)noexcept
{
  if (!isEmpty())
  {
    Engine::Get<IBufferManager>()->Release(ID_);
  }

  ID_ = _VertexBuffer.ID_;
  _VertexBuffer.ID_ = Constants::kDisableID;

  return *this;
}

VertexBuffer::~VertexBuffer()
{
  if (!isEmpty())
  {
    Engine::Get<IBufferManager>()->Release(ID_);
  }
}

//--------------------------------------------------

InstanceBuffer::InstanceBuffer(vdl::uint _BufferSize)
{
  ID_ = Engine::Get<IBufferManager>()->CreateInstanceBuffer(_BufferSize);
}

InstanceBuffer::InstanceBuffer(const InstanceBuffer& _InstanceBuffer)
{
  ID_ = _InstanceBuffer.ID_;
  if (!isEmpty())
  {
    Engine::Get<IBufferManager>()->AddRef(ID_);
  }
}

InstanceBuffer::InstanceBuffer(InstanceBuffer&& _InstanceBuffer)noexcept
{
  ID_ = _InstanceBuffer.ID_;
  _InstanceBuffer.ID_ = Constants::kDisableID;
}

InstanceBuffer& InstanceBuffer::operator=(const InstanceBuffer& _InstanceBuffer)
{
  if (ID_ != _InstanceBuffer.ID_)
  {
    if (!isEmpty())
    {
      Engine::Get<IBufferManager>()->Release(ID_);
    }
    ID_ = _InstanceBuffer.ID_;
    if (!isEmpty())
    {
      Engine::Get<IBufferManager>()->AddRef(ID_);
    }
  }

  return *this;
}

InstanceBuffer& InstanceBuffer::operator=(InstanceBuffer&& _InstanceBuffer)noexcept
{
  if (!isEmpty())
  {
    Engine::Get<IBufferManager>()->Release(ID_);
  }

  ID_ = _InstanceBuffer.ID_;
  _InstanceBuffer.ID_ = Constants::kDisableID;

  return *this;
}

InstanceBuffer::~InstanceBuffer()
{
  if (!isEmpty())
  {
    Engine::Get<IBufferManager>()->Release(ID_);
  }
}

//--------------------------------------------------

IndexBuffer::IndexBuffer(vdl::uint _BufferSize, IndexType _IndexType)
{
  ID_ = Engine::Get<IBufferManager>()->CreateIndexBuffer(_BufferSize, _IndexType);
}

IndexBuffer::IndexBuffer(const void* _Indices, vdl::uint _BufferSize, IndexType _IndexType)
{
  ID_ = Engine::Get<IBufferManager>()->CreateIndexBuffer(_Indices, _BufferSize, _IndexType);
}

IndexBuffer::IndexBuffer(const IndexBuffer& _IndexBuffer)
{
  ID_ = _IndexBuffer.ID_;
  if (!isEmpty())
  {
    Engine::Get<IBufferManager>()->AddRef(ID_);
  }
}

IndexBuffer::IndexBuffer(IndexBuffer&& _IndexBuffer)noexcept
{
  ID_ = _IndexBuffer.ID_;
  _IndexBuffer.ID_ = Constants::kDisableID;
}

IndexBuffer& IndexBuffer::operator=(const IndexBuffer& _IndexBuffer)
{
  if (ID_ != _IndexBuffer.ID_)
  {
    if (!isEmpty())
    {
      Engine::Get<IBufferManager>()->Release(ID_);
    }
    ID_ = _IndexBuffer.ID_;
    if (!isEmpty())
    {
      Engine::Get<IBufferManager>()->AddRef(ID_);
    }
  }

  return *this;
}

IndexBuffer& IndexBuffer::operator=(IndexBuffer&& _IndexBuffer)noexcept
{
  if (!isEmpty())
  {
    Engine::Get<IBufferManager>()->Release(ID_);
  }

  ID_ = _IndexBuffer.ID_;
  _IndexBuffer.ID_ = Constants::kDisableID;

  return *this;
}

IndexBuffer::~IndexBuffer()
{
  if (!isEmpty())
  {
    Engine::Get<IBufferManager>()->Release(ID_);
  }
}
