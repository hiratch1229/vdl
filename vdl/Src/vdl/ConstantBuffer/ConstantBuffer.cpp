#include <vdl/ConstantBuffer.hpp>

#include <vdl/Engine.hpp>
#include <vdl/BufferManager/IBufferManager.hpp>

namespace vdl::Detail
{
  ConstantBufferData::ConstantBufferData(uint _BufferSize)
    : ID_(Engine::Get<IBufferManager>()->CreateConstantBuffer(_BufferSize))
  {
  }

  ConstantBufferData::ConstantBufferData(const ConstantBufferData& _ConstantBufferData)
  {
    if (ID_ = _ConstantBufferData.ID_)
    {
      Engine::Get<IBufferManager>()->AddRef(ID_);
    }
  }

  ConstantBufferData::~ConstantBufferData()
  {
    if (ID_)
    {
      Engine::Get<IBufferManager>()->Release(ID_);
    }
  }

  ConstantBufferData& ConstantBufferData::operator=(const ConstantBufferData& _ConstantBufferData)
  {
    IBufferManager* pBufferManager = Engine::Get<IBufferManager>();

    if (ID_ != _ConstantBufferData.ID_)
    {
      if (ID_)
      {
        pBufferManager->Release(ID_);
      }

      if (ID_ = _ConstantBufferData.ID_)
      {
        pBufferManager->AddRef(ID_);
      }
    }

    return *this;
  }

  void* ConstantBufferData::GetData()const
  {
    return static_cast<IConstantBuffer*>(Engine::Get<IBufferManager>()->GetBuffer(ID_))->GetData();
  }

  uint ConstantBufferData::GetSize()const
  {
    return static_cast<IConstantBuffer*>(Engine::Get<IBufferManager>()->GetBuffer(ID_))->GetSize();
  }
}
