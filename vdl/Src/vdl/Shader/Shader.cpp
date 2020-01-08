#include <vdl/Shader.hpp>

#include <vdl/Engine.hpp>
#include <vdl/ShaderManager/IShaderManager.hpp>

#include <assert.h>

namespace
{
  template<class Shader>
  inline Shader* Cast(IShader* _pShader)
  {
    static_assert(std::is_base_of<IShader, Shader>::value);

    if constexpr (std::is_same<IVertexShader, Shader>::value)
    {
      assert(_pShader->GetType() == ShaderType::eVertexShader);
    }

    return static_cast<Shader*>(_pShader);
  }
}

namespace vdl
{
  VertexShader::VertexShader(const char* _FilePath, InputLayoutType _InputLayout, const char* _EntryPoint)
  {
    ID_ = Engine::Get<IShaderManager>()->LoadFromFile(_FilePath, _EntryPoint, _InputLayout);
  }

  VertexShader::VertexShader(const char* _Source, uint _DataSize, InputLayoutType _InputLayout, const char* _EntryPoint)
  {
    ID_ = Engine::Get<IShaderManager>()->LoadFromMemory(_Source, _DataSize, _EntryPoint, _InputLayout);
  }

  VertexShader::VertexShader(const VertexShader& _VertexShader)
  {
    ID_ = _VertexShader.ID_;
    if (!isEmpty())
    {
      Engine::Get<IShaderManager>()->AddRef(ID_);
    }
  }

  VertexShader::VertexShader(VertexShader&& _VertexShader)noexcept
  {
    ID_ = _VertexShader.ID_;
    _VertexShader.ID_ = Constants::kDisableID;
  }

  VertexShader& VertexShader::operator=(const VertexShader& _VertexShader)
  {
    if (ID_ != _VertexShader.ID_)
    {
      if (!isEmpty())
      {
        Engine::Get<IShaderManager>()->Release(ID_);
      }
      ID_ = _VertexShader.ID_;
      if (!isEmpty())
      {
        Engine::Get<IShaderManager>()->AddRef(ID_);
      }
    }

    return *this;
  }

  VertexShader& VertexShader::operator=(VertexShader&& _VertexShader)noexcept
  {
    if (!isEmpty())
    {
      Engine::Get<IShaderManager>()->Release(ID_);
    }

    ID_ = _VertexShader.ID_;
    _VertexShader.ID_ = Constants::kDisableID;

    return *this;
  }

  VertexShader::~VertexShader()
  {
    if (!isEmpty() && Engine::isActive())
    {
      Engine::Get<IShaderManager>()->Release(ID_);
    }
  }

  InputLayoutType VertexShader::GetInputLayout()const
  {
    assert(!isEmpty());

    return Cast<IVertexShader>(Engine::Get<IShaderManager>()->GetShader(ID_))->GetInputLayout();
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
    ID_ = _HullShader.ID_;
    if (!isEmpty())
    {
      Engine::Get<IShaderManager>()->AddRef(ID_);
    }
  }

  HullShader::HullShader(HullShader&& _HullShader)noexcept
  {
    ID_ = _HullShader.ID_;
    _HullShader.ID_ = Constants::kDisableID;
  }

  HullShader& HullShader::operator=(const HullShader& _HullShader)
  {
    if (ID_ != _HullShader.ID_)
    {
      if (!isEmpty())
      {
        Engine::Get<IShaderManager>()->Release(ID_);
      }
      ID_ = _HullShader.ID_;
      if (!isEmpty())
      {
        Engine::Get<IShaderManager>()->AddRef(ID_);
      }
    }

    return *this;
  }

  HullShader& HullShader::operator=(HullShader&& _HullShader)noexcept
  {
    if (!isEmpty())
    {
      Engine::Get<IShaderManager>()->Release(ID_);
    }

    ID_ = _HullShader.ID_;
    _HullShader.ID_ = Constants::kDisableID;

    return *this;
  }

  HullShader::~HullShader()
  {
    if (!isEmpty() && Engine::isActive())
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
    ID_ = _DomainShader.ID_;
    if (!isEmpty())
    {
      Engine::Get<IShaderManager>()->AddRef(ID_);
    }
  }

  DomainShader::DomainShader(DomainShader&& _DomainShader)noexcept
  {
    ID_ = _DomainShader.ID_;
    _DomainShader.ID_ = Constants::kDisableID;
  }

