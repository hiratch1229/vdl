#include "CShaderManager.hpp"

#include <vdl/Engine.hpp>
#include <vdl/Device/IDevice.hpp>
#include <vdl/Misc/Windows/Misc.hpp>

#include <string>
#include <fstream>

namespace
{
  //  シェーダーファイルのロード
  std::string LoadShaderFile(const char* _FilePath)
  {
    std::string Data;
    {
      std::ifstream IStream(_FilePath);
      _ASSERT_EXPR_A(IStream, (std::string(_FilePath) + "が存在しません。").c_str());

      IStream.seekg(0, std::ios::end);
      Data.reserve(static_cast<std::string::size_type>(IStream.tellg()));
      IStream.seekg(0, std::ios::beg);
      Data.assign(std::istreambuf_iterator<char>(IStream), std::istreambuf_iterator<char>());
    }

    return Data;
  }
}

void CShaderManager::Initialize()
{
  pDevice_ = Engine::Get<IDevice>();
}

ID CShaderManager::LoadFromFile(const char* _FilePath, const char* _EntryPoint, ShaderType _Type)
{
  std::string Data = std::move(LoadShaderFile(_FilePath));

  return LoadFromMemory(Data.data(), static_cast<vdl::uint>(Data.size()), _EntryPoint, _Type);
}

ID CShaderManager::LoadFromFile(const char* _FilePath, const char* _EntryPoint, vdl::RenderType _InputLayout)
{
  std::string Data = std::move(LoadShaderFile(_FilePath));

  return LoadFromMemory(Data.data(), static_cast<vdl::uint>(Data.size()), _EntryPoint, _InputLayout);
}

ID CShaderManager::LoadFromMemory(const char* _Source, vdl::uint _DataSize, const char* _EntryPoint, ShaderType _Type)
{
  IShader* pShader;
  pDevice_->LoadShader(&pShader, _Source, _DataSize, _EntryPoint, _Type);

  return Shaders_.Add(pShader);
}

ID CShaderManager::LoadFromMemory(const char* _Source, vdl::uint _DataSize, const char* _EntryPoint, vdl::RenderType _InputLayout)
{
  IVertexShader* pVertexShader;
  pDevice_->LoadShader(&pVertexShader, _Source, _DataSize, _EntryPoint, _InputLayout);

  return Shaders_.Add(pVertexShader);
}
