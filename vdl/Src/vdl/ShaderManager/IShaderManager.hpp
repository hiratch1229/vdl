#pragma once
#include <vdl/ID.hpp>
#include <vdl/InputLayout.hpp>

#include <vdl/Shader/IShader.hpp>

class IShaderManager
{
public:
  static IShaderManager* Create();

  IShaderManager() = default;

  virtual ~IShaderManager() = default;

  virtual void Initialize() = 0;

  [[nodiscard]] virtual vdl::ID LoadFromFile(const char* _FilePath, const char* _EntryPoint, ShaderType _Type) = 0;

  //  頂点シェーダー用
  [[nodiscard]] virtual vdl::ID LoadFromFile(const char* _FilePath, const char* _EntryPoint, vdl::InputLayout _InputLayout) = 0;

  [[nodiscard]] virtual vdl::ID LoadFromMemory(const char* _Source, vdl::uint _DataSize, const char* _EntryPoint, ShaderType _Type) = 0;

  //  頂点シェーダー用
  [[nodiscard]] virtual vdl::ID LoadFromMemory(const char* _Source, vdl::uint _DataSize, const char* _EntryPoint, vdl::InputLayout _InputLayout) = 0;

  virtual void AddRef(const vdl::ID& _ID) = 0;

  virtual void Release(const vdl::ID& _ID) = 0;

  [[nodiscard]] virtual IShader* GetShader(const vdl::ID& _ID) = 0;
};
