#include <vdl/ConstantBuffer.hpp>

#include <vdl/Engine.hpp>
#include <vdl/BufferManager/IBufferManager.hpp>

#include <assert.h>

namespace vdl::Detail
{
  ConstantBufferData::ConstantBufferData(uint _BufferSize)
  {
    ID_ = Engine::Get<IBufferManager>()->CreateConstantBuffer(_BufferSize);
  }

  ConstantBufferData::ConstantBufferData(const ConstantBufferData& _ConstantBufferData)
  {
    ID_ = _ConstantBufferData.ID_;
    if (!isEmpty())
    {
      Engine::Get<IBufferManager>()->AddRef(ID_);
    }
  }

  ConstantBufferData::ConstantBufferData(ConstantBufferData&& _ConstantBufferData)noexcept
  {
    ID_ = _ConstantBufferData.ID_;
    _ConstantBufferData.ID_ = Constants::kDisableID;
  }

  ConstantBufferData& ConstantBufferData::operator=(const ConstantBufferData& _ConstantBufferData)
  {
    IBufferManager* pBufferManager = Engine::Get<IBufferManager>();

    if (ID_ != _ConstantBufferData.ID_)
    {
      if (!isEmpty())
      {
        pBufferManager->Release(ID_);
      }
      ID_ = _ConstantBufferData.ID_;
      if (!isEmpty())
      {
        pBufferManager->AddRef(ID_);
      }
    }

    return *this;
  }

  ConstantBufferData& ConstantBufferData::operator=(ConstantBufferData&& _ConstantBufferData)noexcept
  {
    if (!isEmpty())
    {
      Engine::Get<IBufferManager>()->Release(ID_);
    }

    ID_ = _ConstantBufferData.ID_;
    _ConstantBufferData.ID_ = Constants::kDisableID;

    return *this;
  }

  ConstantBufferData::~ConstantBufferData()
  {
    if (!isEmpty() && Engine::isActive())
    {
      Engine::Get<IBufferManager>()->Release(ID_);
    }
  }

  void* ConstantBufferData::GetData()const
  {
    assert(!isEmpty());

    return Engine::Get<IBufferManager>()->GetBuffer(*this);
  }

  uint ConstantBufferData::GetBufferSize()const
  {
    assert(!isEmpty());

    return Engine::Get<IBufferManager>()->GetBufferSize(*this);
  }
}
