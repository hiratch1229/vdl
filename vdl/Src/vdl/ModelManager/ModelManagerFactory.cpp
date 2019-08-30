#include "IModelManager.hpp"

#include <vdl/Platform.hpp>

#if defined VDL_TARGET_WINDOWS
#include "Windows/CModelManager.hpp"
#endif

IModelManager* IModelManager::Create()
{
  return new CModelManager;
}
