#include "CShaderManager.hpp"

#include <vdl/Engine.hpp>
#include <vdl/Device/IDevice.hpp>
#include <vdl/Misc/Windows/Misc.hpp>

#include <vdl/DetectMemoryLeak.hpp>

#include <string>
#include <fstream>

void CShaderManager::Initialize()
{
  pDevice_ = Engine::Get<IDevice>();
}

vdl::ID CShaderManager::LoadFromFile(const char* _FilePath, const char* _EntryPoint, ShaderType _Type)
{
  IShader* pShader;
  pDevice_->LoadShader(&pShader, _FilePath, _EntryPoint, _Type);

  return Shaders_.Add(pShader);
}

vdl::ID CShaderManager::LoadFromFile(const char* _FilePath, const char* _EntryPoint, vdl::InputLayoutType _InputLayout)
{
  IVertexShader* pVertexShader;
  pDevice_->LoadShader(&pVertexShader, _FilePath, _EntryPoint, _InputLayout);

  return Shaders_.Add(pVertexShader);
}

vdl::ID CShaderManager::LoadFromMemory(const char* _Source, vdl::uint _DataSize, const char* _EntryPoint, ShaderType _Type)
{
  IShader* pShader;
  pDevice_->LoadShader(&pShader, _Source, _DataSize, _EntryPoint, _Type);

  return Shaders_.Add(pShader);
}

vdl::ID CShaderManager::LoadFromMemory(const char* _Source, vdl::uint _DataSize, const char* _EntryPoint, vdl::InputLayoutType _InputLayout)
{
  IVertexShader* pVertexShader;
  pDevice_->LoadShader(&pVertexShader, _Source, _DataSize, _EntryPoint, _InputLayout);

  return Shaders_.Add(pVertexShader);
}
