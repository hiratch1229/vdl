#include "IBufferManager.hpp"

#include "CBufferManager.hpp"

IBufferManager* IBufferManager::Create()
{
  return new CBufferManager;
}
