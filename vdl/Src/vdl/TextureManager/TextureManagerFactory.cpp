#include "ITextureManager.hpp"

#include "CTextureManager.hpp"

ITextureManager* ITextureManager::Create()
{
  return new CTextureManager;
}