  DomainShader& DomainShader::operator=(const DomainShader& _DomainShader)
  {
    if (ID_ != _DomainShader.ID_)
    {
      if (!isEmpty())
      {
        Engine::Get<IShaderManager>()->Release(ID_);
      }
      ID_ = _DomainShader.ID_;
      if (!isEmpty())
      {
        Engine::Get<IShaderManager>()->AddRef(ID_);
      }
    }

    return *this;
  }

  DomainShader& DomainShader::operator=(DomainShader&& _DomainShader)noexcept
  {
    if (!isEmpty())
    {
      Engine::Get<IShaderManager>()->Release(ID_);
    }

    ID_ = _DomainShader.ID_;
    _DomainShader.ID_ = Constants::kDisableID;

    return *this;
  }

  DomainShader::~DomainShader()
  {
    if (!isEmpty() && Engine::isActive())
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
    ID_ = _GeometryShader.ID_;
    if (!isEmpty())
    {
      Engine::Get<IShaderManager>()->AddRef(ID_);
    }
  }

  GeometryShader::GeometryShader(GeometryShader&& _GeometryShader)noexcept
  {
    ID_ = _GeometryShader.ID_;
    _GeometryShader.ID_ = Constants::kDisableID;
  }

  GeometryShader& GeometryShader::operator=(const GeometryShader& _GeometryShader)
  {
    if (ID_ != _GeometryShader.ID_)
    {
      if (!isEmpty())
      {
        Engine::Get<IShaderManager>()->Release(ID_);
      }
      ID_ = _GeometryShader.ID_;
      if (!isEmpty())
      {
        Engine::Get<IShaderManager>()->AddRef(ID_);
      }
    }

    return *this;
  }

  GeometryShader& GeometryShader::operator=(GeometryShader&& _GeometryShader)noexcept
  {
    if (!isEmpty())
    {
      Engine::Get<IShaderManager>()->Release(ID_);
    }

    ID_ = _GeometryShader.ID_;
    _GeometryShader.ID_ = Constants::kDisableID;

    return *this;
  }

  GeometryShader::~GeometryShader()
  {
    if (!isEmpty() && Engine::isActive())
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
    ID_ = _PixelShader.ID_;
    if (!isEmpty())
    {
      Engine::Get<IShaderManager>()->AddRef(ID_);
    }
  }

  PixelShader::PixelShader(PixelShader&& _PixelShader)noexcept
  {
    ID_ = _PixelShader.ID_;
    _PixelShader.ID_ = Constants::kDisableID;
  }

  PixelShader& PixelShader::operator=(const PixelShader& _PixelShader)
  {
    if (ID_ != _PixelShader.ID_)
    {
      if (!isEmpty())
      {
        Engine::Get<IShaderManager>()->Release(ID_);
      }
      ID_ = _PixelShader.ID_;
      if (!isEmpty())
      {
        Engine::Get<IShaderManager>()->AddRef(ID_);
      }
    }

    return *this;
  }

  PixelShader& PixelShader::operator=(PixelShader&& _PixelShader)noexcept
  {
    if (!isEmpty())
    {
      Engine::Get<IShaderManager>()->Release(ID_);
    }

    ID_ = _PixelShader.ID_;
    _PixelShader.ID_ = Constants::kDisableID;

    return *this;
  }

  PixelShader::~PixelShader()
  {
    if (!isEmpty() && Engine::isActive())
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
    ID_ = _ComputeShader.ID_;
    if (!isEmpty())
    {
      Engine::Get<IShaderManager>()->AddRef(ID_);
    }
  }

  ComputeShader::ComputeShader(ComputeShader&& _ComputeShader)noexcept
  {
    ID_ = _ComputeShader.ID_;
    _ComputeShader.ID_ = Constants::kDisableID;
  }

  ComputeShader& ComputeShader::operator=(const ComputeShader& _ComputeShader)
  {
    if (ID_ != _ComputeShader.ID_)
    {
      if (!isEmpty())
      {
        Engine::Get<IShaderManager>()->Release(ID_);
      }
      ID_ = _ComputeShader.ID_;
      if (!isEmpty())
      {
        Engine::Get<IShaderManager>()->AddRef(ID_);
      }
    }

    return *this;
  }

  ComputeShader& ComputeShader::operator=(ComputeShader&& _ComputeShader)noexcept
  {
    if (!isEmpty())
    {
      Engine::Get<IShaderManager>()->Release(ID_);
    }

    ID_ = _ComputeShader.ID_;
    _ComputeShader.ID_ = Constants::kDisableID;

    return *this;
  }

  ComputeShader::~ComputeShader()
  {
    if (!isEmpty() && Engine::isActive())
    {
      Engine::Get<IShaderManager>()->Release(ID_);
    }
  }
}
