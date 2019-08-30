#include "ITextureManager.hpp"

#include <vdl/Platform.hpp>

#if defined VDL_TARGET_WINDOWS
#include "Windows/CTextureManager.hpp"
#endif

ITextureManager* ITextureManager::Create()
{
  return new CTextureManager;
}
