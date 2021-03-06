#include "ISoundManager.hpp"

#include <vdl/Platform.hpp>

#if defined VDL_TARGET_WINDOWS
#include "Windows/CSoundManager.hpp"
#elif defined VDL_TARGET_LINUX
#include "Linux/CSoundManager.hpp"
#endif

ISoundManager* ISoundManager::Create()
{
  return new CSoundManager;
}
