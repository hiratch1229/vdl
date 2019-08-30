#include "CShaderManager.hpp"

#include <vdl/Engine.hpp>
#include <vdl/Device/IDevice.hpp>
#include <vdl/Misc/Windows/Misc.hpp>

#include <string>
#include <fstream>

void CShaderManager::Initialize()
{
  pDevice_ = Engine::Get<IDevice>();
}

ID CShaderManager::LoadFromFile(const char* _FilePath, const char* _EntryPoint, ShaderType _Type)
{
  //  シェーダーファイルのロード
  std::string Data;
  {
    std::ifstream IStream(_FilePath);
    _ASSERT_EXPR_A(IStream, (std::string(_FilePath) + "が存在しません。").c_str());

    IStream.seekg(0, std::ios::end);
    Data.reserve(static_cast<std::string::size_type>(IStream.tellg()));
    IStream.seekg(0, std::ios::beg);
    Data.assign(std::istreambuf_iterator<char>(IStream), std::istreambuf_iterator<char>());
  }

  return LoadFromMemory(Data.data(), static_cast<vdl::uint>(Data.size()), _EntryPoint, _Type);
}

ID CShaderManager::LoadFromFile(const char* _FilePath, const char* _EntryPoint, vdl::InputLayout _InputLayout)
{


  return LoadFromFile(_FilePath, _EntryPoint, ShaderType::eVertexShader);
}

ID CShaderManager::LoadFromMemory(const char* _Source, vdl::uint _DataSize, const char* _EntryPoint, ShaderType _Type)
{
  IShader* pShader;
  pDevice_->LoadShader(&pShader, _Source, _DataSize, _EntryPoint, _Type);

  return Shaders_.Add(pShader);
}

ID CShaderManager::LoadFromMemory(const char* _Source, vdl::uint _DataSize, const char* _EntryPoint, vdl::InputLayout _InputLayout)
{
  return LoadFromMemory(_Source, _DataSize, _EntryPoint, ShaderType::eVertexShader);
}
