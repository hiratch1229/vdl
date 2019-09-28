#pragma once
#include "ID.hpp"
#include "Fwd.hpp"

namespace vdl
{
  class VertexShader
  {
    ID ID_;
  public:
    VertexShader() = default;

    VertexShader(VertexShader&&) = default;

    VertexShader(const char* _FilePath, InputLayout _InputLayout, const char* _EntryPoint = "main");

    VertexShader(const char* _Source, uint _DataSize, InputLayout _InputLayout, const char* _EntryPoint = "main");

    VertexShader(const VertexShader& _VertexShader);

    ~VertexShader();
  public:
    VertexShader& operator=(const VertexShader& _VertexShader);

    VertexShader& operator=(VertexShader&&) = default;

    [[nodiscard]] constexpr bool operator==(const VertexShader& _VertexShader)const noexcept { return ID_ == _VertexShader.ID_; }

    [[nodiscard]] constexpr bool operator!=(const VertexShader& _VertexShader)const noexcept { return ID_ != _VertexShader.ID_; }
  public:
    [[nodiscard]] ID GetID()const noexcept { return ID_; }

    [[nodiscard]] bool isEmpty()const noexcept { return ID_ ==std::nullopt; }

    [[nodiscard]] InputLayout GetInputLayout()const;
  };

  class HullShader
  {
    ID ID_;
  public:
    HullShader() = default;

    HullShader(HullShader&&) = default;

    HullShader(const char* _FilePath, const char* _EntryPoint = "main");
    
    HullShader(const char* _Source, uint _DataSize, const char* _EntryPoint = "main");

    HullShader(const HullShader& _HullShader);

    ~HullShader();
  public:
    HullShader& operator=(const HullShader& _HullShader);

    HullShader& operator=(HullShader&&) = default;

    [[nodiscard]] constexpr bool operator==(const HullShader& _HullShader)const noexcept { return ID_ == _HullShader.ID_; }

    [[nodiscard]] constexpr bool operator!=(const HullShader& _HullShader)const noexcept { return ID_ != _HullShader.ID_; }
  public:
    [[nodiscard]] ID GetID()const noexcept { return ID_; }

    [[nodiscard]] bool isEmpty()const noexcept { return ID_ == std::nullopt; }
  };

  class DomainShader
  {
    ID ID_;
  public:
    DomainShader() = default;

    DomainShader(DomainShader&&) = default;

    DomainShader(const char* _FilePath, const char* _EntryPoint = "main");

    DomainShader(const char* _Source, uint _DataSize, const char* _EntryPoint = "main");

    DomainShader(const DomainShader& _DomainShader);

    ~DomainShader();
  public:
    DomainShader& operator=(const DomainShader& _DomainShader);

    DomainShader& operator=(DomainShader&&) = default;

    [[nodiscard]] constexpr bool operator==(const DomainShader& _DomainShader)const noexcept { return ID_ == _DomainShader.ID_; }

    [[nodiscard]] constexpr bool operator!=(const DomainShader& _DomainShader)const noexcept { return ID_ != _DomainShader.ID_; }
  public:
    [[nodiscard]] ID GetID()const noexcept { return ID_; }

    [[nodiscard]] bool isEmpty()const noexcept { return ID_ == std::nullopt; }
  };

  class GeometryShader
  {
    ID ID_;
  public:
    GeometryShader() = default;

    GeometryShader(const char* _FilePath, const char* _EntryPoint = "main");

    GeometryShader(const char* _Source, uint _DataSize, const char* _EntryPoint = "main");

    GeometryShader(const GeometryShader& _GeometryShader);

    ~GeometryShader();
  public:
    GeometryShader& operator=(const GeometryShader& _GeometryShader);

    GeometryShader& operator=(GeometryShader&&) = default;

    [[nodiscard]] constexpr bool operator==(const GeometryShader& _GeometryShader)const noexcept { return ID_ == _GeometryShader.ID_; }

    [[nodiscard]] constexpr bool operator!=(const GeometryShader& _GeometryShader)const noexcept { return ID_ != _GeometryShader.ID_; }
  public:
    [[nodiscard]] ID GetID()const noexcept { return ID_; }

    [[nodiscard]] bool isEmpty()const noexcept { return ID_ == std::nullopt; }
  };

  class PixelShader
  {
    ID ID_;
  public:
    PixelShader() = default;
    
    PixelShader(PixelShader&&) = default;

    PixelShader(const char* _FilePath, const char* _EntryPoint = "main");

    PixelShader(const char* _Source, uint _DataSize, const char* _EntryPoint = "main");

    PixelShader(const PixelShader& _PixelShader);

    ~PixelShader();
  public:
    PixelShader& operator=(const PixelShader& _PixelShader);

    PixelShader& operator=(PixelShader&&) = default;

    [[nodiscard]] constexpr bool operator==(const PixelShader& _PixelShader)const noexcept { return ID_ == _PixelShader.ID_; }

    [[nodiscard]] constexpr bool operator!=(const PixelShader& _PixelShader)const noexcept { return ID_ != _PixelShader.ID_; }
  public:
    [[nodiscard]] ID GetID()const noexcept { return ID_; }

    [[nodiscard]] bool isEmpty()const noexcept { return ID_ == std::nullopt; }
  };

  class ComputeShader
  {
    ID ID_;
  public:
    ComputeShader() = default;

    ComputeShader(ComputeShader&&) = default;

    ComputeShader(const char* _FilePath, const char* _EntryPoint = "main");

    ComputeShader(const char* _Source, uint _DataSize, const char* _EntryPoint = "main");

    ComputeShader(const ComputeShader& _ComputeShader);

    ~ComputeShader();
  public:
    ComputeShader& operator=(const ComputeShader& _ComputeShader);

    ComputeShader& operator=(ComputeShader&&) = default;

    [[nodiscard]] constexpr bool operator==(const ComputeShader& _ComputeShader)const noexcept { return ID_ == _ComputeShader.ID_; }

    [[nodiscard]] constexpr bool operator!=(const ComputeShader& _ComputeShader)const noexcept { return ID_ != _ComputeShader.ID_; }
  public:
    [[nodiscard]] ID GetID()const noexcept { return ID_; }

    [[nodiscard]] bool isEmpty()const noexcept { return ID_ == std::nullopt; }
  };
}
