#include "ISoundManager.hpp"

#include <vdl/Platform.hpp>

#if defined VDL_TARGET_WINDOWS
#include "Windows/CSoundManager.hpp"
#endif

ISoundManager* ISoundManager::Create()
{
  return CSoundManager;
}
