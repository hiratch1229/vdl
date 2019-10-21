#include <vdl/Shader.hpp>

#include <vdl/Engine.hpp>
#include <vdl/ShaderManager/IShaderManager.hpp>

#include <vdl/Misc/Misc.hpp>

#include <filesystem>
#include <assert.h>

namespace vdl
{
  VertexShader::VertexShader(const char* _FilePath, InputLayoutType _InputLayout, const char* _EntryPoint)
  {
    _ASSERT_EXPR_A(std::filesystem::exists(std::filesystem::path(_FilePath)),
      std::string(std::string(_FilePath) + "が見つかりません。").c_str());

    ID_ = Engine::Get<IShaderManager>()->LoadFromFile(_FilePath, _EntryPoint, _InputLayout);
  }

  VertexShader::VertexShader(const char* _Source, uint _DataSize, InputLayoutType _InputLayout, const char* _EntryPoint)
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

  VertexShader::VertexShader(VertexShader&& _VertexShader)noexcept
  {
    ID_ = _VertexShader.ID_;
    _VertexShader.ID_ = std::nullopt;
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

  VertexShader& VertexShader::operator=(VertexShader&& _VertexShader)noexcept
  {
    if (ID_)
    {
      Engine::Get<IShaderManager>()->Release(ID_);
    }

    ID_ = _VertexShader.ID_;
    _VertexShader.ID_ = std::nullopt;

    return *this;
  }

  VertexShader::~VertexShader()
  {
    if (ID_)
    {
      Engine::Get<IShaderManager>()->Release(ID_);
    }
  }

  InputLayoutType VertexShader::GetInputLayout()const
  {
    assert(!isEmpty());
    assert(Engine::Get<IShaderManager>()->GetShader(ID_)->GetType() == ShaderType::eVertexShader);
    return static_cast<IVertexShader*>(Engine::Get<IShaderManager>()->GetShader(ID_))->GetInputLayout();
  }

  //--------------------------------------------------

  HullShader::HullShader(const char* _FilePath, const char* _EntryPoint)
  {
    _ASSERT_EXPR_A(std::filesystem::exists(std::filesystem::path(_FilePath)),
      std::string(std::string(_FilePath) + "が見つかりません。").c_str());

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

  HullShader::HullShader(HullShader&& _HullShader)noexcept
  {
    ID_ = _HullShader.ID_;
    _HullShader.ID_ = std::nullopt;
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

  HullShader& HullShader::operator=(HullShader&& _HullShader)noexcept
  {
    if (ID_)
    {
      Engine::Get<IShaderManager>()->Release(ID_);
    }

    ID_ = _HullShader.ID_;
    _HullShader.ID_ = std::nullopt;

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
    _ASSERT_EXPR_A(std::filesystem::exists(std::filesystem::path(_FilePath)),
      std::string(std::string(_FilePath) + "が見つかりません。").c_str());

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

  DomainShader::DomainShader(DomainShader&& _DomainShader)noexcept
  {
    ID_ = _DomainShader.ID_;
    _DomainShader.ID_ = std::nullopt;
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

  DomainShader& DomainShader::operator=(DomainShader&& _DomainShader)noexcept
  {
    if (ID_)
    {
      Engine::Get<IShaderManager>()->Release(ID_);
    }

    ID_ = _DomainShader.ID_;
    _DomainShader.ID_ = std::nullopt;

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
    _ASSERT_EXPR_A(std::filesystem::exists(std::filesystem::path(_FilePath)),
      std::string(std::string(_FilePath) + "が見つかりません。").c_str());

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

  GeometryShader::GeometryShader(GeometryShader&& _GeometryShader)noexcept
  {
    ID_ = _GeometryShader.ID_;
    _GeometryShader.ID_ = std::nullopt;
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

  GeometryShader& GeometryShader::operator=(GeometryShader&& _GeometryShader)noexcept
  {
    if (ID_)
    {
      Engine::Get<IShaderManager>()->Release(ID_);
    }

    ID_ = _GeometryShader.ID_;
    _GeometryShader.ID_ = std::nullopt;

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
    _ASSERT_EXPR_A(std::filesystem::exists(std::filesystem::path(_FilePath)),
      std::string(std::string(_FilePath) + "が見つかりません。").c_str());

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

  PixelShader::PixelShader(PixelShader&& _PixelShader)noexcept
  {
    ID_ = _PixelShader.ID_;
    _PixelShader.ID_ = std::nullopt;
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

  PixelShader& PixelShader::operator=(PixelShader&& _PixelShader)noexcept
  {
    if (ID_)
    {
      Engine::Get<IShaderManager>()->Release(ID_);
    }

    ID_ = _PixelShader.ID_;
    _PixelShader.ID_ = std::nullopt;

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
    _ASSERT_EXPR_A(std::filesystem::exists(std::filesystem::path(_FilePath)),
      std::string(std::string(_FilePath) + "が見つかりません。").c_str());

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

  ComputeShader::ComputeShader(ComputeShader&& _ComputeShader)noexcept
  {
    ID_ = _ComputeShader.ID_;
    _ComputeShader.ID_ = std::nullopt;
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

  ComputeShader& ComputeShader::operator=(ComputeShader&& _ComputeShader)noexcept
  {
    if (ID_)
    {
      Engine::Get<IShaderManager>()->Release(ID_);
    }

    ID_ = _ComputeShader.ID_;
    _ComputeShader.ID_ = std::nullopt;

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
