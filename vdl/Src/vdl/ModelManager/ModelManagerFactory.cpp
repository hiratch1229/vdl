#include "IModelManager.hpp"

#include "CModelManager.hpp"

IModelManager* IModelManager::Create()
{
  return new CModelManager;
}
