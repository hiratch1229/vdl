#include <vdl/Shader.hpp>

#include <vdl/Engine.hpp>
#include <vdl/ShaderManager/IShaderManager.hpp>

#include <assert.h>

namespace vdl
{
  VertexShader::VertexShader(const char* _FilePath, InputLayout _InputLayout, const char* _EntryPoint)
  {
    ID_ = Engine::Get<IShaderManager>()->LoadFromFile(_FilePath, _EntryPoint, _InputLayout);
  }

  VertexShader::VertexShader(const char* _Source, uint _DataSize, InputLayout _InputLayout, const char* _EntryPoint)
  {
    ID_ = Engine::Get<IShaderManager>()->LoadFromMemory(_Source, _DataSize, _EntryPoint, _InputLayout);
  }

  VertexShader::VertexShader(const VertexShader& _VertexShader)
  {
    if (ID_ = _VertexShader.ID_)
    {
      Engine::Get<IShaderManager>()->AddRef(ID_);
    }
  }

  VertexShader& VertexShader::operator=(const VertexShader& _VertexShader)
  {
    if (ID_ != _VertexShader.ID_)
    {
      if (ID_)
      {
        Engine::Get<IShaderManager>()->Release(ID_);
      }
      if (ID_ = _VertexShader.ID_)
      {
        Engine::Get<IShaderManager>()->AddRef(ID_);
      }
    }

    return *this;
  }

  VertexShader::~VertexShader()
  {
    if (ID_)
    {
      Engine::Get<IShaderManager>()->Release(ID_);
    }
  }

  InputLayout VertexShader::GetInputLayout()const
  {
    assert(Engine::Get<IShaderManager>()->GetShader(ID_)->GetType() == ShaderType::eVertexShader);

    return static_cast<IVertexShader*>(Engine::Get<IShaderManager>()->GetShader(ID_))->GetInputLayout();
  }

  //--------------------------------------------------

  HullShader::HullShader(const char* _FilePath, const char* _EntryPoint)
  {
    ID_ = Engine::Get<IShaderManager>()->LoadFromFile(_FilePath, _EntryPoint, ShaderType::eHullShader);
  }

  HullShader::HullShader(const char* _Source, uint _DataSize, const char* _EntryPoint)
  {
    ID_ = Engine::Get<IShaderManager>()->LoadFromMemory(_Source, _DataSize, _EntryPoint, ShaderType::eHullShader);
  }

  HullShader::HullShader(const HullShader& _HullShader)
  {
    if (ID_ = _HullShader.ID_)
    {
      Engine::Get<IShaderManager>()->AddRef(ID_);
    }
  }

  HullShader& HullShader::operator=(const HullShader& _HullShader)
  {
    if (ID_ != _HullShader.ID_)
    {
      if (ID_)
      {
        Engine::Get<IShaderManager>()->Release(ID_);
      }
      if (ID_ = _HullShader.ID_)
      {
        Engine::Get<IShaderManager>()->AddRef(ID_);
      }
    }

    return *this;
  }

  HullShader::~HullShader()
  {
    if (ID_)
    {
      Engine::Get<IShaderManager>()->Release(ID_);
    }
  }

  //--------------------------------------------------

  DomainShader::DomainShader(const char* _FilePath, const char* _EntryPoint)
  {
    ID_ = Engine::Get<IShaderManager>()->LoadFromFile(_FilePath, _EntryPoint, ShaderType::eDomainShader);
  }

  DomainShader::DomainShader(const char* _Source, uint _DataSize, const char* _EntryPoint)
  {
    ID_ = Engine::Get<IShaderManager>()->LoadFromMemory(_Source, _DataSize, _EntryPoint, ShaderType::eDomainShader);
  }

  DomainShader::DomainShader(const DomainShader& _DomainShader)
  {
    if (ID_ = _DomainShader.ID_)
    {
      Engine::Get<IShaderManager>()->AddRef(ID_);
    }
  }

  DomainShader& DomainShader::operator=(const DomainShader& _DomainShader)
  {
    if (ID_ != _DomainShader.ID_)
    {
      if (ID_)
      {
        Engine::Get<IShaderManager>()->Release(ID_);
      }
      if (ID_ = _DomainShader.ID_)
      {
        Engine::Get<IShaderManager>()->AddRef(ID_);
      }
    }

    return *this;
  }

