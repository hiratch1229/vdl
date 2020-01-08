#pragma once
#include "IShaderManager.hpp"

#include <vdl/ReferenceCount/ReferenceCount.hpp>

class IDevice;

class CShaderManager : public IShaderManager
{
  IDevice* pDevice_;
  ReferenceCounts<IShader> Shaders_;
public:
  CShaderManager() = default;

  PlatformType GetPlatform()const final { return PlatformType::eCommon; }

  void Initialize()override;

  vdl::ID LoadFromFile(const char* _FilePath, const char* _EntryPoint, ShaderType _Type)override;

  //  ���_�V�F�[�_�[�p
  vdl::ID LoadFromFile(const char* _FilePath, const char* _EntryPoint, vdl::InputLayoutType _InputLayout)override;

  vdl::ID LoadFromMemory(const char* _Source, vdl::uint _DataSize, const char* _EntryPoint, ShaderType _Type)override;

  //  ���_�V�F�[�_�[�p
  vdl::ID LoadFromMemory(const char* _Source, vdl::uint _DataSize, const char* _EntryPoint, vdl::InputLayoutType _InputLayout)override;

  void AddRef(vdl::ID _ID)override { Shaders_.Get(_ID).AddRef(); }

  void Release(vdl::ID _ID)override { Shaders_.Get(_ID).Release(); }

  IShader* GetShader(vdl::ID _ID)override { return Shaders_.Get(_ID).Get(); }
};
