#pragma once
#include <vdl/ID.hpp>
#include <vdl/RenderType.hpp>

#include <vdl/Shader/IShader.hpp>

class IShaderManager
{
public:
  static IShaderManager* Create();

  IShaderManager() = default;

  virtual ~IShaderManager() = default;

  virtual void Initialize() = 0;

  virtual ID LoadFromFile(const char* _FilePath, const char* _EntryPoint, ShaderType _Type) = 0;

  //  頂点シェーダー用
  virtual ID LoadFromFile(const char* _FilePath, const char* _EntryPoint, vdl::RenderType _RenderType) = 0;

  virtual ID LoadFromMemory(const char* _Source, vdl::uint _DataSize, const char* _EntryPoint, ShaderType _Type) = 0;

  //  頂点シェーダー用
  virtual ID LoadFromMemory(const char* _Source, vdl::uint _DataSize, const char* _EntryPoint, vdl::RenderType _RenderType) = 0;

  virtual void AddRef(const ID& _ID) = 0;

  virtual void Release(const ID& _ID) = 0;

  virtual IShader* GetShader(const ID& _ID) = 0;
};
