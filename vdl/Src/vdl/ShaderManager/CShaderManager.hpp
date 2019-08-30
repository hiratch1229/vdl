#pragma once
#include "IShaderManager.hpp"

#include <vdl/ReferenceCount/ReferenceCount.hpp>

class IDevice;

class CShaderManager : public IShaderManager
{
  IDevice* pDevice_;
private:
  ReferenceCounts<IShader> Shaders_;
public:
  CShaderManager() = default;

  void Initialize()override;

  ID LoadFromFile(const char* _FilePath, const char* _EntryPoint, ShaderType _Type)override;

  //  ���_�V�F�[�_�[�p
  ID LoadFromFile(const char* _FilePath, const char* _EntryPoint, vdl::InputLayout _InputLayout)override;

  ID LoadFromMemory(const char* _Source, vdl::uint _DataSize, const char* _EntryPoint, ShaderType _Type)override;

  //  ���_�V�F�[�_�[�p
  ID LoadFromMemory(const char* _Source, vdl::uint _DataSize, const char* _EntryPoint, vdl::InputLayout _InputLayout)override;

  void AddRef(const ID& _ID)override { Shaders_.Get(_ID).AddRef(); }

  void Release(const ID& _ID)override { Shaders_.Get(_ID).Release(); }

  IShader* GetShader(const ID& _ID)override { return Shaders_.Get(_ID).Get(); }
};