  DomainShader::~DomainShader()
  {
    if (ID_)
    {
      Engine::Get<IShaderManager>()->Release(ID_);
    }
  }

  //--------------------------------------------------

  GeometryShader::GeometryShader(const char* _FilePath, const char* _EntryPoint)
  {
    ID_ = Engine::Get<IShaderManager>()->LoadFromFile(_FilePath, _EntryPoint, ShaderType::eGeometryShader);
  }

  GeometryShader::GeometryShader(const char* _Source, uint _DataSize, const char* _EntryPoint)
  {
    ID_ = Engine::Get<IShaderManager>()->LoadFromMemory(_Source, _DataSize, _EntryPoint, ShaderType::eGeometryShader);
  }

  GeometryShader::GeometryShader(const GeometryShader& _GeometryShader)
  {
    if (ID_ = _GeometryShader.ID_)
    {
      Engine::Get<IShaderManager>()->AddRef(ID_);
    }
  }

  GeometryShader& GeometryShader::operator=(const GeometryShader& _GeometryShader)
  {
    if (ID_ != _GeometryShader.ID_)
    {
      if (ID_)
      {
        Engine::Get<IShaderManager>()->Release(ID_);
      }
      if (ID_ = _GeometryShader.ID_)
      {
        Engine::Get<IShaderManager>()->AddRef(ID_);
      }
    }

    return *this;
  }

  GeometryShader::~GeometryShader()
  {
    if (ID_)
    {
      Engine::Get<IShaderManager>()->Release(ID_);
    }
  }

  //--------------------------------------------------

  PixelShader::PixelShader(const char* _FilePath, const char* _EntryPoint)
  {
    ID_ = Engine::Get<IShaderManager>()->LoadFromFile(_FilePath, _EntryPoint, ShaderType::ePixelShader);
  }

  PixelShader::PixelShader(const char* _Source, uint _DataSize, const char* _EntryPoint)
  {
    ID_ = Engine::Get<IShaderManager>()->LoadFromMemory(_Source, _DataSize, _EntryPoint, ShaderType::ePixelShader);
  }

  PixelShader::PixelShader(const PixelShader& _PixelShader)
  {
    if (ID_ = _PixelShader.ID_)
    {
      Engine::Get<IShaderManager>()->AddRef(ID_);
    }
  }

  PixelShader& PixelShader::operator=(const PixelShader& _PixelShader)
  {
    if (ID_ != _PixelShader.ID_)
    {
      if (ID_)
      {
        Engine::Get<IShaderManager>()->Release(ID_);
      }
      if (ID_ = _PixelShader.ID_)
      {
        Engine::Get<IShaderManager>()->AddRef(ID_);
      }
    }

    return *this;
  }

  PixelShader::~PixelShader()
  {
    if (ID_)
    {
      Engine::Get<IShaderManager>()->Release(ID_);
    }
  }

  //--------------------------------------------------

  ComputeShader::ComputeShader(const char* _FilePath, const char* _EntryPoint)
  {
    ID_ = Engine::Get<IShaderManager>()->LoadFromFile(_FilePath, _EntryPoint, ShaderType::eComputeShader);
  }

  ComputeShader::ComputeShader(const char* _Source, uint _DataSize, const char* _EntryPoint)
  {
    ID_ = Engine::Get<IShaderManager>()->LoadFromMemory(_Source, _DataSize, _EntryPoint, ShaderType::eComputeShader);
  }

  ComputeShader::ComputeShader(const ComputeShader& _ComputeShader)
  {
    if (ID_ = _ComputeShader.ID_)
    {
      Engine::Get<IShaderManager>()->AddRef(ID_);
    }
  }

  ComputeShader& ComputeShader::operator=(const ComputeShader& _ComputeShader)
  {
    if (ID_ != _ComputeShader.ID_)
    {
      if (ID_)
      {
        Engine::Get<IShaderManager>()->Release(ID_);
      }
      if (ID_ = _ComputeShader.ID_)
      {
        Engine::Get<IShaderManager>()->AddRef(ID_);
      }
    }

    return *this;
  }

  ComputeShader::~ComputeShader()
  {
    if (ID_)
    {
      Engine::Get<IShaderManager>()->Release(ID_);
    }
  }
}
