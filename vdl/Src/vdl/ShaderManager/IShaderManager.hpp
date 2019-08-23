#pragma once
#include <vdl/ID.hpp>

#include <vdl/Shader/IShader.hpp>

class IShaderManager
{
public:
  static IShaderManager* Create();

  IShaderManager() = default;

  virtual ~IShaderManager() = default;

  virtual void Initialize() = 0;

  virtual ID Load(const char* _FilePath, const char* _EntryPoint, ShaderType _Type) = 0;
};
