#include "IShaderManager.hpp"

#include "CShaderManager.hpp"

IShaderManager* IShaderManager::Create()
{
  return new CShaderManager;
}
