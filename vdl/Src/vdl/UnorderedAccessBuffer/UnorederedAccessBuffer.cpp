#include <vdl/UnorderedAccessBuffer.hpp>

#include <vdl/Engine.hpp>
#include <vdl/Device/IDevice.hpp>
#include <vdl/BufferManager/IBufferManager.hpp>

#include <assert.h>

namespace vdl::Detail
{
  UnorderedAccessBufferData::UnorderedAccessBufferData(uint _Stride, uint _BufferSize, void* _Buffer)
  {
    ID_ = Engine::Get<IBufferManager>()->CreateUnorderedAccessBuffer(_Stride, _BufferSize, _Buffer);
  }

  UnorderedAccessBufferData::UnorderedAccessBufferData(const UnorderedAccessBufferData& _UnorderedAccessBufferData)
  {
    if (ID_ = _UnorderedAccessBufferData.ID_)
    {
      Engine::Get<IBufferManager>()->AddRef(ID_);
    }
  }

  UnorderedAccessBufferData::UnorderedAccessBufferData(UnorderedAccessBufferData&& _UnorderedAccessBufferData)noexcept
  {
    ID_ = _UnorderedAccessBufferData.ID_;
    _UnorderedAccessBufferData.ID_ = std::nullopt;
  }

  UnorderedAccessBufferData& UnorderedAccessBufferData::operator=(const UnorderedAccessBufferData& _UnorderedAccessBufferData)
  {
    IBufferManager* pBufferManager = Engine::Get<IBufferManager>();

    if (ID_ != _UnorderedAccessBufferData.ID_)
    {
      if (ID_)
      {
        pBufferManager->Release(ID_);
      }

      if (ID_ = _UnorderedAccessBufferData.ID_)
      {
        pBufferManager->AddRef(ID_);
      }
    }

    return *this;
  }

  UnorderedAccessBufferData& UnorderedAccessBufferData::operator=(UnorderedAccessBufferData&& _UnorderedAccessBufferData)noexcept
  {
    if (ID_)
    {
      Engine::Get<IBufferManager>()->Release(ID_);
    }

    ID_ = _UnorderedAccessBufferData.ID_;
    _UnorderedAccessBufferData.ID_ = std::nullopt;

    return *this;
  }

  UnorderedAccessBufferData::~UnorderedAccessBufferData()
  {
    if (ID_)
    {
      Engine::Get<IBufferManager>()->Release(ID_);
    }
  }
}
